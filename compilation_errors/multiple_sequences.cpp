//Multiple IN_SEQUENCE does not make sense. You can list several sequence
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
    .IN_SEQUENCE(seq)
    .RETURN(0)
    .IN_SEQUENCE(seq);
}
