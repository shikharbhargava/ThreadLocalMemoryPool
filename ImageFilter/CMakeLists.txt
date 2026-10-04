file(GLOB_RECURSE IMAGE_FILTER_SOURCES src/*.cpp)

# Find OpenCV package
find_package(OpenCV REQUIRED)

add_library(ImageFilter STATIC ${IMAGE_FILTER_SOURCES})

target_include_directories(ImageFilter PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ImageFilter utils ${OpenCV_LIBS} json)