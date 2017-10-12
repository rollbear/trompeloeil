cmake_minimum_required(VERSION 3.2)
project(trompeloeil)

include(GNUInstallDirs)
include(ExternalProject)
include(CMakePackageConfigHelpers)
include(CheckCXXCompilerFlag)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/trompeloeil/trompeloeil-config-version.cmake"
  VERSION 28
  COMPATIBILITY AnyNewerVersion)

if (CXX_STANDARD MATCHES "11")
  set(CMAKE_CXX_STANDARD 11)
else()
  set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

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

  set(CMAKE_CXX_STANDARD_REQUIRED YES)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set(CATCH_DIR ${CMAKE_CURRENT_BINARY_DIR}/catch)
  if(NOT EXISTS ${CATCH_DIR}/catch.hpp)
    if (NOT EXISTS ${CATCH_DIR})
      make_directory(${CATCH_DIR})
    endif()
    file(
      DOWNLOAD
        https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp  ${CATCH_DIR}/catch.hpp
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

  ExternalProject_Add(
    kcov
    GIT_REPOSITORY
      https://github.com/simonkagstrom/kcov
    GIT_TAG
      v33
    INSTALL_DIR
      ${CMAKE_CURRENT_BINARY_DIR}/kcov
    CMAKE_ARGS
      "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/kcov"
    )

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      string(CONCAT
             WARN_FLAGS
             "-Weverything"
             " -Wno-c++98-compat-pedantic"
             " -Wno-padded"
             " -Wno-weak-vtables"
             " -Wno-exit-time-destructors"
             " -Wno-global-constructors")
      CHECK_CXX_COMPILER_FLAG("-Wno-inconsistent-missing-destructor-override"
                              W_MISSING_DTOR_OVERRIDE)
      if (W_MISSING_DTOR_OVERRIDE)
        string(CONCAT
               WARN_FLAGS
               " -Wno-inconsistent-missing-destructor-override")
      endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
      string(CONCAT
             WARN_FLAGS
             "-Wall"
             " -Wextra"
             " -pedantic"
             " -Wshadow")
    endif()
    set(WARN_FLAGS "${WARN_FLAGS} -Werror")
  endif()
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
    elseif (NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.0"))
      set(SSAN "-fsanitize=undefined,address -fno-sanitize-address-use-after-scope")
    endif()

  endif()

  add_executable(
    self_test
    EXCLUDE_FROM_ALL
    test/compiling_tests.cpp
    test/compiling_tests_11.cpp
    test/compiling_tests_14.cpp
  )

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

  add_custom_target(
    run_self_test
    COMMAND
      ${CMAKE_CURRENT_BINARY_DIR}/self_test
    DEPENDS
      self_test
  )

  if(TRAVIS_JOB_ID)
    set(COVERALLS_FLAG "--coveralls-id=${TRAVIS_JOB_ID}")
  endif()

  add_custom_target(
    run_coverage
    COMMAND
      ${CMAKE_CURRENT_BINARY_DIR}/kcov/bin/kcov --skip-solibs --include-pattern=trompeloeil.hpp ${COVERALLS_FLAG} ./coverage ${CMAKE_CURRENT_BINARY_DIR}/self_test
    DEPENDS
      self_test
      kcov
  )
endif()

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
    lib/cmake/trompeloeil
)
install(
  FILES
    trompeloeil-config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/trompeloeil/trompeloeil-config-version.cmake"
  DESTINATION
    lib/cmake/trompeloeil
  COMPONENT
    Devel
)

install(
  FILES
    include/trompeloeil.hpp
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}
)

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
