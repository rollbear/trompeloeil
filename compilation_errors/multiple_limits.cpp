//Only one TIMES call limit is allowed, but it can express an interval
#include "../trompeloeil.hpp"

struct S
{
  virtual int f() = 0;
};

struct MS : trompeloeil::mocked_class<S>
{
  MOCK(f, ());
};

int main()
{
  trompeloeil::sequence seq;
  MS obj;
  REQUIRE_CALL(obj, f())
    .TIMES(AT_LEAST(1))
    .TIMES(AT_MOST(3))
    .RETURN(0)
}
