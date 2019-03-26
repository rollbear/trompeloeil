/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2019
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */


#ifndef TROMPELOEIL_BOOST_HPP_
#define TROMPELOEIL_BOOST_HPP_

#ifndef BOOST_TEST
#error "<boost/unit_test.hpp> must be included before <boost/trompeloeil.hpp>"
#endif

#include "../trompeloeil.hpp"

namespace trompeloeil
{
  template <>
  inline void reporter<specialized>::send(
    severity s,
    char const *file,
    unsigned long line,
    const char* msg)
  {
    std::ostringstream os;
    if (line != 0U) os << file << ':' << line << '\n';
    auto text = os.str() + msg;
    if (s == severity::fatal)
      BOOST_FAIL(text);
    else
      BOOST_ERROR(text);
  }
}


#endif //TROMPELOEIL_BOOST_HPP_
