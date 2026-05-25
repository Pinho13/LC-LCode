#!/bin/sh

usage() {
  echo "Usage"
}

choose_mode() {
  if [ $1 = "proj" ]; then
    echo "Run Project"
  elif [ $1 = "fw" ]; then
    echo "Run fw"
  elif [ $1 = "clean" ]; then
    echo "Run clean"
  else 
    usage
  fi
}


if [ "$#" -eq 1 ]; then
    choose_mode $1
else
    usage
    exit 1
fi