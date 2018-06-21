#!/bin/bash

SOURCES_NAMES="source/dialg_common.cpp source/qgroup.cpp"

g++ -Wall -o main main.cpp $SOURCES_NAMES -Lsource -lpcre -std=c++11
