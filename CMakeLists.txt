cmake_minimum_required(VERSION 3.0.0)
project(trompeloeil)

include(GNUInstallDirs)


add_library(trompeloeil INTERFACE)

target_sources(trompeloeil INTERFACE trompeloeil.hpp)


include(GNUInstallDirs)

# if someone knows how to do this with header only libs, pls submitt a pull request

#target_include_directories(trompeloeil INTERFACE
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
#    $<INSTALL_INTERFACE:include/>  # <prefix>/include
#)

#install(EXPORT trompeloeilConfig DESTINATION share/trompeloeil/cmake)

#export(TARGETS trompeloeil FILE trompeloeilConfig.cmake)


install(FILES trompeloeil.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES LICENSE_1_0.txt
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

install (DIRECTORY docs 
         DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

