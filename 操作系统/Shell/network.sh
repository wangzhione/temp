#!/bin/bash

Network=$1
for Host in $(seq 1 254)
do
    # 执行成功后 执行 result=0
    # ping -c 1 $Network.$Host > /dev/null && result=0 || result=1

    # 执行成功后 $? == 0
    ping -c 1 $Network.$Host > /dev/null

    if [ $? == 0 ]
    then
        echo -e "\033[32;1m$Network.$Host is up \033[0m"
    else
        echo -e "\033[;31m$Network.$Host is down \033[0m"
    fi
done
