# Set cmake minimum required version
cmake_minimum_required(VERSION 3.10)

# Set project name
project(xRayGeant4)

# Set default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# require C++11-capable compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
    set(OLD_CXX true)
  endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find ROOT package
find_package(ROOT)
find_package(ROOT REQUIRED COMPONENTS Core Tree TreePlayer Gpad Hist Physics)
find_package(ROOT REQUIRED COMPONENTS RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread)

# Find GEANT4 visualizing driver
option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

# set C++ standard ensuring compatibility with ROOT
set(CMAKE_CXX_STANDARD 17)
if(ROOT_cxx17_FOUND)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(ROOT_cxx14_FOUND)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(ROOT_cxx11_FOUND)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
message(STATUS "Using C++${CMAKE_CXX_STANDARD}")

# Set compiler options
add_compile_options("-Wall")
add_compile_options("-Wpedantic")
add_compile_options("-Wno-unused-variable")
add_compile_options("-Wunused-variable")
add_compile_options("-Wno-uninitialized")
add_compile_options("-Wno-attributes")

set(DATA_DIR "${CMAKE_BINARY_DIR}/Data" CACHE PATH "Data directory")	

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wno-c11-extensions)
endif()

# Set for doxygen for documentation
option(BUILD_DOC "Build documentation" OFF)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  set(BUILDINSOURCE true)
endif()

# Print the path of source directory
message(STATUS "In Project Directory: ${PROJECT_SOURCE_DIR}")
# Print path of include directories of external package
message(STATUS "ROOT Include Directory: ${ROOT_INCLUDE_DIRS}")
message(STATUS "Geant4 Include Directory: ${Geant4_INCLUDE_DIR}")

# Find header files and source files and store it as variable
file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_SOURCE_DIR} "inc/*.hpp")
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp")


# Add header and source files to library
add_library(myProject SHARED ${SOURCES})

# Find include files and link to source files
foreach(HEADER_PATH ${HEADERS})
    get_filename_component(DIR ${HEADER_PATH} DIRECTORY)
    target_include_directories(myProject PRIVATE ${DIR})
endforeach()

# Add ROOT and Geant4 include files
include(${Geant4_USE_FILE})
include_directories(${Geant4_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})

# Add exe subdirectory for executable cpp files
add_subdirectory(exe)

# Make Data directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Data)

# file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})