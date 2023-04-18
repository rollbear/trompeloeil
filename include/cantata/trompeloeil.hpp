/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2023
 * Copyright Andreas Schätti 2023
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

#ifndef TROMPELOEIL_CANTATA_HPP_
#define TROMPELOEIL_CANTATA_HPP_

#ifndef INCLUDED_CANTPP_H
#error "<cantpp.h> must be included before <trompeloeil.hpp>"
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
    {
      text += "\nseverity: fatal\n";
      CHECK_NAMED(text.c_str(), true, false);
      throw expectation_violation("severity: fatal");
    }
    else
    {
      CHECK_NAMED(text.c_str(), true, false);
    }
  }

  template <>
  inline void reporter<specialized>::sendOk(
    const char* trompeloeil_mock_calls_done_correctly)
  {
    CHECK_NAMED(trompeloeil_mock_calls_done_correctly, true, true);
  }
}

#endif //TROMPELOEIL_CANTATA_HPP_
