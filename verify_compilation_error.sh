#!/bin/sh

#
# Trompeloeil C++ mocking framework
#
# Copyright Björn Fahller 2022
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

SCRIPT='
s:^//\(.*\)$:\1:g
t print
b
:print
P
'
f=$1
RE=`sed -n "$SCRIPT" < $f`
# echo "RE=$RE"
${CXX} ${CXXFLAGS} ${CPPFLAGS} -I ../include $f -c 2>&1 | egrep  -q "${RE}" && printf "%-45s ${PASS}\n" $f && continue || printf "%-45s ${FAIL}\n" $f && false
