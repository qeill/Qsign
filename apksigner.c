#include "apksigner_common.h"

/**
 * 显示帮助信息
 */
void show_help() {
    printf("APK一键签名工具 (终端版)\n");
    printf("使用方法: apksigner_cli [选项] <APK文件>\n\n");
    printf("选项:\n");
    printf("  -k <路径>    指定keystore文件路径\n");
    printf("  -a <别名>    指定keystore别名\n");
    printf("  -p <密码>    指定密钥库密码和密钥密码（使用相同密码）\n");
    printf("  -o <路径>    指定输出APK路径\n");
    printf("  -g           自动生成keystore\n");
    printf("  -v           显示详细信息\n");
    printf("  -h           显示帮助信息\n");
    printf("  -V <APK>     验证APK签名\n\n");
    printf("示例:\n");
    printf("  1. 使用已有keystore签名:\n");
    printf("     apksigner_cli -k my.keystore -a myalias -p 123456 app.apk\n\n");
    printf("  2. 自动生成keystore并签名:\n");
    printf("     apksigner_cli -g -p 123456 app.apk\n\n");
    printf("  3. 验证APK签名:\n");
    printf("     apksigner_cli -V app-signed.apk\n");
}

/**
 * 简化的命令行参数解析
 */
int parse_args(int argc, char *argv[], ApkSignerConfig *config, int *verify_mode, char *verify_apk) {
    int i = 1;
    
    while (i < argc) {
        if (strcmp(argv[i], "-k") == 0) {
            if (i + 1 < argc) {
                strncpy(config->keystore_path, argv[i+1], sizeof(config->keystore_path) - 1);
                i += 2;
            } else {
                printf("错误: -k选项需要指定路径\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            if (i + 1 < argc) {
                strncpy(config->alias, argv[i+1], sizeof(config->alias) - 1);
                i += 2;
            } else {
                printf("错误: -a选项需要指定别名\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                strncpy(config->store_pass, argv[i+1], sizeof(config->store_pass) - 1);
                strncpy(config->key_pass, argv[i+1], sizeof(config->key_pass) - 1);
                i += 2;
            } else {
                printf("错误: -p选项需要指定密码\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                strncpy(config->output_apk, argv[i+1], sizeof(config->output_apk) - 1);
                i += 2;
            } else {
                printf("错误: -o选项需要指定路径\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-g") == 0) {
            config->auto_gen_keystore = 1;
            i++;
        } else if (strcmp(argv[i], "-v") == 0) {
            config->verbose = 1;
            i++;
        } else if (strcmp(argv[i], "-h") == 0) {
            show_help();
            return 2;
        } else if (strcmp(argv[i], "-V") == 0) {
            *verify_mode = 1;
            if (i + 1 < argc) {
                strncpy(verify_apk, argv[i+1], sizeof(verify_apk) - 1);
                i += 2;
            } else {
                printf("错误: -V选项需要指定APK文件\n");
                return 1;
            }
        } else {
            // 处理APK文件路径
            if (i == argc - 1) {
                strncpy(config->apk_path, argv[i], sizeof(config->apk_path) - 1);
                i++;
            } else {
                printf("错误: 无效的参数: %s\n", argv[i]);
                show_help();
                return 1;
            }
        }
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    ApkSignerConfig config;
    int verify_mode = 0;
    char verify_apk[256] = {0};
    
    // 初始化配置
    init_config(&config);
    
    // 解析命令行参数
    int parse_ret = parse_args(argc, argv, &config, &verify_mode, verify_apk);
    if (parse_ret == 1) {
        return 1;
    } else if (parse_ret == 2) {
        return 0;
    }
    
    // 处理验证模式
    if (verify_mode) {
        if (strlen(verify_apk) == 0) {
            printf("错误: 验证模式需要指定APK文件\n");
            show_help();
            return 1;
        }
        
        printf("正在验证APK签名: %s\n", verify_apk);
        int ret = verify_apk_signature(verify_apk);
        if (ret == 0) {
            printf("验证成功: %s\n", verify_apk);
        } else {
            printf("验证失败: %s\n", verify_apk);
        }
        return ret;
    }
    
    // 检查APK文件路径是否已通过parse_args设置
    if (strlen(config.apk_path) == 0) {
        printf("错误: 请指定APK文件\n");
        show_help();
        return 1;
    }
    
    // 检查APK文件是否存在
    if (!file_exists(config.apk_path)) {
        printf("错误: APK文件不存在: %s\n", config.apk_path);
        return 1;
    }
    
    // 设置默认输出路径
    if (strlen(config.output_apk) == 0) {
        char *dot_pos = strrchr(config.apk_path, '.');
        if (dot_pos != NULL) {
            strncpy(config.output_apk, config.apk_path, dot_pos - config.apk_path);
            strcat(config.output_apk, "-signed.apk");
        } else {
            strcpy(config.output_apk, "signed.apk");
        }
    }
    
    // 处理自动生成keystore
    if (config.auto_gen_keystore) {
        // 如果没有指定keystore路径，生成默认路径
        if (strlen(config.keystore_path) == 0) {
            strcpy(config.keystore_path, DEFAULT_KEYSTORE_NAME);
        }
        
        // 检查keystore是否已存在
        if (file_exists(config.keystore_path)) {
            printf("警告: keystore文件已存在，将使用现有文件: %s\n", config.keystore_path);
            config.auto_gen_keystore = 0;
        }
    } else {
        // 非自动生成模式，必须指定keystore
        if (strlen(config.keystore_path) == 0) {
            printf("错误: 请指定keystore文件或使用-g选项自动生成\n");
            show_help();
            return 1;
        }
        
        // 检查keystore是否存在
        if (!file_exists(config.keystore_path)) {
            printf("错误: keystore文件不存在: %s\n", config.keystore_path);
            return 1;
        }
    }
    
    // 检查必要的密码
    if (strlen(config.store_pass) == 0 || strlen(config.key_pass) == 0) {
        printf("错误: 请指定密钥库密码和密钥密码\n");
        show_help();
        return 1;
    }
    
    printf("APK签名工具开始执行\n");
    printf("----------------------\n");
    printf("APK文件: %s\n", config.apk_path);
    printf("输出文件: %s\n", config.output_apk);
    printf("Keystore: %s\n", config.keystore_path);
    printf("别名: %s\n", config.alias);
    printf("自动生成keystore: %s\n", config.auto_gen_keystore ? "是" : "否");
    printf("----------------------\n");
    
    // 自动生成keystore
    if (config.auto_gen_keystore) {
        printf("正在生成keystore...\n");
        int ret = generate_keystore(&config);
        if (ret != 0) {
            printf("错误: 生成keystore失败\n");
            return ret;
        }
        printf("keystore生成成功: %s\n", config.keystore_path);
    }
    
    // 复制APK到输出路径（jarsigner会直接修改原文件）
    // 使用Windows API复制文件，避免依赖copy命令
    if (!CopyFileA(config.apk_path, config.output_apk, FALSE)) {
        printf("错误: 复制APK文件失败，错误码: %d\n", GetLastError());
        return 1;
    }
    
    // 使用输出APK进行签名
    char temp_apk[256];
    strcpy(temp_apk, config.apk_path);
    strcpy(config.apk_path, config.output_apk);
    
    // 签名APK
    printf("正在签名APK...\n");
    int ret = sign_apk(&config);
    
    // 恢复原APK路径
    strcpy(config.apk_path, temp_apk);
    
    if (ret == 0) {
        printf("\n签名成功！\n");
        printf("输入APK: %s\n", temp_apk);
        printf("输出APK: %s\n", config.output_apk);
    } else {
        printf("\n签名失败！\n");
    }
    
    return ret;
}
