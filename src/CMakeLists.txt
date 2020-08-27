find_package(Boost 1.73 REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Crc32c CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)

set(LIBRARY_LINK_TYPE STATIC)
if(NOT MSVC)
    set(LIBRARY_LINK_TYPE SHARED)
endif(NOT MSVC)

add_library(WBTree ${LIBRARY_LINK_TYPE} wbtree.cpp control_data.cpp posixio.cpp)
set_property(TARGET WBTree PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(WBTree PRIVATE ${Boost_LIBRARIES} fmt::fmt Crc32c::crc32c Microsoft.GSL::GSL)

target_include_directories(
    WBTree
    PRIVATE ${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS}
    INTERFACE ${PROJECT_SOURCE_DIR}/include)

add_warning_flags(WBTree)
add_sanitizer_flags(WBTree)
