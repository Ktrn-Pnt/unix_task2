#!/bin/bash

make 1>>/dev/null

rm test.lck 2>>/dev/null
rm result.txt 2>>/dev/null

for _ in {1..10}
do
    (./myprogram test >> result.txt) &
done 

sleep 5m

killall -SIGINT myprogram
sleep 1
rm test.lck 2>>/dev/null
python3 stat_analyzer.py result.txt 10