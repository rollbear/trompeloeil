//Multiple RETURN does not make sense

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
  int n;
  REQUIRE_CALL(obj, f())
    .SIDE_EFFECT(n = 1)
    .RETURN(1)
    .RETURN(2);
}
