cmake_minimum_required(VERSION 3.14)
project(trompeloeil)

include(GNUInstallDirs)
include(ExternalProject)
include(CMakePackageConfigHelpers)
include(CheckCXXCompilerFlag)

option(TROMPELOEIL_BUILD_TESTS "Build self test programs" off)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/trompeloeil/trompeloeil-config-version.cmake"
  VERSION 45
  COMPATIBILITY AnyNewerVersion
  ARCH_INDEPENDENT)

add_library(trompeloeil INTERFACE)
add_library(trompeloeil::trompeloeil ALIAS trompeloeil)

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

option(TROMPELOEIL_INSTALL_TARGETS "Sets whether trompeloeil should be installed" ${MASTER_PROJECT})
option(TROMPELOEIL_INSTALL_DOCS "Install documentation" ${TROMPELOEIL_INSTALL_TARGETS})

if (MASTER_PROJECT AND TROMPELOEIL_BUILD_TESTS)
  add_subdirectory(test)
endif()

if (TROMPELOEIL_INSTALL_TARGETS)
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
  NAMESPACE
    trompeloeil::
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
endif(TROMPELOEIL_INSTALL_TARGETS)

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
