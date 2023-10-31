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

get_rule ()
{
SCRIPT='
s|^// '$1': \(.*\)$|\1|g
t print
b
:print
P
'
sed -n "$SCRIPT" < $2
}
PASS="\033[32mPASS\033[0m"
FAIL="\033[1;31mFAIL\033[0m"

f=$1
EXCEPTION_RE=`get_rule "exception" $f`
COMPILER="${CXX} ${CXXFLAGS} ${CPPFLAGS}"

[ -n "$EXCEPTION_RE" ] && echo ${COMPILER} | grep -q -e "$EXCEPTION_RE" && exit 0
PASS_RE=`get_rule "pass" $f`
failfile=`mktemp`
(${COMPILER} -I ../include $f -c 2>&1) > $failfile
cat $failfile | egrep  -q "${PASS_RE}" && printf "%-45s ${PASS}\n" $f && continue || { printf "%-45s ${FAIL}\n" $f; cat $failfile; } && false
RET=$?
rm $failfile
exit $RET
