#!/bin/bash

#
# Trompeloeil C++ mocking framework
#
# Copyright Björn Fahller 2014-2017
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/rollbear/trompeloeil
#

PASS=$'\E[32mPASS\E[0m'
FAIL=$'\E[1;31mFAIL\E[0m'
FAILURES=0

#echo "CXX=$CXX"
#echo "CXXFLAGS=$CXXFLAGS"
#echo "CPPFLAGS=$CPPFLAGS"

#${CXX} --version

cd compilation_errors

for f in *.cpp
do
  RE=$(sed -n 's:^//\(.*\)$:\1:g;T;P' < $f)
  printf "%-45s" $f
  ${CXX} ${CXXFLAGS} ${CPPFLAGS} -I ../include -std=c++14 $f -c |& egrep -q "$RE" && echo $PASS && continue || echo $FAIL && false
  FAILURES=$((FAILURES+$?))
done
exit $FAILURES
