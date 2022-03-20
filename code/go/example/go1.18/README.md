# Go 

## 代理

由于国内的网络环境, 可以通过配置 GOPROXY 避免 DNS 污染导致的模块拉取缓慢或失败的问题, 加速构建.

```Bash
# GOPROXY 中国
go env -w GOPROXY=https://goproxy.cn,direct
```

