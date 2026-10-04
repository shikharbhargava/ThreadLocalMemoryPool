cmake_minimum_required(VERSION 3.12)

# Compiler flags first
include(cmake/CompilerFlags.cmake)

project(threading VERSION 0.1.0 LANGUAGES CXX)

# Set default installation prefix to build/install/ContainerRegistry
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install/threading" CACHE PATH "Installation prefix" FORCE)
endif()

# Enable testing
# enable_testing()

# Add dependencies
add_subdirectory(deps)
add_subdirectory(ImageFilter)

# add_subdirectory(tests)

add_executable(threading main/main.cpp)
target_link_libraries(threading PRIVATE ArgParser ImageFilter)

# Install targets
install(TARGETS threading
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
