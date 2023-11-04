#ifndef TROMPELOEIL_STREAM_TRACER_HPP
#define TROMPELOEIL_STREAM_TRACER_HPP

#include "mock.hpp"

namespace trompeloeil {

class stream_tracer : public tracer
{
public:
  explicit
  stream_tracer(
    std::ostream& stream_)
    : stream(stream_) {}
  void
  trace(
    char const *file,
    unsigned long line,
    std::string const &call)
  override
  {
    stream << location{file, line} << '\n' << call << '\n';
  }
private:
  std::ostream& stream;
};

}
#endif //TROMPELOEIL_STREAM_TRACER_HPP
