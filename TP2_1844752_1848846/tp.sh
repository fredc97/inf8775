#!/bin/bash

ALGO="glouton"
OPTIONS=""
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -a)
    ALGO="$2"
    shift
    ;;
    -e)
    EX_PATH="$2"
    shift
    ;;
    -p|-t|-c)
    OPTIONS="${OPTIONS}${1} "
    ;;
    *)
        echo "Argument inconnu: ${1}"
        exit
    ;;
esac
shift
done


./main -e $EX_PATH -a $ALGO $OPTIONS
