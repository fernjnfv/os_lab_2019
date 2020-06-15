#!/bin/bash

sum=0
count=0
count=$#
for var in "$@"
do
sum=$[$sum + $var]
done
mid=$[$sum / $count]
echo "count = $count"
echo "mid = $mid"