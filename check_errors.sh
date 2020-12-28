#!/bin/sh

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
PASS="\033[32mPASS\033[0m"
FAIL="\033[1;31mFAIL\033[0m"
FAILURES=0

echo "CXX=$CXX"
echo "CXXFLAGS=$CXXFLAGS"
echo "CPPFLAGS=$CPPFLAGS"

# Default CXXFLAGS to -std=c++14 if not set in the environment
# for backward compatibility.
#CXXFLAGS=${CXXFLAGS:-"-std=c++14"}

#echo "CXXFLAGS is now $CXXFLAGS"

#${CXX} --version
cd compilation_errors
SCRIPT='
s:^//\(.*\)$:\1:g
t print
b
:print
P
'
for f in *.cpp
do
  RE=`sed -n "$SCRIPT" < $f`
  printf "%-45s" $f
  # echo "RE=$RE"
  ${CXX} ${CXXFLAGS} ${CPPFLAGS} -I ../include $f -c 2>&1 | egrep  -q "${RE}" && echo ${PASS} && continue || echo ${FAIL} && false
  FAILURES=$((FAILURES+$?))
done
# echo "FAILURES=$FAILURES"
exit $FAILURES
