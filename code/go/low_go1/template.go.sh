#!/bin/bash

set -ue

if [[ $# != 1 ]]
then
    echo "usage: $0 {name.go}"
    exit -1
fi

file=$1

if [[ -a ${file} ]]
then
    code ${file}
    echo "${file} exists"
    exit 0
fi

readonly template_file=template.go

sed "s/{name}/${file}/g" ${template_file} > ${file}

code ${file}
