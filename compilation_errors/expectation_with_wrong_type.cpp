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

//argument.*2

#include "../trompeloeil.hpp"

struct MS
{
  MAKE_MOCK3(f, int(int,int,int));
};
using trompeloeil::_;

int main()
{
  MS obj;
  REQUIRE_CALL(obj, f(1,nullptr,_))
    .RETURN(_2);
}
