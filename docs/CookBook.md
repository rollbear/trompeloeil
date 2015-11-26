# *Trompeloeil* cook book

- [Integrating with unit test frame works](#unit_test_frameworks)
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
  - [Return values from matching calls](#return_values)
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


## <A name="unit_test_frameworks"/> Integrating with unit test frame works

**A.** By default, *Trompeloeil* reports violations by throwing an exception,
explaining the problem in the
[`what()`](http://en.cppreference.com/w/cpp/error/exception/what) string.

Depending on your test frame work and your runtime environment, this may,
or may not, suffice.

*Trompeloeil* offers support for adaptation to any test frame work. Some
sample adaptations are:

- [Catch!](#adapt_catch)
- [crpcut](#adapt_crpcut)
- [gtest](#adapt_gtest)
- [boost Unit Test Framework](#adapt_boost_unit_test_framework)

What you need to know to adapt to other frame works is this function:

```Cpp
trompeloeil::set_reporter(std::function<void(trompeloeil::severity,
                                             char const *file,
                                             unsigned long line,
                                             const std::string& msg)>)
```

Call it with the adapter to your test frame work. The important thing to
understand is the first parameter `trompeloeil::severity`. It is an enum
with the values `trompeloeil::severity::fatal` and
`trompeloeil::severity::nonfatal`. The value `severity::nonfatal` is
used when reporting violations during stack rollback, typically during
the destruction of an [expectation](reference.md/#expectation). In this
case it is vital that no exception is thrown, or the process will
terminate.

**NOTE!** There are some violation that cannot be attributed to a source code
location. An example is an unexpected call to a mock function for which there
are no expectations. In these cases `file` will be `""` string and
`line` == 0.

### <A name="adapt_catch"/>Use *Trompeloeil* with [Catch!](https://github.com/philsquared/Catch)

```Cpp
  trompeloeil::set_reporter([](::trompeloeil::severity s,
                               char const *file,
                               unsigned long line,
                               const std::string& msg)
    {
      std::ostringstream os;
      if (line) os << file << ':' << line << '\n';
      os << msg;
      if (s == ::trompeloeil::severity::fatal)
        {
          FAIL(os.str());
        }
      CHECK(os.str() == "");
    });
```

### <A name="adapt_crpcut"/>Use *Trompeloeil* with [crpcut](http://crpcut.sourceforge.net)

```Cpp
  trompeloeil::set_reporter([](::trompeloeil::severity,
                               char const *file,
                               unsigned long line,
                               const std::string& msg)
    {
      std::ostringstream os;
      os << file << ':' << line;
      auto loc = os.str();
      auto location = line == 0U
        ? ::crpcut::crpcut_test_monitor::current_test()->get_location()
        : ::crpcut::datatypes::fixed_string::make(loc.c_str(), loc.length());
      ::crpcut::comm::report(::crpcut::comm::exit_fail,
                             std::ostringstream(msg),
                             location);
    });
```

### <A name="adapt_gtest"/>Use *Trompeloeil* with [gtest](https://code.google.com/p/googletest/)

```Cpp
  trompeloeil::set_reporter([](trompeloeil::severity s,
                               char const *file,
                               unsigned long line,
                               const std::string& msg)
    {
      if (s == trompeloeil::severity::fatal)
      {
        std::ostringstream os;
        if (line != 0U)
        {
          os << file << ':' << line << '\n';
        }
        throw trompeloeil::expectation_violation(os.str() + msg);
      }

      ADD_FAILURE_AT(file, line) << msg;
    });
```

### <A name="adapt_boost_unit_test_framework"/>Use *Trompeloeil* with [boost Unit Test Framework](http://www.boost.org/doc/libs/1_59_0/libs/test/doc/html/index.html)

```Cpp
  using trompeloeil::severity;
  trompeloeil::set_reporter([](severity s,
                               char const *file,
                               unsigned long line,
                               const std::string& msg)
    {
      std::ostringstream os;
      if (line != 0U) os << file << ':' << line << '\n';
      auto text = os.str() + msg;
      if (s == severity::fatal)
        BOOST_FAIL(text);
      else
        BOOST_ERROR(text);
    });
```


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
  MAKE_MOCK2(func, void(const char*));
};

void test()
{
  Mock m;
  ALLOW_CALL(m, func(1);          // int version any number of times
  REQUIRE_CALL(m, func(nullptr)); // const char * version exactly once
  func(&m);
  // expectations must be met before end of scope
}
```

### <A name="matching_conditions"/> Matching values with conditions

Instead of using exact values of parameters to match calls with, *Trompeloeil*
provides a set of [matchers](reference.md/#matcher). Simple value matchers are:

- [**`ne(`** *value* **`)`**](reference.md/#ne) matches value not equal
- [**`gt(`** *value* **`)`**](reference.md/#gt) matches value greater than
- [**`ge(`** *value* **`)`**](reference.md/#ge) matches value greater than or eqeal
- [**`lt(`** *value* **`)`**](reference.md/#lt) matches value less than
- [**`le(`** *value* **`)`**](reference.md/#le) matches value less than or equal

Example:

```Cpp
class Mock
{
public:
  MAKE_MOCK1(func, void(int));
  MAKE_MOCK2(func, void(const char*));
};

void test()
{
  Mock m;
  ALLOW_CALL(m, func(trompeloeil::gt(1))); // int version any number of times
  REQUIRE_CALL(m, func(trompeloeil::ne(nullptr))); // const char * version once
  func(&m);
  // expectations must be met before end of scope
}
```

### <A name="matching_multiconditions"/> Matching calls with conditions depending on several parameters

Some times a matching call cannot be judged for individual parameter values
alone, but together they work. Assume for example a c-string API where you have
a `const char*` and a length.

Example:

```Cpp
class Mock
{
public:
  MAKE_MOCK2(func, void(const char*, size_t len));
};

using trompeloeil::ne;
using trompeloeil::_;

void test()
{
  Mock m;
  REQUIRE_CALL(m, func(ne(nullptr), _)))  // once
    .WITH(std::string(_1, _2) == "meow");
  func(&m);
  // expectations must be met before end of scope
}
```

[**`_`**](reference.md/#wildcard) is a special matcher that matches everything.
[**`.WITH(...)`**](reference.md/#WITH) is a construction used for when simple
matchers aren't enough. If a call is made which matches the values given in
the [**`REQUIRE_CALL(...)`**](reference.md/#REQUIRE_CALL), the selection process
continues in [**`.WITH(std::string(_1, _2) == "meow")`**](reference.md/#WITH).

**`_1`** and **`_2`** are the parameters to the call, so in this case a
`std::string` is constructed using the non-nul `const char*` and the length,
and its value is compared agains `"meow"`.

The expression in [**`.WITH(...)`**](reference.md/#WITH) can be anything at all
that returns a boolean value. It can refer to global variables, for example.

It is important to understand that [**`.WITH(...)`**](reference.md/#WITH)
accesses any local variable used in the expression as a copy. If you want to
refer to a local variable by reference, use
[**`.LR_WITH(...)`**](reference.md/#LR_WITH) instead (`LR_` for
"local reference").


### <A name="matching_non_copyable"/> Matching `std::unique_ptr<T>` and other non-copyable values

Matching parameter values that you cannot copy, or do not want to copy,
requires a bit of thought.

The wildcards [**`_`**](reference.md/#wildcard) and
[**`ANY(...)`**](reference.md(#ANY) works. For `std::unique_ptr<T>` and
`std::shared_ptr<T>`, the matcher [**`ne(nullptr)`**](reference.md/#ne) also
works.

If you want to be more specific, you will need to use
[**`.WITH(...)`**](reference.md/#WITH) or
[**`.LR_WITH(...)`**](reference.md/#LR_WITH)

Example:

```Cpp
class Mock
{
public:
  MAKE_MOCK1(func, void(std::unique_ptr<int>));
};

using trompeloeil::ne;

void test()
{
  Mock m;
  REQUIRE_CALL(m, func(ne(nullptr)))
    .WITH(*_1 == 3);
  func(&m);
  // expectations must be met before end of scope
}
```

Above there is a requirement that the function is called with a non-null
`std::unique_ptr<int>`, which points to a value of `3`.

### <A name="return_values"/> Return values from matching calls

An [expectation](reference.md/#expectation) on a non-void function
must return something. There are no default values. Returning is easy,
however. Just use a [**`.RETURN(...)`**](reference.md/#RETURN) or
[**`.LR_RETURN(...)`**](reference.md/#LR_RETURN) with an expression of
the right type.

Example:

```Cpp
class Dictionary
{
public:
  using id_t = size_t;
  MAKE_MOCK1(lookup, std::string(id_t));
};

using trompeloeil::ge; // greater than or equal
using trompeloeil::lt; // less than
void test()
{
  Dictionary d;
  std::vector<std::string> dict{...};
  
  ALLOW_CALL(d, lookup(ge(dict.size())))
    .RETURN("");                          // create std::string from ""
  ALLOW_CALL(d, lookup(lt(dict.size())))
    .LR_RETURN(dict[_1]);                 // access element in vector
  func(&d);
}
```

Above, the [matchers](reference.md/#matcher) [**`lt(...)`**](reference.md/#lt)
and [**`ge(...)`**](reference.md/#ge) are used to ensure that the indexing
in the  local variable `dict` can be made safely. Note that the first
[expectation](reference.md/#expectation) does not match the return type
exactly, but is something that can be implicitly converted.

[**`LR_RETURN(...)`**](reference.md/#LR_RETURN) is used in the second to
avoid copying the vector, since [**`RETURN(...)`**](reference.md/#RETURN)
always accesses copies of local variables.

### <A name="return_references"/> Return references from matching calls

Returning references from matching [expectations](reference.md/#expectation)
triggers some peculiarities in the language. Specifically, it is not
allowed to return a captured local variable as a reference in
[**`RETURN(...)`**](reference.md/#RETURN), and in
[**`LR_RETURN(...)`**](reference.md/#LR_RETURN) a returned variable must be
decorated to ensure that a reference is intended.


Example:

```Cpp
class Dictionary
{
public:
  using id_t = size_t;
  MAKE_MOCK1(lookup, const std::string&(id_t));
};

using trompeloeil::gt; // greater than or equal
using trompeloeil::lt; // less than

std::string global_empty;

void test()
{
  Dictionary d;
  std::vector<std::string> dict{...};

  std::string empty;
  
  ALLOW_CALL(d, lookup(gt(dict.size())))
    .LR_RETURN((empty));                // extra () -> reference to local variable
  ALLOW_CALL(d, lookup(dict.size()))
    .LR_RETURN(std::ref(empty));        // reference to local variable
  ALLOW_CALL(d, lookup(lt(dict.size())))
    .LR_RETURN(dict[_1]);               // result of function call
  ALLOW_CALL(d, lookup(0))
    .RETURN(std::ref(global_empty));    // reference to global variable
  func(&d);
}
```

Captured variable that are returned as references must either be enclosed in
an extra parenthesis, or
[`std::ref()`](http://en.cppreference.com/w/cpp/utility/functional/ref).

Returning a reference obtained from a function call, however, does not
require any extra decoration, as the 3rd
[expectation](reference.md/#expectation) above, which looks up values in
`dict` shows.


### <A name="side_effects"/> Define side effects for matching calls

A side effect, in *Trompeloeil* parlance, is something that is done after
a match has been made for an [expectation](reference.md/#expectation), and
before returning (or throwing.)

Typical side effects are:

 - Setting out parameters
 - Capturing in parameters
 - Calling other functions
 
Example:

```Cpp
class Dispatcher
{
public:
  MAKE_MOCK1(subscribe, void(std::function<void(const std::string&)>));
};

using trompeloeil::_;

void test()
{
  Dispatcher d;

  std::vector<std::function<void(const std::string&)>> clients;

  {
    REQUIRE_CALL(d, subscribe(_))
      .LR_SIDE_EFFECT(clients.push_back(std::move(_1)))
      .TIMES(AT_LEAST(1));
    
    func(&d);
  }
  for (auto& cb : clients) cb("meow");
}
```

Above, any call to `d.subscribe(...)` will have the side effect that the
parameter value is stored in the local vector `clients`.

The test then goes on to call all subscribers.

[**`LR_SIDE_EFFECT(...)`**](reference.md/#LR_SIDE_EFFECT) accesses references
to local variables. There is also
[**`SIDE_EFFECT(...)`**](reference.md/#SIDE_EFFECT), which accesses copies of
local variables.

### <A name="matching_overloads"/> Matching calls to overloaded member functions

Distinguishing between overloads is simple when using exact values to match
since the type follows the values. It is more difficult when you want to use
wildcards and other [matchers](reference.md/#matccher).

One useful matcher is [**`ANY(...)`**](reference.md/#ANY), which behaves
like the open wildcard [**`_`**](reference.md/#wildcard), but has a type.
It is also possible to specify types in the matchers.

Example:

```Cpp
class Mock
{
public:
  MAKE_MOCK1(func, void(int*));
  MAKE_MOCK1(func, void(char*));
};

using namespace trompeloeil;

void test()
{
  Mock m;
  
  REQUIRE_CALL(m, func(ANY(int*)));
  REQUIRE_CALL(m, func(ne<char*>(nullptr)));
  
  func(&m);
}
```

Above, each of the `func` overloads must be called once, the `int*` version with
any pointer value at all, and the `char*` version with a non-null value.

### <A name="allowing_any"/> Allowing any call

By default it is illegal to call any,
[mock function](reference.md/#mock_function), and you provide narrow specifi
expectations according to the needs of your test. However, some times it makes
sense to have a wide open default. That is done with the
[exceptations](reference.md/#expectation)
[**`ALLOW_CALL(...)`**](reference.md/#ALLOW_CALL) and
[**`NAMED_ALLOW_CALL(...)`**](reference.md/#NAMED_ALLOW_CALL). The difference
between them is that **`ALLOW_CALL`** is local in nature and is only valid
until the end of the scope, while **`NAMED_ALLOW_CALL(...)`** can be bound
to a `std::unique_ptr<trompeloeil::expectation>`, which you can control the
lifetime of.

Example:

```Cpp
template <typename T>
class Allocator
{
public:
  MAKE_MOCK1(allocate, T*(size_t));
  MAKE_MOCK1(deallocate, void(T*));
};

using trompeloeil::_;

void test_no_mem()
{
  Allocator<int> ai;
  
  ALLOW_CALL(ai, allocate(_))
    .RETURN(nullptr);
    
  ALLOW_CALL(ai, deallocate(nullptr));
  
  hairy_int_job(&ai);
}
```

The simplistic allocator above is rigged to allow any attempts to allocate
memory, but always return `nullptr`, and only allow deallocation of
`nullptr`.


### <A name="temporary_disallow"/> Temporarily disallowing matching calls

Just as it is sometimes convenient to provide a blanket default behaviour,
it is sometimes desirable to temporarily ban calls.

Example:

```Cpp

#include "hairy_job.h"

template <typename T>
class Allocator
{
public:
  MAKE_MOCK1(allocate, T*(size_t));
  MAKE_MOCK1(deallocate, void(T*));
};


using trompeloeil::_;

void test_restricted_mem()
{
  Allocator<int> ai;
  
  ALLOW_CALL(ai, allocate(_))
    .RETURN(new int[_1]);
    
  ALLOW_CALL(ai, deallocate(_))
    .SIDE_EFFECT(delete[] _1);

  hairy_job<int, Allocator<int>> job(ai, initial_data);

  {
    FORBID_CALL(ai, allocate(_));
    
    job.churn(); // must not use allocator
  }

  job.get_result();
}
```

Above we see a simple Allocator that by default allocates and deallocates
arrays.

The `hairy_job` uses the Allocator for its setup, and is expected to allocate
all memory it needs for the churn in its constructor.

That the `churn` doesn't use the allocator in ensured by the local scope, in
which all calls to `allocate(...)` are forbidden.

This pattern is quite common when writing tests with *Trompeloeil*. Use
wide defaults in the scope of the test case (or in a fixture,) and use
local scopes with specifics, be they forbidden or exact requirements.

### <A name="sequences"/> Expecting several matching calls in some sequences

By default all [expectations](reference.md/#expectation) are equal, and the
only sequencing relationship is that if several match a call, the one last
created is the one matched.

This means that [expectations](reference.md/#expectation) that do not compete
for matching the same call have no ordering relationship at all, they are
logically parallel.

Often this is exactly what you want. When you poke an object, you want this and
that thing to happen and the order between them is irrelevant (for example,
if calling callbacts stored in a hash table, you don't want to impose an
order of those calls.)

There are two very different reasons for using sequence control with
*Trompeloeil*.

One is hinted at above, to impose an order between
[expectations](reference.md/#expectation) that are logically parallel. The
other is to set an exact order of indistinguishable
[expectations](reference.md/#expectation). The latter can be achieved by setting
them up in reverse order of matching, but this can make the test code very
difficult to read.

First example. Impose an order between logically parallel calls:

```Cpp
class FileOps
{
public:
  using handle = int;
  MAKE_MOCK1(open, handle(const std::string&));
  MAKE_MOCK3(write, size_t(handle, const char*, size_t));
  MAKE_MOCK1(close, void(handle));
};

using trompeloeil::ne;

void test()
{
  FileOps ops;
  
  trompeloeil::sequence seq;
  
  int handle = 4711;
  
  REQUIRE_CALL(ops, open("name"))
    .RETURN(handle)
    .IN_SEQUENCE(seq);
    
  REQUIRE_CALL(ops, write(handle, ne(nullptr), ne(0)))
    .RETURN(_3)
    .IN_SEQUENCE(seq);
    
  REQUIRE_CALL(ops, close(handle))
    .IN_SEQUENCE(seq);
    
  test_writes(&ops);
}
```

Without the use of `trompeloeil::sequence` above, all three
[expectations](reference.md/#expectation) would be logically parallel and
all permutations of matches would be considered equally correct.

By imposing an order between them, there is now only one legal sequence
of calls.

The other example is to provide an order between equally matching calls.
Suppose we want the `write` function above to first return 0 once and then
give the desired result:

```Cpp
class FileOps
{
public:
  using handle = int;
  MAKE_MOCK1(open, handle(const std::string&));
  MAKE_MOCK3(write, size_t(handle, const char*, size_t));
  MAKE_MOCK1(close, void(handle));
};

using trompeloeil::ne;

void test()
{
  FileOps ops;
  
  trompeloeil::sequence seq;
  
  int handle = 4711;
  
  REQUIRE_CALL(ops, open("name"))
    .RETURN(handle)
    .IN_SEQUENCE(seq);
    
  REQUIRE_CALL(ops, write(handle, ne(nullptr), ne(0)))
    .RETURN(0)                                         // indicate failure
    .IN_SEQUENCE(seq);

  REQUIRE_CALL(ops, write(handle, ne(nullptr), ne(0)))
    .RETURN(_3)                                        // successful retry
    .IN_SEQUENCE(seq);

  REQUIRE_CALL(ops, close(handle))
    .IN_SEQUENCE(seq);
    
  test_writes(&ops);
}
```

Here the two calls to `write` are supposed to be made with exactly the same
parameters, so they cannot be distinguished that way. We want the first
call to indicate intermittent failure, and to be followed by a retry
that will succeed.

[**`.IN_SEQUENCE(...)`**](reference.md/#IN_SEQUENCE) can refer to several
sequence objects, which is a way to allow some variation in order, without
being too lax. For a more thorough walk through, see the blog post [Sequence
control with the Trompeloeil C++14 Mocking Framework](http://playfulprogramming.blogspot.se/2015/01/sequence-control-with-trompeloeil-c.html)

### <A name="match_count"/> Expecting matching calls a certain number of times

By default [**`REQUIRE_CALL(...)`**](reference.md/#REQUIRE_CALL) needs exactly
one matching call, otherwise a violation is reported. Sometimes the need is
for somethig else. A modifier [**`TIMES(...)`**](reference.md/#TIMES) is used
to change that. You can either specify an exact number of times matching calls
must be made, or a range of numbers.

Example:

```Cpp
class Mock
{
public:
  MAKE_MOCK1(func, void(int));
};

void some_test()
{
  Mock m;
  
  REQUIRE_CALL(m, func(0))
    .TIMES(2);
    
  REQUIRE_CALL(m, func(1))
    .TIMES(3, 5);
    
  REQUIRE_CALL(m, func(2))
    .TIMES(AT_LEAST(3));
    
  REQUIRE_CALL(m, func(3))
    .TIMES(AT_MOST(4));
    
  func(&m);
}
```

Above, `m.func(0)` must be called exactly twise. `m.func(1)` must be called 3, 4
or 5 times. The call 'm.func(2)` must be made 3 or more times. Finally
`m.func(4)` must not be called more that 4 times.


## <A name="lifetime"/> Controlling lifetime of mock objects

If you test a case where you hand over ownership of a
[mock object](refecence.md/#mock_object), you may want to test that the mock
object is destroyed when intended. For this there are is a modifier class
template `trompeloeil::deathwatched<T>` and the macros
[**`REQUIRE_DESTRUCTION(...)`**](reference.md/#REQUIRE_DESTRUCTION) and
[**`NAMED_RELQUIRE_DESTRUCTION(...)`**](reference.md/#NAMED_REQUIRE_DESTRUCTION).

Example:

```Cpp
class Mock
{
public:
  virtual ~Mock() {}          // virtual destructor required for deathwatched<>
  MAKE_MOCK1(func, void(int));
}

template <typename T>
class consumer
{
public:
  consumer(T&&);
  void poke(int n);
private:
  ...
};

void consume_test()
{
  auto owner = std::make_unique<trompeloeil::deathwatched<Mock>>();
  
  auto mock = owner.get(); // use raw unowned pointer
  
  consumer<Mock> c(std::move(owner));
  
  c.poke(3);
  
  {
    REQUIRE_DESTRUCTION(*mock);
  
    c.poke(0);
  }
}
```

Above, the constructor of object `c` takes ownership of the
[mock object](reference.md/#mock_object).

Since the mock object is on deathwatch, destruction is reported as a violation.
Thus we can be sure that if the constructor destroys the mock object, the
test will fail. Likewise if the call `c.poke(3)` would destroy the mock object.

The local scope afterwards has a requirement that the mock object *is* destroyed.
If the call `c.poke(0)` does not destroy the mock, a violation will be reported
and fail the test.


## <A name="tracing"/> Tracing mocks
 
### <A name="stream_tracer"/> Using `trompeloeil::stream_tracer`

### <A name="custom_tracer"/> Writing custom tracers

## <A name="custom_matchers"/> Writing custom matchers

