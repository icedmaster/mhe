cmake_minimum_required (VERSION 2.8)

set(MHE_DIR ${CMAKE_SOURCE_DIR}/../../..)
set(SOURCE_ROOT ${CMAKE_SOURCE_DIR}/..)
set(INCLUDE_ROOT ${CMAKE_SOURCE_DIR}/..)
set(ADDITIONAL_SOURCES "")

set(CMAKE_MODULES_DIR ${MHE_DIR}/build/cmake_modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_MODULES_DIR})
include(mhe_based_project)
mhe_based_project("raytracing")
