# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(SaplingTacticsTests LANGUAGES CXX)

# ---- Tests ----

add_executable(SaplingTactics_test source/SaplingTactics_test.cpp)
target_link_libraries(SaplingTactics_test PRIVATE SaplingTactics_lib)
target_compile_features(SaplingTactics_test PRIVATE cxx_std_20)

add_test(NAME SaplingTactics_test COMMAND SaplingTactics_test)

# ---- End-of-file commands ----

add_folders(Test)
