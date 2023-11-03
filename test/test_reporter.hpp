#ifndef TROMPELOEIL_TEST_REPORTER_HPP
#define TROMPELOEIL_TEST_REPORTER_HPP

#include <trompeloeil.hpp>

namespace detail
{
/*
 * Use compiler-version independent make_unique.
 */
using ::trompeloeil::detail::make_unique;

// std::uncaught_exception() is deprecated in C++17.
inline
bool
there_are_uncaught_exceptions()
{
    /*
     * GCC 5.x supports specifying -std=c++17 but libstdc++-v3 for
     * GCC 5.x doesn't declare std::uncaught_exceptions().
     * Rather than detect what version of C++ Standard Library
     * is in use, we equate the compiler version with the library version.
     *
     * Some day this test will based on __cpp_lib_uncaught_exceptions.
     */
#   if (TROMPELOEIL_CPLUSPLUS > 201402L) && \
       ((!TROMPELOEIL_GCC) || \
        (TROMPELOEIL_GCC && TROMPELOEIL_GCC_VERSION >= 60000))

    return std::uncaught_exceptions() > 0;

#   else

    return std::uncaught_exception();

#   endif
}

} /* namespace detail */

class reported {};

struct report
{
    trompeloeil::severity s;
    const char           *file;
    unsigned long         line;
    std::string           msg;
};

extern std::vector<report> reports;
extern std::vector<std::string> okReports;

namespace trompeloeil
{
template <>
struct reporter<specialized>
{
    static void send(severity s,
                     char const* file,
                     unsigned long line,
                     char const* msg)
    {
        reports.push_back(report{s, file, line, msg});
        if (s == severity::fatal && !::detail::there_are_uncaught_exceptions())
        {
            throw reported{};
        }
    }

    static void sendOk(char const* msg)
    {
        okReports.push_back(msg);
    }
};
}

struct Fixture
{
    Fixture() {
        reports.clear();
        okReports.clear();
    }
};

#endif //TROMPELOEIL_TEST_REPORTER_HPP
