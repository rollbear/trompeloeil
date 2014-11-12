//In TIMES the first value must not exceed the second
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
    .RETURN(0)
    .TIMES(3,2);
}
