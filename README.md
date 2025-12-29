# Qsign -  APK一键签名工具 - 原生版

## 程序简介

APK一键签名工具原生版是一个基于C语言和OpenSSL库开发的APK签名工具，不依赖Java的keytool和jarsigner工具，直接使用OpenSSL库实现RSA密钥生成、SHA哈希计算和APK签名功能。该工具提供了两种版本：


## 功能特点

- ✅ 不依赖Java工具包，独立运行
- ✅ 极其小巧便携只有88KB
- ✅ 基于OpenSSL库，支持现代加密算法
- ✅ 支持自动生成RSA密钥对
- ✅ 支持使用已有的私钥和公钥文件
- ✅ 支持自定义密钥别名和密码
- ✅ 支持验证APK签名
- ✅ 支持V1 JAR签名格式
- ✅ 支持SHA1和SHA256哈希算法
- ✅ 支持选择密钥长度（1024/2048/4096）
- ✅ 支持Windows、Linux、macOS等多平台
- ✅ 支持UTF-8编码的文件名

## 使用方法


#### 命令行参数

qsign.exe [选项] <APK文件路径>

选项：
  -h, --help          显示帮助信息
  -v, --version       显示版本信息
  -p, --private <路径> 使用指定的私钥文件（.pem格式）
  -u, --public <路径>  使用指定的公钥文件（.pem格式）
  -o, --output <路径> 输出签名后的APK路径
  -g, --generate      自动生成密钥对
  -f, --force         强制覆盖输出文件
  -a, --alias <别名>   密钥别名（用于生成密钥文件）
  -l, --length <长度>  密钥长度（1024/2048/4096，默认2048）
  -t, --verify        验证APK签名
  -c, --cert <路径>    公钥证书路径（用于验证签名）
```
示例用法
#windows用户
qsign.exe -g -o signed.apk app.apk
#Linux用户
./qsign -g -o signed.apk app.apk
#Linux运行报错请执行以下命令，为程序赋予权限
chmod +x qsign
```

2. **使用已有密钥文件签名APK**
   ```bash
   qsign.exe -p private.pem -u public.pem -o signed.apk app.apk
   ```

3. **验证APK签名**
   ```bash
   qsign.exe -t -c public.pem app.apk
   ```



本程序采用MIT许可证，可自由使用、修改和分发。

## 联系方式

如有问题或建议，欢迎issue提问。
