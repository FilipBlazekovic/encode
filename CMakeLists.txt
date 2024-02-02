cmake_minimum_required(VERSION 3.22.1)
project(encode C)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 17)

set(HEADERS_DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
set(LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR})

set(CMAKE_C_FLAGS "-O3 -fmessage-length=0 -Wall -Werror -Wmissing-prototypes -Wstrict-prototypes -Wpointer-arith -Wwrite-strings")
set(SOURCE_ROOT "src")

add_library(encode STATIC ${SOURCE_ROOT}/encode.h ${SOURCE_ROOT}/encode.c)
set_target_properties(encode PROPERTIES PUBLIC_HEADER ${SOURCE_ROOT}/encode.h)

target_link_libraries(encode m)

install(
        TARGETS encode
        PUBLIC_HEADER DESTINATION ${HEADERS_DESTINATION}
        LIBRARY DESTINATION ${LIBRARY_DESTINATION}
        ARCHIVE DESTINATION ${LIBRARY_DESTINATION}
)
