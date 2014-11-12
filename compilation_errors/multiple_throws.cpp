//Multiple THROW does not make sense

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
  MS obj;
  REQUIRE_CALL(obj, f())
    .THROW(3)
    .THROW('a');
}
