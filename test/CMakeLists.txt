option(ENABLE_COVERAGE_ANALYSIS "Enable Coverage of tests. Works best with Debug build types" OFF)

find_package(doctest CONFIG REQUIRED)

add_executable(WBTreeTest testbase.cpp testwbtree.cpp)
target_include_directories(WBTreeTest PRIVATE ${WBTree_INCLUDE_DIRS})
target_link_libraries(WBTreeTest PRIVATE WBTree doctest::doctest)

enable_testing()
add_test(NAME WBTreeTest COMMAND WBTreeTest) # print test duration

if(ENABLE_COVERAGE_ANALYSIS)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage_lcov(NAME coverage EXECUTABLE WBTreeTest DEPENDENCIES WBTreeTest)
endif()

add_warning_flags(WBTreeTest)
add_sanitizer_flags(WBTreeTest)
