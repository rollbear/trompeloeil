# *Trompeloeil*

![trompeloeil logo](trompeloeil-logo.png)

<!-- markdownlint-disable no-space-in-links -->
![CI](https://github.com/rollbear/trompeloeil/workflows/CI/badge.svg)
[![codecov](https://codecov.io/gh/rollbear/trompeloeil/branch/master/graph/badge.svg?token=PCUO4knwdU)](https://codecov.io/gh/rollbear/trompeloeil)

Get: [![Conan](https://img.shields.io/badge/on-conan-blue.svg)](https://conan.io/center/trompeloeil)

<a href="https://www.buymeacoffee.com/bjornfahller"> <img src="https://cdn.buymeacoffee.com/buttons/v2/default-orange.png" height="50" width="210" alt="Buy me a coffee"/></a>

> *trompe l'oeil* noun    (Concise Encyclopedia)
> Style of representation in which a painted object is intended
> to deceive the viewer into believing it is the object itself...

# What is it?

A thread-safe header-only mocking framework for C++11/14 using the Boost Software License 1.0

# Documentation

- [Integrating with unit test frame works](docs/CookBook.md/#unit_test_frameworks)
- [Introduction](https://playfulprogramming.blogspot.com/2014/12/introducing-trompeloeil-c-mocking.html)
- [Building and running the self test suite](#self_test)
- [How to contribute](#contribute)
- [Compiler compatibility](#compilers)
- [External tools](#tools)
- [Presentation videos](#videos)
- [Trompeloeil on CppCast](http://cppcast.com/2017/02/bjorn-fahller/)
- [Cheat Sheet (2*A4)](docs/trompeloeil_cheat_sheet.pdf)
- [Cook Book](docs/CookBook.md)
- [FAQ](docs/FAQ.md)
- [Backward compatibility with earlier versions of C++](docs/Backward.md)
- [Platform and library support for Trompeloeil](docs/PlatformsAndLibraries.md)
- [Reference](docs/reference.md)

Also, follow up with the post on
[sequencing](
  https://playfulprogramming.blogspot.se/2015/01/sequence-control-with-trompeloeil-c.html
) for examples on how to restrict or relax allowed sequences of matching calls.

# Teaser

```Cpp
#include <trompeloeil.hpp>

class Interface
{
public:
  virtual ~Interface() = default;
  virtual bool foo(int, std::string& s) = 0;
  virtual bool bar(int) = 0;
  virtual bool bar(std::string) = 0;
};

void interface_func(Interface*); // function to test

class Mock : public Interface
{
public:
  MAKE_MOCK2(foo, bool(int, std::string&),override);
  MAKE_MOCK1(bar, bool(int),override);
  MAKE_MOCK1(bar, bool(std::string),override);
  MAKE_MOCK0(baz, void()); // not from Interface
};

TEST(exercise_interface_func)
{
  using trompeloeil::_;  // wild card for matching any value
  using trompeloeil::gt; // greater-than match

  Mock m;

  trompeloeil::sequence seq1, seq2;  // control order of matching calls

  int local_var = 0;

  REQUIRE_CALL(m, bar(ANY(int)))     // expect call to m.bar(int)
    .LR_SIDE_EFFECT(local_var = _1)  // set captured variable to value of param
    .RETURN(_1 > 0)                  // return value depending on param value
    .IN_SEQUENCE(seq1)               // must be first match for seq1
    .TIMES(AT_LEAST(1));             // can be called several times

  FORBID_CALL(m, bar(0));            // but m.bar(0) is not allowed

  REQUIRE_CALL(m, bar("word"))       // expect one call to m.bar(std::string)
    .RETURN(true)
    .IN_SEQUENCE(seq2);              // must be first match for seq2

  REQUIRE_CALL(m, foo(gt(2), _))     // expect call to foo(int,std::string&)
    .WITH(_2 == "")                  // with int > 2 and empty string
    .IN_SEQUENCE(seq1, seq2)         // last for both seq1 and seq2
    .SIDE_EFFECT(_2 = "cat")         // and set param string to "cat"
    .RETURN(true);

  interface_func(&m);

  // all the above expectations must be fulfilled here
}
```

# <A name="self_test"/> Building and running the self test suite

To build the self test suite run `cmake` with `-DTROMPELOEIL_BUILD_TESTS=yes`.
Use the options `CXX_STANDARD` to select which C++ standard to test, and
`SANITIZE` to select sanitizers to build with. Note that the self tests needs a
reasonably modern version of CMake. Example:

```
cmake -B build_dir \
      -D TROMPELOEIL_BUILD_TESTS=yes \
      -D CMAKE_BUILD_TYPE=Debug \
      -D CXX_STANDARD=17 \
      -D SANITIZE=Address,Undefined \
      <trompeloeil source dir>
```

If the build finds a CMake package for `Catch2` it will use that, otherwise it
will download a header-only version of Catch2 v2.x.

```
cmake --build build_dir -t self_test thread_terror custom_recursive_mutex
```

Then run the built binaries:

```
./build_dir/self_test && ./build_dir/thread_terror && ./build_dir/custom_recursive_mutex
```

# <A name="contribute"/> How to contribute

Contributions are most welcome. For new functionality, please file an issue as
an enhancement request first, to get a discussion going about how to best
implement it. Also for bugfixes, it is good to file an issue, so that others can
see what the problem is and when it's solved. Internal changes are normally not
mentioned in the ChangeLog - it should typically reflect what a user can see
(however, performance improvements and silencing warnings are visible for
users.) Feel free to add your name to the copyright blurb.

|Change                       | PR to          |
|-----------------------------|----------------|
|Documentation                |master branch   |
|Trivial bugfixes             |master branch   |
|Non trivial bugfixes         |develop branch  |
|Simple new functionality     |develop branch  |
|Non-trivial new functionality|new topic branch|

# <A name="compilers"/> Compiler compatibility

Trompeloeil is known to work with:

- GCC [4.8.4](docs/Backward.md#gxx48x_limitations)+, 4.9.3+, 5, 6, 7, 8, 9, 10, 11, 12
- Clang 3.5, 3.6, 3.7, 3.8, 3.9, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
- Visual Studio 2015, 2017, 2019

Latest patch level releases are assumed in the versions listed above.

Further details on C++11 support, platform and library limitations, may
be found in

- [Backward compatibility with earlier versions of C++](docs/Backward.md)
- [Platform and library support for Trompeloeil](docs/PlatformsAndLibraries.md)

# <A name="tools"/> External Tools

- [ReSharperC++](https://www.jetbrains.com/resharper-cpp/) extension to
  [VisualStudio](https://visualstudio.microsoft.com/) has a mock generator for
  *Trompeloeil* since [2016.2](https://blog.jetbrains.com/rscpp/2016/09/14/whats-new-in-resharper-c-2016-2/)
  
# <A name="videos"/> Videos
|  |  |
|--|--|
| [![Mocking Modern C++ with Trompeloeil](https://img.youtube.com/vi/mPYNsARvTDk/mqdefault.jpg)](https://www.youtube.com/watch?v=mPYNsARvTDk)| *Mocking Modern C++ with Trompeloeil*, introduction to Trompeloeil by Björn Fahller from from from Stockholm C++ UG (34m)| [(Slides)](https://speakerdeck.com/rollbear/mocking-modern-c-plus-plus-with-trompeloeil) |
| [![Using Trompeloeil, a Mocking Framework for Modern C++](https://img.youtube.com/vi/vvQ-kK4coYM/mqdefault.jpg)](https://www.youtube.com/watch?v=vvQ-kK4coYM)| *Using Trompeloeil, a Mocking Framework for Modern C++*, by Björn Fahller from NDC{Oslo} 2017 (52m) [(Slides)](https://speakerdeck.com/rollbear/ndc-oslo-using-trompeloeil-a-mocking-framework-for-modern-c-plus-plus) |
| [![Using Trompeloeil, a Mocking Framework for Modern C++](https://img.youtube.com/vi/HCh6cs9nXt0/mqdefault.jpg)](https://www.youtube.com/watch?v=HCh6cs9nXt0) | *Using Trompeloeil, a Mocking Framework for Modern C++*, Detailed presentation by Björn Fahller from ACCU 2017 (1h25m) [(Slides with extra material)](https://speakerdeck.com/rollbear/using-trompeloeil-a-mocking-framework-for-modern-c-plus-plus) |
| [![Packporting to the future](https://img.youtube.com/vi/KKvSVyZ4_5k/mqdefault.jpg)](https://www.youtube.com/watch?v=KKvSVyZ4_5k) | *Backporting to the Future*, Detailing the C++11 API and how it came to be, by Andrew Paxie from Pacific++ 2018 (53m) [(Slides)](https://github.com/pacificplusplus/conference/blob/master/slides-2018/backporting-to-the-future/slides.pdf) |
