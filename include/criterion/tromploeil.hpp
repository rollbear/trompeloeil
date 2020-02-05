/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2019
 * Copyright Etienne Barbier 2020
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */


#ifndef TROMPELOEIL_CRITERION_HPP_
#define TROMPELOEIL_CRITERION_HPP_

#ifndef CRITERION_H_
#error "<criterion.h> must be included before <criterion/trompeloeil.hpp>"
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
    struct criterion_assert_stats cr_stat__;
    cr_stat__.passed = false;
    cr_stat__.file = file;
    cr_stat__.line = line;
    cr_stat__.message = msg;
    if (s == severity::fatal)
    {
        criterion_send_assert(&cr_stat__);
        CR_FAIL_ABORT_();
    }
    else
    {
        criterion_send_assert(&cr_stat__);
        CR_FAIL_CONTINUES_();
    }
  }

  template <>
  inline void reporter<specialized>::sendOk(
    const char* trompeloeil_mock_calls_done_correctly)
  {
      cri_asserts_passed_incr();
  }
}



#endif //TROMPELOEIL_CRITERION_HPP_
