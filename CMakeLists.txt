cmake_minimum_required (VERSION 3.20)

set (LIB_NAME libcommon)

file(GLOB core_src CONFIGURE_DEPENDS
    "str.cpp"
    "datetime.cpp"
    "fss.cpp")

add_library(${LIB_NAME} STATIC ${core_src})