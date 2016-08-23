cmake_minimum_required (VERSION 2.8)

# mandatory definitions
set(MHE_DIR ${CMAKE_SOURCE_DIR}/../../..)
set(THIRD_PARTY_DIR ${MHE_DIR}/3rdparty)
set(SOURCE_ROOT ${CMAKE_SOURCE_DIR}/../src ${THIRD_PARTY_DIR}/pugixml)
set(INCLUDE_ROOT ${SOOURCE_ROOT})

set(CMAKE_MODULES_DIR ${MHE_DIR}/build/cmake_modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_MODULES_DIR})

include(mhe_based_project)

set(USE_ASSIMP_LIB TRUE)
mhe_based_project("meshconverter")