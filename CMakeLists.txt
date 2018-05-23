cmake_minimum_required(VERSION 3.2)
project(trompeloeil)

include(GNUInstallDirs)
include(ExternalProject)
include(CMakePackageConfigHelpers)
include(CheckCXXCompilerFlag)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/trompeloeil/trompeloeil-config-version.cmake"
  VERSION 31
  COMPATIBILITY AnyNewerVersion
)

if (CXX_STANDARD MATCHES "11")
  set(CMAKE_CXX_STANDARD 11)
else()
  set(CMAKE_CXX_STANDARD 14)
endif()

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

  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED YES)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set(CATCH_DIR ${CMAKE_CURRENT_BINARY_DIR}/catch)
  if(NOT EXISTS ${CATCH_DIR}/catch.hpp)
    if (NOT EXISTS ${CATCH_DIR})
      make_directory(${CATCH_DIR})
    endif()
    file(
      DOWNLOAD
        https://raw.githubusercontent.com/catchorg/Catch2/v2.2.2/single_include/catch.hpp  ${CATCH_DIR}/catch.hpp
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

  # Enable verbose output from Makefile builds.
  set(CMAKE_VERBOSE_MAKEFILE ON)

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
    add_compile_options(/WX)
    add_compile_options(/bigobj)

    check_cxx_compiler_flag(/permissive HAS_PERMISSIVE_FLAG)
    if(HAS_PERMISSIVE_FLAG)
      add_compile_options(/permissive-)
    endif()

  endif() # MSVC

  enable_testing()
  add_subdirectory(test)

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
