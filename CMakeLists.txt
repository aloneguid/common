cmake_minimum_required (VERSION 3.20)

set (LIB_NAME libcommon)

find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

#file(GLOB core_src CONFIGURE_DEPENDS
#    "*.cpp"
#    "config/*.cpp"
#    "ext/*.cpp")

#if (WIN32)
#    file(GLOB core_src CONFIGURE_DEPENDS ${core_src} "win32/*.cpp")
#endif()

file(GLOB core_src CONFIGURE_DEPENDS
    "str.cpp"
    "datetime.cpp"
    "fss.cpp")

add_library(${LIB_NAME} STATIC ${core_src})

target_include_directories(${LIB_NAME} PRIVATE ${SIMPLEINI_INCLUDE_DIRS})
