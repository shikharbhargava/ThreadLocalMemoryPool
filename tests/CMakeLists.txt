cmake_minimum_required(VERSION 3.12)

# Enable testing
enable_testing()

# Create the test executable with all categorized test files
add_executable(container_tests
  type_detection.cpp
  vector_containers.cpp
  map_containers.cpp
  adapter_containers.cpp
  nested_structures.cpp
  empty_containers.cpp
  edge_cases.cpp
  custom_compute_functions.cpp
  registry_behavior.cpp
  custom_structures.cpp
)

# Include directories for common.hpp
target_include_directories(container_tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against ContainerRegistry and Google Test (from deps)
target_link_libraries(container_tests
  PRIVATE
    ContainerRegistry
    GTest::gtest_main
)

# Enable /FS flag for MSVC to allow safe parallel compilation with shared PDB
if(MSVC)
  target_compile_options(container_tests PRIVATE /FS)
endif()

# Register the test with CTest
add_test(NAME ContainerRegistryTests COMMAND container_tests)

# Install test executable
install(TARGETS container_tests
  RUNTIME DESTINATION bin
)
