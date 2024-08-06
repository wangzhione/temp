# SHELL 烂笔头

**批量 remove**

```Shell
for pkg in docker.io docker-doc docker-compose docker-compose-v2 podman-docker containerd runc; do sudo apt-get remove $pkg; done
```

****

```Shell
#!/bin/bash

Network=$1
for Host in $(seq 1 254)
do
    ping -c 1 $Network.$Host > /dev/null && result=0 || result=1

    if [ $result == 0 ]
    then
        echo -e "\033[32;1m$Network.$Host is up \033[0m"
    else
        echo -e "\033[;31m$Network.$Host is down \033[0m"
    fi
done
```

***

```Shell
grep -r 'aaaa' / 2> /dev/null | head -5
```

在类 Unix 系统中，有两个输出路径，如果不进行修改，则会将输出发送到屏幕。标准错误（或 stderr）是捕获大多数故障和错误情况的路径。

要将 stderr 中的权限被拒绝消息传递到与“常规输出”相同的输出流，您必须将两者结合起来。在您的示例中，为了grep -v正确操作它，您将 stdout（标准输出）和 stderr 与您所看到的神秘语法结合起来。

来自 GNU Bash 手册第 3.2.2 节管道：

> 如果 |& 使用 ' '，则 command1 的标准错误除了其标准输出外，还通过管道连接到 command2 2>&1 | 的标准输入；它是 的简写。在命令指定的任何重定向之后，将标准错误隐式重定向到标准输出。

***

统计 nginx 日志里访问次数最多的前十个 IP

```SHELL
awk '{print $1}' /var/log/nginx/access.log | sort | uniq -c | sort -nr -k1 | head -n 10
```

***

查找当前目录一个月(30 天)以前大于 100M 的日志文件(.log)并删除

```SHELL
find  . -name "*.log" –m time +30 –type f –size +100M | xargs rm –rf {} ;
```

***

