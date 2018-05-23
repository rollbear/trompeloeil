# self_test
add_executable(
  self_test
  EXCLUDE_FROM_ALL
  compiling_tests.cpp
  compiling_tests_11.cpp
  compiling_tests_14.cpp
)

target_include_directories(
  self_test
  PRIVATE
  ${CATCH_DIR}
)

target_link_libraries(
  self_test
  PUBLIC
    trompeloeil
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

add_test(NAME run_self_test COMMAND self_test)

# thread_terror
add_executable(
  thread_terror
  EXCLUDE_FROM_ALL
  thread_terror.cpp
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

#add_test(NAME run_thread_terror COMMAND thread_terror)

# coverage
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (UNIX AND NOT APPLE))

  # kcov is only available on Linux platforms.
  ExternalProject_Add(
    kcov
    GIT_REPOSITORY
      https://github.com/simonkagstrom/kcov
    GIT_TAG
      v34
    INSTALL_DIR
      ${CMAKE_CURRENT_BINARY_DIR}/kcov
    CMAKE_ARGS
      "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/kcov"
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
