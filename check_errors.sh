#!/bin/bash

for f in compilation_errors/*.cpp
do
  RE=$(sed -n 's:^//\(.*\)$:\1:g;T;P' < $f)
  echo -n "$f "
  ${CXX} -std=c++14 $f -c |& egrep -q "$RE" && echo OK || echo FAIL
done