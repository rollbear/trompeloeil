# Platform and library support for Trompeloeil

- [Using libc\+\+ with Trompeloeil](#using_libcxx)
- [Using sanitizers with Trompeloeil](#using_sanitizers)
- [Compiler versions in sample Linux distributions](#compilers_in_distributions)
  - [Ubuntu](#compilers_in_ubuntu)
    - [In summary](#ubuntu_summary)
    - [In detail](#ubuntu_detail)
  - [Fedora](#compilers_in_fedora)
- [Tested configurations](#tested_configurations)
- [Testing Trompeloeil on Artful Aardvark (Ubuntu 17.10)](#testing_on_artful)
  - [`std::to_string()` is not defined for some versions of `libstd++-v3`](#defect_to_string)
  - [Glibc 2.26 no longer supplies `xlocale.h`](#defect_xlocale)
  - [Glibc 2.26 `std::signbit()` broken for GCC compilers < 6](#defect_signbit)
  - [Conclusion](#artful_conclusion)
- [Support platforms without std::recursive_mutex](#custom_recursive_mutex)  


## <A name="using_libcxx"/> Using libc\+\+ with Trompeloeil

On some distributions `clang` is configured to use `libstdc++-v3` as the
implementation of the C\+\+ Standard Library.  In order to use `libc++`,
pass the `-stdlib=libc++` command line flag to the compiler.

For example,

```text
clang++-5.0 -std=c++14 -stdlib=libc++ <other command line arguments>
```

To use `libc++` with `g++` a few more command line flags need to be passed.
This is a command line known to work with `g++-6`,

```text
g++-6 -std=c++14 -nostdinc++ -isystem/usr/include/c++/v1 \
<other command line arguments> \
-nodefaultlibs -lc++ -lc++abi -lm -lc -lgcc_s -lgcc
```

## <A name="using_sanitizers"/> Using sanitizers with Trompeloeil

Trompeloeil test cases have been compiled and run without error with
AddressSanitizer (ASan), Undefined Behavior Sanitizer (UBSan) and
Thread Sanitizer (TSan).

Feel free to add `-fsanitize=address`, `-fsanitize=thread` or
`-fsanitize=undefined` to your compiler command lines, especially
when unit testing.

Maybe your compiler supports `-fsanitize-address-use-after-scope`.
Add that flag as well.

## <A name="compilers_in_distributions"/> Compiler versions in sample Linux distributions

### <A name="compilers_in_ubuntu"/> Ubuntu

Canonical supports the `main` component of the repositories of a release.

Canonical does not support the `universe` component.  Support for `universe`
is provided by the Ubuntu community. If a compiler or library is in `universe`
for a particular release, then there is no guarantee that there will be a
release of that compiler or library in the next or any future release of
Ubuntu, let alone updates in the current release.

Either migrate compilers and libraries with each Ubuntu release or take
control of your toolchain and remove a dependency on the platform.  In the
latter case you then have the option of supporting the Ubuntu community
with a contribution of your toolchain to `universe`.

For more information, see

ubuntu.com, "Repositories" \
Available: <https://help.ubuntu.com/community/Repositories> \
Accessed: 29 October 2017

#### <A name="ubuntu_summary"/> In summary

```text
                Trusty Tahr                 Xenial Xerus                Zesty Zapus         Artful Aardvark     Bionic Beaver
                (14.04LTS)                  (16.04LTS)                  (17.04)             (17.10)             (18.04LTS)
Released        2014-04-17                  2016-04-21                  2017-04-17          2017-10-19          2018-04
Supported to    2019-04                     2021-04                     2018-01             2018-07             2023-04

Compiler(s)     g++-4.8.4                   g++-5                       g++-6               g++-7               TODO

                clang++-3.5                 clang++-4.0                 clang++-4.0         universe            TODO
                                                                                            clang++-5.0         TODO


libc++-dev      universe                    universe                    universe            universe            TODO
                1.0~svn199600-1             3.7.0                       3.9.1               3.9.1               TODO
```

#### <A name="ubuntu_detail"/> In detail

```text
                Trusty Tahr                 Xenial Xerus                Zesty Zapus         Artful Aardvark     Bionic Beaver
                (14.04LTS)                  (16.04LTS)                  (17.04)             (17.10)             (18.04LTS)
Released        2014-04-17                  2016-04-21                  2017-04-17          2017-10-19          2018-04
Supported to    (2019-04)                   (2021-04)                   (2018-01)           (2018-07)           (2023-04)

GCC

g++-4.8         ports                       universe                    universe            universe            TODO
                4.8.2-19ubuntu1             4.8.5-4ubuntu2              4.8.5-4ubuntu4      4.8.5-4ubuntu6      TODO

                security                    xenial-updates              zesty-updates                           TODO
                4.8.4-2ubuntu1~14.04.3      N/A                         N/A                                     TODO

                trusty-updates              xenial-backports            zesty-backports                         TODO
                4.8.4-2ubuntu1~14.04.3      N/A                         N/A                                     TODO

                trusty-backports
                N/A


g++-4.9         main                        universe                    universe            universe            TODO
                N/A                         4.9.3-13ubuntu2             4.9.4-2ubuntu1      N/A                 TODO

                trusty-updates              xenial-updates              zesty-updates                           TODO
                N/A                         N/A                         N/A                                     TODO

                trusty-backports            xenial-backports            zesty-backports                         TODO
                N/A                         N/A                         N/A                                     TODO


g++-5           main                        ports                       universe            universe            TODO
                N/A                         5.3.1-14ubuntu2             5.4.1-8ubuntu1      5.5.0-1ubuntu1      TODO

                trusty-updates              security                    zesty-updates                           TODO
                N/A                         5.4.0-6ubuntu1~16.04.4      N/A                                     TODO

                trusty-backports            xenial-updates              zesty-backports                         TODO
                N/A                         5.4.0-6ubuntu1~16.04.5      N/A                                     TODO

                                            xenial-backports
                                            N/A


g++-6           main                        main                        ports               universe            TODO
                N/A                         N/A                         6.3.0-8ubuntu1      6.4.0-8ubuntu1      TODO

                trusty-updates              xenial-updates              main                                    TODO
                N/A                         N/A                         6.3.0-12ubuntu2                         TODO

                trusty-backports            xenial-backports            zesty-updates                           TODO
                N/A                         N/A                         N/A                                     TODO

                                                                        zesty-backports
                                                                        N/A


g++-7           main                        main                        main                main                TODO
                N/A                         N/A                         N/A                 7.2.0-8ubuntu3      TODO

                trusty-updates              xenial-updates              zenial-updates                          TODO
                N/A                         N/A                         N/A                                     TODO

                trusty-backports            xenial-backports            zenial-backports                        TODO
                N/A                         N/A                         N/A                                     TODO


Clang

clang-3.5       ports                       universe                    N/A                 N/A                 TODO
                1:3.5~svn201651-1ubuntu1    1:3.5.2-3ubuntu1            N/A                 N/A                 TODO

                security                    xenial-updates              zesty-updates                           TODO
                1:3.5-4ubuntu2~trusty2      N/A                         N/A                                     TODO

                trusty-updates              xenial-backports            zesty-backports                         TODO
                1:3.5-4ubuntu2~trusty2      N/A                         N/A                                     TODO

                trusty-backports
                N/A


clang-3.6       N/A                         universe                    N/A                 N/A                 TODO
                N/A                         1:3.6.2-3ubuntu2            N/A                 N/A                 TODO

                trusty-updates/universe     xenial-updates              zesty-updates                           TODO
                1:3.6-2ubuntu1~trusty1      N/A                         N/A                                     TODO

                trusty-backports            xenial-backports            zesty-backports                         TODO
                N/A                         N/A                         N/A                                     TODO


clang-3.7       N/A                         universe                    universe            N/A                 TODO
                N/A                         1:3.7.1-2ubuntu2            1:3.7.1-3ubuntu4    N/A                 TODO

                trusty-updates              xenial-updates              zesty-updates                           TODO
                N/A                         N/A                         N/A                                     TODO

                trusty-backports            xenial-backports            zesty-backports                         TODO
                N/A                         N/A                         N/A                                     TODO


clang-3.8       security                    universe                    universe            universe            TODO
                1:3.8-2ubuntu3~trusty5      1:3.8-2ubuntu1              1:3.8.1-18ubuntu1   1:3.8.1-24ubuntu7   TODO

                trusty-updates/universe     xenial-updates/universe     zesty-updates                           TODO
                1:3.8-2ubuntu3~trusty5      1:3.8-2ubuntu4              N/A                                     TODO

                trusty-backports            xenial-backports            zesty-backports                         TODO
                N/A                         N/A                         N/A                                     TODO


clang-3.9       security                    security                    ports               universe            TODO
                1:3.9.1-4ubuntu3~14.04.3    1:3.9.1-4ubuntu3~16.04.2    1:3.9.1-5ubuntu1    1:3.9.1-17ubuntu1   TODO

                trusty-updates/universe     xenial-updates/universe     security                                TODO
                1:3.9.1-4ubuntu3~14.04.3    1:3.9.1-4ubuntu3~16.04.2    1:3.9.1-5ubuntu1.1                      TODO

                trusty-backports            xenial-backports            zesty-updates                           TODO
                N/A                         N/A                         1:3.9.1-5ubuntu1.1                      TODO

                                                                        zesty-backports                         TODO
                                                                        N/A                                     TODO


clang-4.0       N/A                         security                    main                universe            TODO
                N/A                         1:4.0-1ubuntu1~16.04.2      1:4.0-1ubuntu1      1:4.0.1-6           TODO

                trusty-updates              xenial-updates              zesty-updates                           TODO
                N/A                         1:4.0-1ubuntu1~16.04.2      N/A                                     TODO

                trusty-backports            xenial-backports            zesty-backports                         TODO
                N/A                         N/A                         N/A                                     TODO


clang-5.0       N/A                         N/A                         N/A                 universe            TODO
                N/A                         N/A                         N/A                 1:5.0-3             TODO

                trusty-updates              xenial-updates              zesty-updates                           TODO
                N/A                         N/A                         N/A                                     TODO

                trusty-backports            xenial-backports            zesty-backports                         TODO
                N/A                         N/A                         N/A                                     TODO

libc++-dev

1.0             universe
                1.0~svn199600-1

                trusty-updates
                N/A

                trusty-backports
                N/A

3.7                                         universe
                                            3.7.0-1

                                            xenial-updates/universe
                                            3.7.0-1ubuntu0.1

                                            xenial-backports
                                            N/A

3.9                                                                     ports               universe
                                                                        3.7.0-1             3.9.1-3

                                                                        universe
                                                                        3.9.1-2

                                                                        zesty-updates
                                                                        N/A

                                                                        zesty-backports
                                                                        N/A

4.0                                                                                                             TODO
                                                                                                                TODO


5.0                                                                                                             TODO
                                                                                                                TODO
```

Table first compiled: 28 October 2017.
Last updated: 28 October 2017.

### <A name="compilers_in_fedora"/> Fedora

A short list of Fedora releases tells a similar story
to the Ubuntu distribution.

```text
                25                          26                          27                          28
Released        2016-11-22                  2017-07-11                  (2017-11-14)                (2018-05-01)
Supported to    TODO                        TODO                        TBD                         TBD

gcc-c++         6.4.1-1.fc25                7.2.1-2.fc26                7.2.1-2.fc27                TODO
g++             6.4.1 20170727              7.2.1 20170915              7.2.1 20170915              TODO
                (Red Hat 6.4.1-1)           (Red Hat 7.2.1-2)           (Red Hat 7.2.1-2)


clang           3.9.1-2.fc25                4.0.1-5.fc26                4.0.1-5.fc27                TODO
clang++         3.9.1                       4.0.1                       4.0.1                       TODO
                (tags/RELEASE_391/final     (tags/RELEASE_401/final)    (tags/RELEASE_401/final)


libcxx-devel    3.9.1-1.fc25                4.0.1-3.fc26                4.0.1-3.fc27                TODO
```

Table first compiled: 28 October 2017
Last updated: 9 November 2017

## <A name="tested_configurations"/> Tested configurations

Before release, Trompeloeil is tested with the following configurations
of compiler, language dialect, and standard library.

### GCC

Last updated: 3 June 2019

Key:

- `N/A`: The combination `g++-4.8/c++11/libc++` leads to
  compile errors and is not currently supported.  Further investigation
  may change this outcome.
- `--`: The version of `libstdc++-v3` lacks a definition of the
  `_GLIBCXX_RELEASE` macro.
- `stdc++` means `libstdc++-v3` from GCC.
- `c++` means `libc++` from Clang.

`g++-latest` means the "live at head" build of `g++`.

```text
Compiler        Mode      stdc++                            c++
                -std=     __GLIBCXX__   _GLIBCXX_RELEASE    _LIBCPP_VERSION
----------      ----      ------------------------------    ---------------
g++-4.8         c++11     20150623      --                  N/A


g++-4.9         c++11     20160726      --                  8000
                c++14

g++-5           c++11     20171010      --                  8000
                c++14
                c++17

g++-6           c++11     20181026      --                  8000
                c++14
                c++17


g++-7           c++11     20190326      7                   8000
                c++14
                c++17


g++-8           c++11     20190406      8                   8000
                c++14
                c++17
                c++2a


g++-9           c++11     20190402      9                   8000
                c++14
                c++17
                c++2a


g++-latest      c++11     20190421      9                   8000
                c++14
                c++17
                c++2a
```

### Clang

`clang++-latest` means the "live at head" version of `clang++`.

```text
Compiler        Mode      stdc++                            c++
                -std=     __GLIBCXX__   _GLIBCXX_RELEASE    _LIBCPP_VERSION
----------      ----      ------------------------------    ---------------
clang++-3.5     c++11     20190326      7                   1101
                c++14


clang++-3.6     c++11     20190326      7                   1101
                c++14


clang++-3.7     c++11     20190326      7                   3700
                c++14


clang++-3.8     c++11     20190326      7                   3800
                c++14


clang++-3.9     c++11     20190402      9                   3900
                c++14


clang++-4.0     c++11     20190402      9                   4000
                c++14


clang++-5.0     c++11     20190402      9                   5000
                c++14
                c++17
                c++2a


clang++-6.0     c++11     20190402      9                   6000
                c++14
                c++17
                c++2a


clang++-7       c++11     20190402      9                   7000
                c++14
                c++17
                c++2a


clang++-8       c++11     20190402      9                   8000
                c++14
                c++17
                c++2a


clang++-latest  c++11     20190402      9                   9000
                c++14
                c++17
                c++2a
```

### Microsoft Visual Studio

Last update: 3 June 2019

Tested with Visual Studio Community 2019 16.1.1 .

```text
Platform Toolset            Configuration   Platform
-------------------------   -------------   --------
Visual Studio 2015 (v140)   Debug           x64
Visual Studio 2017 (v141)   Release         x86
Visual Studio 2019 (v142)
```

## <A name="testing_on_artful"/> Testing Trompeloeil on Artful Aardvark (Ubuntu 17.10)

The release of Artful Aardvark (Ubuntu 17.10) contains a number of issues
requiring workarounds if you want to compile and test Trompeloeil with
community supported compiler versions e.g. any version of `clang++`,
any version `g++` less than 7, or community supported libraries e.g.
any version of `libc++`.

Canonical supported compilers and libraries -
just `g++-7` with `libstdc++-v3` - do not have the issues described below,
but this is rather a narrow list for testing Trompeloeil on its
supported compilers and libraries.

### <A name="defect_to_string"/> `std::to_string()` is not defined for some versions of `libstd++-v3`

Affects: `libstdc++-v3` from these packages

- `libstdc++-4.8-dev:amd64 4.8.5-4ubuntu6`
  - See: <https://bugs.launchpad.net/ubuntu/+source/gcc-4.8/+bug/1725847>
- `libstdc++-5-dev:amd64 5.5.0-1ubuntu1`
  - See: <https://bugs.launchpad.net/ubuntu/+source/gcc-5/+bug/1725848>

Workaround: Add `-D_GLIBCXX_USE_C99=1` to your compiler command lines.

### <A name="defect_xlocale"/> Glibc 2.26 no longer supplies `xlocale.h`

The version of `glibc` in package `libc6-dev (2.26-0ubuntu2)`
drops support for `xlocale.h`.

`libc++` tracked this change and supplied a fix for 5.0.

See: "Fix libcxx build with glibc 2.26+ by removing xlocale.h include." \
Available: <https://github.com/llvm-mirror/libcxx/commit/6e02e89f65ca1ca1d6ce30fbc557563164dd327e> \
Accessed: 11 November 2017

But Artful Aardvark ships package `libc++-dev 3.9.1-3`.
As a consequence, no software using `libc++` out-of-the-box version
can compile on Artful.

Workaround: Create a symlink from `locale.h` to `xlocale.h`

```text
cd /usr/include
sudo ln -s locale.h xlocale.h
```

### <A name="defect_signbit"/> Glibc 2.26 `std::signbit()` broken for GCC compilers < 6

A defect in GLIBC 2.26 prevents programs using `signbit()` from `math.h`
from compiling with `g++-4.8`, `g++-4.9`, or `g++-5`.

This happens to include any uses of `libc++`, which requires a
functioning `signbit()` to compile function template `__libcpp_signbit()`
in file `math.h`.

The Clang compilers happen to work with this part of `glibc` 2.26
as they don't implement 128-bit floating point and a different
code path is followed, even for the earliest supported compilers.

See: <https://bugs.launchpad.net/ubuntu/+source/glibc/+bug/1725869>

Workaround: Patch your local copy of `math.h` in `glibc` with the
fix from `glibc` upstream, found by following the links in this bug report:

See: "Bug 22296 - glibc 2.26: signbit build issue with Gcc 5.5.0 on x86_64" \
Available: <https://sourceware.org/bugzilla/show_bug.cgi?id=22296> \
Accessed: 11 November 2017

### <A name="artful_conclusion"/> Conclusion

Hopefully updated packages for `glibc` (`libc6-dev`), `libc++`, and
`libstdc++-dev` for `g++-4.8` and `g++-5` will be released allowing
patch-free building and testing of Trompeloeil on Artful Aardvark.

A better strategy may be to build GLIBC, GCC 4.8, GCC 5.x, and `libc++`
from source and use these to build your software.  Then consider
contributing your build to the Ubuntu Community; you just might be the
"support" in "community supported".

## <A name="custom_recursive_mutex"/> Support platforms without std::recursive_mutex

Some platforms, especially MCUs with RTOS, don't have native support for std::recursive_mutex.
To use your own recursive mutex, define `TROMPELOEIL_RECURSIVE_MUTEX` either before including the Trompeloeil header
(e.g. `#define TROMPELOEIL_RECURSIVE_MUTEX MyRecursiveMutexClass`) or as preprocessor definition
(e.g. GCC: `-DTROMPELOEIL_RECURSIVE_MUTEX=MyRecursiveMutexClass`).
