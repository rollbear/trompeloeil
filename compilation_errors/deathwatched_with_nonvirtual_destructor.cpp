/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

// pass: virtual destructor is a necessity for deathwatched to work

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK0(f, int());
};

int main()
{
  auto obj = new trompeloeil::deathwatched<MS>();
}
