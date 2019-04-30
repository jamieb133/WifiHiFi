#!/bin/sh

./bin/slave
#echo exit code: $?
#CLIENT=$?
#echo $?
if [ $? -eq 10 ]; then
    echo Will now power off the pi
    sudo shutdown now
fi
