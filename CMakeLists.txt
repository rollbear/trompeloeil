cmake_minimum_required(VERSION 3.2)
project(trompeloeil)

option(TROMPELOEIL_INSTALL_DOCS "Install documentation" ON)

include(GNUInstallDirs)
include(ExternalProject)
include(CMakePackageConfigHelpers)
include(CheckCXXCompilerFlag)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/trompeloeil/trompeloeil-config-version.cmake"
  VERSION 39
  COMPATIBILITY AnyNewerVersion)

add_library(trompeloeil INTERFACE)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  trompeloeil
  INTERFACE
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
)

target_include_directories(
  trompeloeil
  INTERFACE
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)

set(MASTER_PROJECT OFF)
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  set(MASTER_PROJECT ON)
endif()

if (MASTER_PROJECT AND CMAKE_BUILD_TYPE MATCHES Debug)

  if (${CXX_STANDARD})
    set(CMAKE_CXX_STANDARD ${CXX_STANDARD})
  else()
    set(CMAKE_CXX_STANDARD 14)
  endif()
  set(CMAKE_CXX_STANDARD_REQUIRED YES)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set(CATCH_DIR ${CMAKE_CURRENT_BINARY_DIR}/catch)
  if(NOT EXISTS ${CATCH_DIR}/catch.hpp)
    if (NOT EXISTS ${CATCH_DIR})
      make_directory(${CATCH_DIR})
    endif()
    file(
      DOWNLOAD
        https://github.com/catchorg/Catch2/releases/download/v2.9.2/catch.hpp  ${CATCH_DIR}/catch.hpp
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
      check_cxx_compiler_flag("-Wno-extra-semi-stmt" WARN_SEMI_STMT)

      if (WARN_SEMI_STMT)
        string(APPEND
               WARN_FLAGS
               " -Wno-extra-semi-stmt")
      endif()

      check_cxx_compiler_flag("-Wno-zero-as-null-pointer-constant" WARN_ZERO_NULL)

      if (WARN_ZERO_NULL)
        string(APPEND
               WARN_FLAGS
               " -Wno-zero-as-null-pointer-constant")
      endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
      string(CONCAT
             WARN_FLAGS
             "-Wall"
             " -Wextra"
             " -pedantic"
             " -Wshadow"
             " -Wconversion")
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

    check_cxx_compiler_flag(/permissive HAS_PERMISSIVE_FLAG)
    if(HAS_PERMISSIVE_FLAG)
      add_compile_options(/permissive-)
    endif()

  endif() # MSVC

  add_executable(
    self_test
    EXCLUDE_FROM_ALL
    test/compiling_tests.cpp
    test/compiling_tests_11.cpp
    test/compiling_tests_14.cpp
    include/catch2/trompeloeil.hpp)

  target_include_directories(
    self_test
    PRIVATE
    ${CATCH_DIR}
  )

  if (SANITIZE)
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
  )

  add_executable(
    thread_terror
    EXCLUDE_FROM_ALL
    test/thread_terror.cpp
    )

  target_link_libraries(
    thread_terror
    PUBLIC
      trompeloeil
      pthread
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
    EXCLUDE_FROM_ALL
    test/custom_recursive_mutex.cpp
  )

  target_link_libraries(
    custom_recursive_mutex
    PUBLIC
    trompeloeil
    pthread
  )

  if (SANITIZE)
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

  # Shameless hack to get target to work on Windows.
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    add_custom_target(
      run_self_test
      COMMAND
        ${CMAKE_CURRENT_BINARY_DIR}/self_test
      DEPENDS
        self_test
    )

  else()

    add_custom_target(
      run_self_test
      COMMAND
        ${CMAKE_CURRENT_BINARY_DIR}/Debug/self_test
      DEPENDS
        self_test
    )

  endif()

  if(TRAVIS_JOB_ID)
    set(COVERALLS_FLAG "--coveralls-id=${TRAVIS_JOB_ID}")
  endif()

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")

    add_custom_target(
      run_coverage
      COMMAND
        kcov --skip-solibs --include-pattern=trompeloeil.hpp ${COVERALLS_FLAG} ./coverage ${CMAKE_CURRENT_BINARY_DIR}/self_test
      DEPENDS
        self_test
    )

  endif()

endif() # MASTER_PROJECT AND CMAKE_BUILD_TYPE MATCHES Debug

install(
  TARGETS
    trompeloeil
  EXPORT
    trompeloeil-targets
  INCLUDES DESTINATION
    include
)

install(
  EXPORT
    trompeloeil-targets
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/trompeloeil
)
install(
  FILES
    trompeloeil-config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/trompeloeil/trompeloeil-config-version.cmake"
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/trompeloeil
  COMPONENT
    Devel
)

install(
  DIRECTORY
    include/
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}
)

if(TROMPELOEIL_INSTALL_DOCS)
  install(
    FILES
      LICENSE_1_0.txt
    DESTINATION
      ${CMAKE_INSTALL_DOCDIR}
  )

  install(
    DIRECTORY
      docs
    DESTINATION
      ${CMAKE_INSTALL_DOCDIR}
  )
endif()
