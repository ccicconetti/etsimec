#!/bin/bash

CPPCHECK=/usr/local/bin/cppcheck
OPTIONS="--library=gnu --library=posix \
         --platform=unix64 --std=c++11 \
         --suppressions-list=cppcheck.suppression \
         --enable=all \
         --xml \
         --xml-version=2 \
         -I../emulator \
         -I../build/debug/emulator/proto-src \
         --suppress=syntaxError \
         -j 8 \
         --force"
         
#-I../build/debug/googletest-src/googletest/include \

${CPPCHECK} ${OPTIONS} \
  ../EtsiMec \
  ../Executables \
  ../Test \
  2>report.xml
