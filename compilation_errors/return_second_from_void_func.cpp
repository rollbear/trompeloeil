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

//RETURN does not make sense for void-function

#include "../trompeloeil.hpp"

struct S
{
  virtual void f() = 0;
};

struct MS : trompeloeil::mocked_class<S>
{
  MAKE_MOCK0(f, void());
};

int main()
{
  int n;
  MS obj;
  REQUIRE_CALL(obj, f())
    .SIDE_EFFECT(n = 0)
    .RETURN(1);
}
