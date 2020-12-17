cmake_minimum_required(VERSION 3.5.1 FATAL_ERROR)

#============================================================================
# Initialize the project
#============================================================================
project(test_bug VERSION 1.0.0)

#============================================================================
# Find ignition-cmake
#============================================================================
# If you get an error at this line, you need to install ignition-cmake
find_package(ignition-cmake2 2.3 REQUIRED)

ign_find_package(ignition-common3 VERSION 3.5 REQUIRED)

#============================================================================
# Configure the project
#============================================================================
ign_configure_project()
ign_configure_build(QUIT_IF_BUILD_ERRORS)
