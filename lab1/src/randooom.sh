#!/bin/bash
list=""
for ((l=0; l < 150; l++))
do
    rnd=$(od -A n -t d -N 1 /dev/random)
    list=$list$rnd
done
echo "$list"