/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2019
 * Copyright Tore Martin Hagen 2019
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */


#ifndef TROMPELOEIL_CATCH2_HPP_
#define TROMPELOEIL_CATCH2_HPP_

#ifndef CATCH_VERSION_MAJOR
#error "<catch.hpp> must be included before <catch2/trompeloeil.hpp>"
#endif

#include "../trompeloeil.hpp"

namespace trompeloeil
{
  struct catch2_error_message {
      std::string s;
      operator bool() const { return false; }
      friend std::ostream& operator<<(std::ostream& os, const catch2_error_message& m)
      {
        return os << m.s;
      }
  };

  template <>
  inline void reporter<specialized>::send(
    severity s,
    const char* file,
    unsigned long line,
    const char* msg)
  {
    std::ostringstream os;
    if (line) os << file << ':' << line << '\n';
    os << msg;

    catch2_error_message m{os.str()};
    if (s == severity::fatal)
    {
#ifdef CATCH_CONFIG_PREFIX_ALL
      CATCH_FAIL(m);
#else
      FAIL(m);
#endif
    }
    else
    {
#ifdef CATCH_CONFIG_PREFIX_ALL
      CATCH_CHECK(m);
#else
      CHECK(m);
#endif
    }
  }

  template <>
  inline void reporter<specialized>::sendOk(
    const char* trompeloeil_mock_calls_done_correctly)
  {
#ifdef CATCH_CONFIG_PREFIX_ALL
      CATCH_REQUIRE(trompeloeil_mock_calls_done_correctly != 0);
#else
      REQUIRE(trompeloeil_mock_calls_done_correctly != 0);
#endif
  }
}


#endif //TROMPELOEIL_CATCH2_HPP_
