//RETURN value is not convertible to the return type of the function

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
  int n;
  MS obj;
  REQUIRE_CALL(obj, f())
    .SIDE_EFFECT(n = 0)
    .RETURN("");
}
