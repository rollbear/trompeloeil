#!/bin/bash

#
# Trompeloeil C++ mocking framework
#
# Copyright Björn Fahller 2014
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/rollbear/trompeloeil
#


for f in compilation_errors/*.cpp
do
  RE=$(sed -n 's:^//\(.*\)$:\1:g;T;P' < $f)
  echo -n "$f "
  ${CXX} -std=c++14 $f -c |& egrep -q "$RE" && echo OK || echo FAIL
done