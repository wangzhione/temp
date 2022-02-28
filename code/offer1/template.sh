#!/bin/bash

set -ue

if [[ $# != 1 ]]
then
    echo "usage: $0 {name}"
    exit -1
fi

name=$1
file=${name}.c

if [[ -a ${file} ]]
then
    echo "${file} exists"
    exit 0
fi

readonly template_file=template.study.c

sed "s/{name}/${name}/g" ${template_file} > ${file}
