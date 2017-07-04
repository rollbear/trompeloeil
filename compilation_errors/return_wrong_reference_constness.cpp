/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014,2015,2017
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

//RETURN const\& from function returning non-const reference

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK0(f, char&());
};

int main()
{
  MS obj;

#if __cplusplus == 201103L

  using m_t = NAMED_MOCK_TYPE(obj, f());
  REQUIRE_CALL(obj, f())
    .RETURN_TYPE(m_t, *"");

#else /* __cplusplus == 201103L */

  REQUIRE_CALL(obj, f())
    .RETURN(*"");

#endif /* !__cplusplus == 201103L */
}
