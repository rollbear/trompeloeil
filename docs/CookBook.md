# *Trompeloeil* cook book

- [Creating Mock Classes](#creating_mock_classes)
  - [Mocking private or protected member functions](#mocking_non_public)
  - [Mocking overloaded member functions](#mocking_overloaded)
  - [Mocking a class template](#mocking_class_template)
  - [Mocking non-virtual member functions](#mocking_non_virtual)
  - [Mocking free functions](#mocking_free_functions)
- [Setting Expectations](#setting_expectations)
  - [Matching exact values](#matching_exact_values)
  - [Matching values with conditions](#matching_conditions)
  - [Matching calls with conditions depending on several parameters](#matching_multiconditions)
  - [Matching `std::unique_ptr<T>` and other non-copyable values](#matching_non_copyable)
  - [Return values from matching calls](#return values)
  - [Return references from matching calls](#return_references)
  - [Define side effects for matching calls](#side_effects)
  - [Matching calls to overloaded member functions](#matching_overloads)
  - [Allowing any call](#allowing_any)
  - [Temporarily disallowing matching calls](#temporary_disallow)
  - [Expecting several matching calls in some sequences](#sequences)
  - [Expecting matching calls a certain number of times](#match_count)
- [Controlling lifetime of mock objects](#lifetime)
- [Tracing mocks](#tracing)
  - [Using `trompeloeil::stream_tracer`](#stream_tracer)
  - [Writing custom tracers](#custom_tracer)
- [Writing custom matchers](#custom_matchers)
- [Integrating with unit test frame works](#unit_test_frameworks)




## <A name="creating_mock_classes"/> Creating Mock Classes

A Mock class is any class that [mocks](reference.md/#mock_function) member
functions.

Member functions are mocked using the macros
[MAKE_MOCKn](reference.md/#MAKE_MOCKn) and
[MAKE_CONST_MOCKn](reference.md/#MAKE_CONST_MOCKn), where `n` is the number
of parameters in the function.

Mock classes are typically used to implement interfaces, so they normally
inherit from a pure virtual class, but this is not necessary.

Example:

```Cpp
class Dictionary
{
public:
  virtual ~Interface() = default;
  virtual std::string& lookup(int n) const;
  virtual void add(int n, std::string&&);
};

class MockDictionary : public Dictionary
{
  MAKE_CONST_MOCK1(lookup, std::string&(int));
  MAKE_MOCK2(add, void(int, std::string&&));
};
```

In the example above, `MockDictionary` is, as the name implies, a mock class for
the pure virtual class `Dictionary`.

The line `MAKE_CONST_MOCK1(lookup, std::string&(int));` implements the
function `std::string& lookup(int) const;`, and the line
`MAKE_MOCK2(add, void(int, std::string&&));` implements the function
`void add(int, std::string&&)`.

### <A name="mocking_non_public"/> Mocking private or protected member functions

Mocking private or protected member functions is no different from mocking
public member functions. Just make them public in the mock class. It may seem
strange that you can change access rights of a member function through
inheritance, but C++ allows it.

Example:

```Cpp
class Base
{
private:
  virtual void secret(int);
};

class Mock : public Base
{
public:
  MAKE_MOCK1(secret, void(int)); // not so secret now
};
```

The [mock functions](reference.md/#mock_function) must be public for you to
be able to sext [expectations](#setting_expectations) on them, but there is
nothig preventing a public function from implementing a private virtual function
in a base class.


### <A name="mocking_overloaded"/> Mocking overloaded member functions

*Trompeloeil* matches [mock functions](reference.md/#mock_function) by
their name and their signature, so there is nothing special about
adding several overloads of mocked functions.

Example:

```Cpp
class Mock
{
public:
  MAKE_MOCK1(overload, void(int));
  MAKE_MOCK1(overload, int(const std::string&));
  MAKE_MOCK2(overload, int(const char*, size_t));
};
```

Above there are three [mock functions](reference.md/#mock_function) named
`overload`, with different signatures.

See [Matching calls to overloaded member functions](#matching_overloads) for
how to place [expectations](reference.md/#expectation) on them.


### <A name="mocking_class_template"/> Mocking a class template

Unlike some *C++* mocking frame works, *Trompeloeil* does not make a
distinction between mocks in class templates and mocks in concrete classes.

Example:

```Cpp
template <typename T>
class Mock
{
public:
  MAKE_MOCK1(func, void(int));
  MAKE_MOCK2(tfunc, int(const T&, size_t));
};
```

Above, `Mock<T>` is a mock class template with two member functions. The
member function `void func(int)` does not depend on the template parameter,
whereas the member function `int tfunc(const T&, size_t)` does. This will
work for any type `T`.

### <A name="mocking_non_virtual"/> Mocking non-virtual member functions

While it is often the case that mocks are used to implement interfaces,
there is no such requirement. Just add the
[mock functions](reference.md/#mock_function) that are needed.

Example:

```Cpp
class ConcreteMock
{
public:
  MAKE_MOCK2(func, bool(size_t, const char*));
};
```

Above `ConcreteMock` is a mock class that implements a non-virtual
[mock function](reference.md/#mock_function) `bool func(size_t, const char*)`.

### <A name="mocking_free_functions"/> Mocking free functions

Free functions on their own cannot be mocked, the calls to them needs to
be dispatched to [mock objects](reference.md/#mock_object). Often there are
several free functions that together form an API, and then it makes sense
to implement one mock class for the API, with
[mock functions](reference.md/#mock_function) for each.

Example, assume a simple C-API

```Cpp
// C-API.h

#ifndef __cplusplus
extern "C" {
#endif

struct c_api_cookie;

struct c_api_cookie* c_api_init();

int c_api_func1(c_api_cookie* cookie, const char* str, size_t len);
 
void c_api_end(struct c_api_cookie*);

#ifndef __cplusplus
}
#endif
```

```Cpp
// unit-test-C-API.h

#include "C-API.h"

class API
{
public:
  MAKE_MOCK0(c_api_init, c_api_cookie*());
  MAKE_MOCK3(c_api_func1, int(c_api_cookie*, const char*, size_t));
  MAKE_MOCK1(c_api_end, void(c_api_cookie*));
};

API c_api_mock;
```

Then implement the functions in a test version of the API, which uses the
mock.

```Cpp
// unit-test_c_api.cpp
#include "unit-test-C-API.h"

extern "C" {
  c_api_cookie c_api_init()
  {
    return api_mock.c_api_init();
  }
  
  int c_api_func1(c_api_cookie* cookie, const char* str, size_t len)
  {
    return api_mock.c_api_func1(cookie, str, len);
  }
  
  void c_api_end(c_api_cookie* cookie)
  {
    api_mock.c_api_end(cookie);
  }
}
```

A test program can place [expectations](reference.md/#expectation) ond the
mock object, and the tested functionality calls the C-api functions which
dispatch to the mock object.

## <A name="setting_expectations"/> Setting Expectations

It is with [expectations](reference.md/#expectation) you define the behaviour
of your test. By default all calls to
[mock functions](reference.md/#mock_function) are illegal and will be reported
as violations. You use expectations, long or short lived, wide or narrow,
to make some calls legal and define what happens.

There are three basic types of expectations.

 - [**`ALLOW_CALL(...)`**](reference.md/#ALLOW_CALL)
 - [**`REQUIRE_CALL(...)`**](reference.md/#REQUIRE_CALL)
 - [**`FORBID_CALL(...)`**](reference.md/#FORBID_CALL)
 
**`ALLOW_CALL(...)`** is often used for a default. It can match any number of
times.

**`REQUIRE_CALL(...)`** is stricter and defaults to match exactly once, although
you can change that and control exactly [how many times](#match_count) you want
the expectation to match.

**`FORBID_CALL(...)`** may seem unnecessary since calls are forbidden by
default, but it is useful in combination with **`ALLOW_CALL(...)`** or
**`FORBID_CALL(...)`** to forbid something that they would otherwise accept.

If several expectations match a call, it is the last matching expectation
created that is used. **`ALLOW_CALL(...)`**, **`REQUIRE_CALL(...)`** and
**`FORBID_CALL(...)`** are active until the end of the scope. This means
that you can place a wide default, and use temporary special expectations in
local scopes, for example to temporarily forbid a call that is otherwise
allowed.

If the scoped lifetime rules are unsuitable, there are also thee named
versions of the expectations.

  -[**`NAMED_ALLOW_CALL(...)`**](reference.md/#NAMED_ALLOW_CALL)
  -[**`NAMED_REQUIRE_CALL(...)`**](reference.md/#NAMED_REQUIRE_CALL)
  -[**`NAMED_FORBID_CALL(...)`**](reference.md/#NAMED_FORBID_CALL)
  
These do the same, but they are `std::unique_ptr<trompeloeil::expectation>`,
which you can bind to variables that you control the life time of.


### <A name="matching_exact_values"/> Matching exact values

The simplest [expectations](reference.md/#expectation) are for calls with exact
expected values. You just provide the expected values in the parameter list
of the expectation.

Example:

```Cpp
class Mock
{
public:
  MAKE_MOCK1(func, void(int));
  MACEK_MOCK2(func, void(const char*);
};

void test()
{
  Mock m;
  ALLOW_CALL(m, func(1);          // int version any number of times
  REQUIRE_CALL(m, func(nullptr)); // const char * version exactly once
}
```

### <A name="matching_conditions"/> Matching values with conditions

### <A name="matching_multiconditions"/> Matching calls with conditions depending on several parameters

### <A name="matching_non_copyable"/> Matching `std::unique_ptr<T>` and other non-copyable values

### <A name="return_value"/> Return values from matching calls

### <A name="return_references"/> Return references from matching calls

### <A name="side_effects"/> Define side effects for matching calls

### <A name="matching_overloads"/> Matching calls to overloaded member functions

### <A name="allowing_any"/> Allowing any call

### <A name="temporary_disallow"/> Temporarily disallowing matching calls

### <A name="sequences"/> Expecting several matching calls in some sequences

### <A name="match_count"/> Expecting matching calls a certain number of times

## <A name="lifetime"/> Controlling lifetime of mock objects

## <A name="tracing"/> Tracing mocks
 
### <A name="stream_tracer"/> Using `trompeloeil::stream_tracer`

### <A name="custom_tracer"/> Writing custom tracers

## <A name="custom_matchers"/> Writing custom matchers

## <A name="unit_test_frameworks"/> Integrating with unit test frame works
