/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

//RETURN value is not convertible to the return type of the function

#include "../trompeloeil.hpp"

struct MS
{
  MAKE_MOCK0(f, int());
};

int main()
{
  int n;
  MS obj;
  REQUIRE_CALL(obj, f())
    .RETURN("")
    .SIDE_EFFECT(n = 0);
}
