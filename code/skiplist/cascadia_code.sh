#!/bin/bash

set -o errexit
set -o nounset

#
# download_uri 设置最新下载链接 https://github.com/microsoft/cascadia-code/releases
# fonts_dir 设置系统字体存放指定目录
# 
# 安装过程:
# sudo bash cascadia_code.sh
#


readonly download_uri='https://github.com/microsoft/cascadia-code/releases/download/v2106.17/CascadiaCode-2106.17.zip'

readonly fonts_dir=/usr/share/fonts/CascadiaCode1

readonly file_name=$(echo ${download_uri} | awk -F '/' '{print $NF}')

# 尝试下载
if [[ ! -f "${file_name}" ]]
then
    wget ${download_uri}
fi

# 尝试安装
readonly temp_dir=.CascadiaCode
if [[ ! -d "${fonts_dir}" ]]
then
    unzip -o -d ${temp_dir} ${file_name}

    mkdir ${fonts_dir}
    mv ${temp_dir}/ttf/*.ttf ${fonts_dir}

    # 重建字体缓存, 强制刷新
    mkfontscale
    mkfontdir
    fc-cache -fv
fi

# 善后清理工作
rm -rf ${file_name}
rm -rf ${temp_dir}
