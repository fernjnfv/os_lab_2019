#!/bin/bash
list=""
for i in $(seq 1 $1)
do
    rnd=$(dd if=/dev/urandom count=4 bs=1 status=none | od -t d)
    count=0
    for word in $rnd
    do
        if (($count==1))
        then
            needed=$word
            #echo "$needed"
            list=$list$needed" "
            break
        fi
        count=$(($count + 1))
    done
done
echo "$list"