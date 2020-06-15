#!/bin/bash
summ=0
num_args=0
for var in "$@"
do
num_args=$(($num_args + 1))
summ=$(($summ + $var))
done
avg_summ=$(($summ / $num_args))
echo "avg_summ = $avg_summ"