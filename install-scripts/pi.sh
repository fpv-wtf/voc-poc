#!/bin/bash

targetdir=/home/pi
pidfile=/tmp/voc.pid

case "$1" in

start)

if [ $(ls $pidfile) ] && [ $(ps $(cat $pidfile) | egrep -v "PID") ]
    then
    echo "VOC RUNNING PLEASE STOP PROCESS FIRST $0 stop"
else
    sudo node $targetdir/index.js -o | ffplay -i - -analyzeduration 1 -probesize 32 -sync ext &
    vocpid=$!
    echo "$vocpid" > $pidfile
    echo "VOC Started!"
fi


;;

stop)

if [[ $(ls $pidfile) ]]
    then
    kill $(cat $pidfile)
    rm $pidfile
    echo "VOC Stopped!"
else
    sudo node $targetdir/index.js -o | ffplay -i - -analyzeduration 1 -probesize 32 -sync ext &
    vocpid=$!
    echo "$vocpid" > $pidfile
fi


;;

install)

sudo apt-get install -y ffmpeg curl
curl -fsSL https://deb.nodesource.com/setup_16.x | sudo -E bash -
sudo apt-get install -y nodejs git
git clone https://github.com/fpv-wtf/voc-poc.git
cp -r voc-poc/* $targetdir/ # voc-poc expects /home/pi/index.js
npm install
sudo apt-get install -y libudev-dev

$0 start

;;

*)
if [ $(ls $pidfile)] && [ $(ps $(cat $pidfile) | egrep -v "PID") ]
    then
    echo "VOC RUNNING"
else
    if [[ $(ls $targetdir/index.js) ]]
    then
        echo "VOC installed please run $0 start"
    else
        $0 install
    fi
fi
;;
esac

