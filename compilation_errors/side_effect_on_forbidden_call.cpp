/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2015
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

//SIDE_EFFECT for forbidden call does not make sense

#include "../trompeloeil.hpp"

struct MS
{
  MAKE_MOCK0(f, int());
};

int main()
{
  MS obj;
  FORBID_CALL(obj, f())
    .SIDE_EFFECT(std::cout << 3);
}
