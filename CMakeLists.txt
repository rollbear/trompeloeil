cmake_minimum_required(VERSION 3.0)
project(trompeloeil)

include(GNUInstallDirs)
include(ExternalProject)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/trompeloeil/trompeloeil-config-version.cmake"
  VERSION 28
  COMPATIBILITY AnyNewerVersion)

set(CMAKE_CXX_STANDARD 14)

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

  set(TSAN "-fsanitize=undefined,thread")
  set(SSAN "-fsanitize=address,undefined")

  add_executable(
    self_test
    EXCLUDE_FROM_ALL
    test/compiling_tests.cpp
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
