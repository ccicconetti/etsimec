#!/bin/bash

if [ $# -ne 1 ] ; then
  echo "Syntax: `basename $0` <compiler_path>"
  exit 1
fi

if [ ! -z "$( ls -A | grep -v .gitignore)" ]; then
  echo "Directory not empty, leaving now"
  exit 1
fi

CURRENTDIR=${PWD##*/}

cmake -DCMAKE_CXX_COMPILER=$1 -DCMAKE_BUILD_TYPE=${CURRENTDIR} ../../
