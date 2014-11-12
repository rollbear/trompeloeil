//RETURN does not make sense for void-function

#include "../trompeloeil.hpp"

struct S
{
  virtual void f() = 0;
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
    .RETURN(1)
    .SIDE_EFFECT(n = 0);
}
