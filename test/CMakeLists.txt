cmake_minimum_required(VERSION 3.19)

option(TROMPELOEIL_CATCH2_PREBUILT "Major version of prebuilt catch2")
option(TROMPELOEIL_CATCH2_PREFIX "Path to catch2 installation")

if (${CXX_STANDARD})
  set(CMAKE_CXX_STANDARD ${CXX_STANDARD})
else()
  set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

if (${CMAKE_CXX_STANDARD} GREATER 11)
  find_package(Catch2 3 QUIET)
endif()
if (NOT Catch2_FOUND)
  find_package(Catch2 2...<3 QUIET)
  if (NOT Catch2_FOUND)
    set(CATCH_DIR ${CMAKE_CURRENT_BINARY_DIR}/catch)
    if (NOT EXISTS "${CATCH_DIR}/catch2/catch.hpp")
      if (NOT EXISTS ${CATCH_DIR})
        FILE(MAKE_DIRECTORY "${CATCH_DIR}/catch2")
      endif()
      file(
              DOWNLOAD
              https://github.com/catchorg/Catch2/releases/download/v2.13.7/catch.hpp  ${CATCH_DIR}/catch2/catch.hpp
              STATUS
              status
              LOG
              log
      )
      list(GET status 0 status_code)
      list(GET status 1 status_string)

      if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "error downloading catch: ${status_string}"
                "${log}")
      endif()
    endif()
    add_library(Catch2 INTERFACE)
    add_library(Catch2::Catch2 ALIAS Catch2)
    target_include_directories(Catch2 INTERFACE "${CATCH_DIR}")
    set(Catch2_VERSION_MAJOR 2)
  endif()
endif()
if (TARGET Catch2::Catch2WithMain)
  set(CATCH2_MAIN Catch2::Catch2WithMain)
  add_compile_definitions(CATCH2_MAIN)
endif()
add_compile_definitions(CATCH2_VERSION=${Catch2_VERSION_MAJOR})

# Assumptions:
# Clang and GNU compilers run on Linux or Linux-like platforms.
# MSVC compilers run on Windows platforms.

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")

  set(
          LIBCXX_PREFIX_PATH
          "/usr/lib/llvm-8"
          CACHE
          PATH
          "Path prefix to libc++"
  )

  # Interpret CXX_STDLIB, if specified
  if (NOT "${CXX_STDLIB}" STREQUAL "")

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")

      # Emulate -stdlib for g++
      if ("${CXX_STDLIB}" STREQUAL "libc++")

        # Disable standard library and
        # set include path to appropriate libc++ headers
        string(CONCAT
                LIBCXX_INCLUDE_PATH
                "-isystem"
                "${LIBCXX_PREFIX_PATH}"
                "/include/c++/v1")
        string(CONCAT
                STDLIB_COMPILE_FLAGS
                "-nostdinc++"
                " "
                "${LIBCXX_INCLUDE_PATH}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STDLIB_COMPILE_FLAGS}")

      elseif ("${CXX_STDLIB}" STREQUAL "libstdc++")

        # This is the default library for g++, nothing to do.

      else()

        # Unknown CXX_STDLIB option
        message(FATAL_ERROR
                "CXX_STDLIB only understands libc++ and libstdc++, not "
                "'${CXX_STDLIB}'")

      endif()

    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

      if (("${CXX_STDLIB}" STREQUAL "libc++") OR
      ("${CXX_STDLIB}" STREQUAL "libstdc++"))

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=${CXX_STDLIB}")

      else()

        # Unknown CXX_STDLIB option
        message(FATAL_ERROR
                "CXX_STDLIB only understands libc++ and libstdc++, not "
                "'${CXX_STDLIB}'")

      endif()

    else()

      message(FATAL_ERROR
              "Only g++ and Clang++ compilers support CXX_STDLIB")

    endif()

  endif()

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    string(CONCAT
            WARN_FLAGS
            "-Weverything"
            " -Wno-c++98-compat-pedantic"
            " -Wno-padded"
            " -Wno-weak-vtables"
            " -Wno-exit-time-destructors"
            " -Wno-global-constructors")

    # Disable for Catch2.
    # See <https://github.com/catchorg/Catch2/issues/1456>
    check_cxx_compiler_flag("-Wno-extra-semi-stmt" WARN_SEMI_STMT)

    if (WARN_SEMI_STMT)
      string(APPEND
              WARN_FLAGS
              " -Wno-extra-semi-stmt")
    endif()

    # Disable for Catch2.
    # See <https://github.com/catchorg/Catch2/issues/578>
    check_cxx_compiler_flag("-Wno-reserved-identifier" WARN_RESERVED_ID)

    if (WARN_RESERVED_ID)
      string(APPEND
              WARN_FLAGS
              " -Wno-reserved-identifier")
    endif()

  elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    string(CONCAT
            WARN_FLAGS
            "-Wall"
            " -Wextra"
            " -pedantic"
            " -Wshadow"
            " -Wconversion")
    check_cxx_compiler_flag("-Wnonnull" WARN_NONNULL)

    if (WARN_NONNULL)
      string(APPEND
              WARN_FLAGS
              " -Wnonnull")
    endif()
  endif()

  set(WARN_FLAGS "${WARN_FLAGS} -Werror")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARN_FLAGS}")

  # Default sanitizer target properties.
  set(TSAN "-fsanitize=undefined,thread")
  set(SSAN "-fsanitize=undefined,address")

  # Exceptions to sanitizer target properties based on compiler and compiler version.
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")

    if (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.0"))
      set(SSAN "-fsanitize=undefined,address -fsanitize-address-use-after-scope")
    else()
      set(SSAN "-fsanitize=undefined,address")
    endif()

  elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    if ((NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8")) AND
    (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9"))
      set(TSAN "-fsanitize=thread")
      set(SSAN "-fsanitize=address")
    endif()

  endif()

endif() # Clang or GNU

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

  add_compile_options(/W4)
  add_compile_options(/bigobj)
  add_compile_options(/EHsc)

  check_cxx_compiler_flag(/permissive HAS_PERMISSIVE_FLAG)
  if(HAS_PERMISSIVE_FLAG)
    add_compile_options(/permissive-)
  endif()

endif() # MSVC

add_executable(
        self_test
        compiling_tests.cpp
        compiling_tests_11.cpp
        compiling_tests_14.cpp)


if (SANITIZE AND NOT APPLE)
  set_target_properties(
          self_test
          PROPERTIES
          LINK_FLAGS
          "${SSAN} -fuse-ld=gold"
          COMPILE_FLAGS
          ${SSAN}
  )
endif()

target_link_libraries(
        self_test
        PUBLIC
        trompeloeil
        ${CATCH2_MAIN}
        Catch2::Catch2
)


find_package(Threads REQUIRED)

add_executable(
        thread_terror
        thread_terror.cpp
)

target_link_libraries(
        thread_terror
        PUBLIC
        trompeloeil
        Threads::Threads
)

if (SANITIZE)
  set_target_properties(
          thread_terror
          PROPERTIES
          LINK_FLAGS
          ${TSAN}
          COMPILE_FLAGS
          ${TSAN}
  )
endif()

add_executable(
        custom_recursive_mutex
        custom_recursive_mutex.cpp
)

target_link_libraries(
        custom_recursive_mutex
        PUBLIC
        trompeloeil
        Threads::Threads
)

if (SANITIZE AND NOT APPLE)
  set_target_properties(
          custom_recursive_mutex
          PROPERTIES
          LINK_FLAGS
          "${SSAN} -fuse-ld=gold"
          COMPILE_FLAGS
          ${SSAN}
  )
endif()

# Linker support for CXX_STDLIB argument
if (NOT ("${CXX_STDLIB}" STREQUAL ""))

  if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    if ("${CXX_STDLIB}" STREQUAL "libc++")

      set(LIBCXX_LIBRARY_PATH "${LIBCXX_PREFIX_PATH}/lib")
      set(STDLIB_LINK_FLAGS
              -nodefaultlibs
              -L${LIBCXX_LIBRARY_PATH}
              -Wl,-rpath,${LIBCXX_LIBRARY_PATH}
              )
      set(STDLIB_LINK_LIBRARIES
              "-lc++"
              "-lc++abi"
              "-lm"
              "-lc"
              "-lgcc_s"
              "-lgcc"
              )

      set_target_properties(
              self_test
              PROPERTIES
              LINK_FLAGS
              ${STDLIB_LINK_FLAGS}
      )

      target_link_libraries(
              self_test
              PUBLIC
              ${STDLIB_LINK_LIBRARIES}
      )

      set_target_properties(
              thread_terror
              PROPERTIES
              LINK_FLAGS
              ${STDLIB_LINK_FLAGS}
      )

      target_link_libraries(
              thread_terror
              PUBLIC
              ${STDLIB_LINK_LIBRARIES}
      )

      set_target_properties(
              custom_recursive_mutex
              PROPERTIES
              LINK_FLAGS
              ${STDLIB_LINK_FLAGS}
      )

      target_link_libraries(
              custom_recursive_mutex
              PUBLIC
              ${STDLIB_LINK_LIBRARIES}
      )

    endif()

  endif()

endif()
