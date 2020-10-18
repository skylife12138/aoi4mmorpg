#!/bin/bash

#TYPE=$1 
#COUNT=$2

cd /data/home/user00/aoi
#python main.py $TYPE $COUNT

pids=$(ps -ef | grep 'Aoi'| awk '{print $2}')
echo $pids
count=0
cpus=0
mems=0
for pid in $pids
do
    echo $pid
    pidstat -u -r -p $pid 2 #| grep 'python'
    #count=count+1
    #cpus=$(pidstat -u -p $pid 2 | grep 'python' |  awk '{print $4}')
   # mems=$(pidstat -r -p $pid 2 | grep 'python' |  awk '{print $7}')
    #cpu=`expr $cpu+$(pidstat -u -p $pid 2 | grep 'python' |  awk '{print $4}')`
    #mem=`expr $mem+$(pidstat -r -p $pid 2 | grep 'python' |  awk '{print $7}')`
    # echo $cpu $mem
   # break
done

cpu_ave=0
mem_ave=0

#for cpu in $cpus
#do
#    cpu_ave=`expr $cpu_ave + $cpu`    
#done

#for mem in $mems
#do
#    mem_ave=`expr $mem_ave + $mem`   
#done

#cpu_ave=`expr $cpu_ave / $count`
#mem_ave=`expr $mem_ave / $count`
#echo $cpu_ave $mem_ave
