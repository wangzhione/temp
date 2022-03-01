#!/bin/bash

set -ue

if [[ $# != 1 ]]
then
    echo "usage: $0 {name}"
    exit -1
fi

name=$1
file=${name}.c

#
# 1. 用户输入文件名, 我们尝试创建文件
#
# ${file##.} -> 后缀名 suffix
# ${file%%.} -> 文件名称
#

if [[ -a ${file} ]]
then
    echo "${file} exists"
    exit 0
fi

readonly template_file=template.c

sed "s/{name}/${name}/g" ${template_file} > ${file}
