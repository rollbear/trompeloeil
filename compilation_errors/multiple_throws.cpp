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

//Multiple THROW does not make sense

#include "../trompeloeil.hpp"

struct MS
{
  MAKE_MOCK0(f, int());
};

int main()
{
  MS obj;
  REQUIRE_CALL(obj, f())
    .THROW(3)
    .THROW('a');
}
