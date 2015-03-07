#!/bin/bash
codefile=$1

if [ "$1" == "" ]
then
  echo "File not specified"
fi

if [ ! -f $1 ] 
then
  echo "file $1 does not exist"
  exit
fi
