# git 烂笔头

    触类旁通, 举一反三, 不求甚解, 欢迎补充

## 详细介绍

git connect github

```Shell
# 1. 本地配置, 姓名和邮箱
git config --global user.name "<name>"
git config --global user.email "<email>"

# 2. 生成 ssh 公钥
ssh-keygen -t rsa -C "<email>"
# 2.2 Eenter 回车三次
# 2.3 根据提示复制 id_rsa.pub 内容
# 2.4 github.com 打开 [Account settings]--[SSH Keys]页面，然后点[Add SSH Key] 

# 3. 验证, 成功会提示 successfully
ssh -T git@github.com
```

git diff

    tig or ide or ...

git 强制远端覆盖本地

```Shell
git fetch --all
git reset --hard origin/master
```

    git fetch --all : 拉取所有更新, 不同步

    git reset --hard origin/master : 本地代码同步线上最新版本(会覆盖本地所有与远程仓库上同名的文件)

git tag 管理

```Shell
# 打了一个 tag v0.0.1
git tag -a v0.0.1 -m "v0.0.1 版本描述信息"

# 本地 tag 推送到远端
git push origin --tags

# 删除本地 tag v0.0.1
git tag -d v0.0.1

# 删除远端 tag v0.0.1
git push origin :refs/tags/v0.0.1
```

git rebase 一种协作的流程

git config --global alias.ll "log --graph --all --pretty=format:'%Cred%h %Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit --date=relative"

    00) git checkout <分支>
    01) coding # 理解需求并快速开发
    02) git add <src path>

    03) git status # [可选] 详细查看你提交文件是否是你所需要的

    04) git commit -m "xxxxx"
    05) git fetch
    06) git ll # 确认本地 HEAD 是否基于 origin/<分支>, 若是则跳至 11)
    07) git rebase origin/<分支> 若成功则跳至 10)
    08) coding update # 解决 both modified 的所有文件冲突
    09) git add # 所有 modified 的代码(包括 modified 和 both modified)
    10) git rebase --continue
    11) git ll # 确认 HEAD 已经基于(rebase 到) origin/<分支>

    12) git diff HEAD <src> # [可选] 详细对比你提交的文件内容, 保证合并过程中的减少意外
    13) git push origin HEAD:<分支>

git 分支清理

```Shell
# 清理本地冗余分支
git branch | grep -v -E 'master$|preview$|staging$' | xargs git branch -D
# 清理远端冗余分支
# git branch -r | grep -v -E 'master$|preview$|staging$' | sed 's/origin\///g' | xargs -I {} echo 'git push origin :{}' | more
git branch -r | grep -v -E 'master$|preview$|staging$' | sed 's/origin\///g' | xargs -I {} git push origin :{}
# 本地分支类比远端分支重新整理
git remote prune origin
```

    git branch -D [branch name] : 删除本地分支

    git push origin :[branch name] : 删除远端分支

git 查水表

[git 版本控制手册](https://git-scm.com/book/zh/v2/%E8%B5%B7%E6%AD%A5-%E5%85%B3%E4%BA%8E%E7%89%88%E6%9C%AC%E6%8E%A7%E5%88%B6)

