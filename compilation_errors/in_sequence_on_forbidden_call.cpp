/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2015,2017
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

//IN_SEQUENCE for forbidden call does not make sense

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK0(f, void());
};

int main()
{
  MS obj;
  trompeloeil::sequence s;
  REQUIRE_CALL(obj, f())
    .TIMES(0)
      .IN_SEQUENCE(s);
}
