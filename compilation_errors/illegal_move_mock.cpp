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

// pass: make a mock object movable, see:

#include <trompeloeil.hpp>


struct M
{
  MAKE_MOCK1(f, void(int));
};

template <typename T>
T ident(T t)
{
  return t;
}

int main()
{
  auto obj = ident(M{});
}
