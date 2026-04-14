# chatdemo

跨平台 UDP 聊天室示例，包含：

- `socketx`：Windows / macOS / Linux 的 UDP socket 薄封装
- `threadx`：线程与互斥锁兼容层
- `chat_protocol`：简单文本协议
- `chat_server`：聊天室服务端
- `chat_client`：命令行客户端

## 协议

报文格式：

```text
UDPCHAT|<version>|<type>|<sender>|<payload>
```

类型：

- `1 JOIN`
- `2 LEAVE`
- `3 CHAT`
- `4 SYSTEM`
- `5 PING`
- `6 PONG`
- `7 ERROR`

## 构建

### CMake

```bash
# 生成构建系统
cmake -S . -B build
# 真正执行编译
cmake --build build
```

### Makefile

```bash
# 使用 Makefile 编译服务端与客户端
make

# 清理 Makefile 生成的构建目录与可执行文件
make clean
```

### Windows MinGW

```bash
# 编译服务端可执行文件 chat_server.exe
gcc -std=c23 -Wall -Wextra -pedantic -Iinc src/platform.c src/socketx.c src/threadx.c src/utf8.c src/chat_protocol.c src/chat_server.c src/chat_client.c src/server_main.c -o chat_server.exe -lws2_32 -lshell32

# 编译客户端可执行文件 chat_client.exe
gcc -std=c23 -Wall -Wextra -pedantic -Iinc src/platform.c src/socketx.c src/threadx.c src/utf8.c src/chat_protocol.c src/chat_server.c src/chat_client.c src/client_main.c -o chat_client.exe -lws2_32 -lshell32
```

## 运行

```bash
# 启动服务端，监听 9000 端口，最多允许 128 个客户端在线
./chat_server 0.0.0.0 9000 128

# 启动第一个客户端，用户名使用 Unicode 字符与表情
./chat_client 127.0.0.1 9000 "阿星😀"

# 启动第二个客户端，继续验证 Unicode 用户名与表情
./chat_client 127.0.0.1 9000 "小喵🚀"

# Linux / macOS / WSL 下如果用户名包含空格、括号、星号等 shell 特殊字符，必须加引号
./chat_client 127.0.0.1 9000 "小王(*^_^*)"
```

客户端命令：

- `/ping`
- `/quit`

客户端消息示例：

```text
你好，世界 👋
今晚一起联调吗？😄
收到请回复 ✅
```

## 约束

- 用户名要求为合法 UTF-8，最长 31 个 Unicode 字符
- 单条消息正文要求为合法 UTF-8，最长 512 个 Unicode 字符
- 当前实现只覆盖 IPv4 UDP
- 服务端会拒绝未加入直接发消息、重名用户、非法 UTF-8 或非法报文

