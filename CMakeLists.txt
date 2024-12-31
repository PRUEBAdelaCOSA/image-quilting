CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(IMAGE-QUILTING)

# Add the subdirectory for agz-utils and set definitions
ADD_SUBDIRECTORY(lib/my-utils)
TARGET_COMPILE_DEFINITIONS(MyUtils PUBLIC AGZ_UTILS_SSE)

# Recursively find all source and header files except main files
FILE(GLOB_RECURSE COMMON_SRC
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/include/*.h"
)

# Remove the main files from COMMON_SRC
LIST(REMOVE_ITEM COMMON_SRC "${PROJECT_SOURCE_DIR}/src/main.cpp")
LIST(REMOVE_ITEM COMMON_SRC "${PROJECT_SOURCE_DIR}/src/main2.cpp")

# Create the first executable target with main.cpp
ADD_EXECUTABLE(ImageQuilting_main ${COMMON_SRC} src/main.cpp)

# Create the second executable target with main2.cpp
ADD_EXECUTABLE(ImageQuilting_main2 ${COMMON_SRC} src/main2.cpp)

# Set the C++ standard for both targets
SET_PROPERTY(TARGET ImageQuilting_main PROPERTY CXX_STANDARD 17)
SET_PROPERTY(TARGET ImageQuilting_main2 PROPERTY CXX_STANDARD 17)
SET_PROPERTY(TARGET ImageQuilting_main PROPERTY CXX_STANDARD_REQUIRED ON)
SET_PROPERTY(TARGET ImageQuilting_main2 PROPERTY CXX_STANDARD_REQUIRED ON)

# Include directories for both targets
TARGET_INCLUDE_DIRECTORIES(ImageQuilting_main PUBLIC
        "${PROJECT_SOURCE_DIR}/lib/cxxopts"
        "${PROJECT_SOURCE_DIR}/lib/my-utils/include"
        "${PROJECT_SOURCE_DIR}/include"
)


TARGET_INCLUDE_DIRECTORIES(ImageQuilting_main2 PUBLIC
        "${PROJECT_SOURCE_DIR}/lib/cxxopts"
        "${PROJECT_SOURCE_DIR}/lib/my-utils"
        "${PROJECT_SOURCE_DIR}/include"
)

# Link libraries for both targets
TARGET_LINK_LIBRARIES(ImageQuilting_main PUBLIC MyUtils)
TARGET_LINK_LIBRARIES(ImageQuilting_main2 PUBLIC MyUtils)
