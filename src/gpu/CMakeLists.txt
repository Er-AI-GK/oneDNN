#===============================================================================
# Copyright 2020-2023 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#===============================================================================

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

add_definitions_with_host_compiler(-DNGEN_CPP11)
add_definitions_with_host_compiler(-DNGEN_SAFE)
add_definitions_with_host_compiler(-DNGEN_NEO_INTERFACE)
add_definitions_with_host_compiler(-DNGEN_NO_OP_NAMES)
add_definitions_with_host_compiler(-DNGEN_WINDOWS_COMPAT)

set(OBJ_LIB ${LIB_PACKAGE_NAME}_gpu)
add_library(${OBJ_LIB} OBJECT ${SOURCES})
set_property(GLOBAL APPEND PROPERTY DNNL_LIB_DEPS
    $<TARGET_OBJECTS:${OBJ_LIB}>)

add_subdirectory(compute)
add_subdirectory(jit)
add_subdirectory(ocl)

if(DNNL_WITH_SYCL)
    add_subdirectory(sycl)
endif()

if(DNNL_SYCL_CUDA)
    add_subdirectory(nvidia)
    # Pass ${LIB_PACKAGE_NAME}_INTERFACE to upper level for proper linking
    set(${LIB_PACKAGE_NAME}_INTERFACE "${${LIB_PACKAGE_NAME}_INTERFACE}" PARENT_SCOPE)
endif()
if(DNNL_SYCL_HIP)
    add_subdirectory(amd)
    set(${LIB_PACKAGE_NAME}_INTERFACE "${${LIB_PACKAGE_NAME}_INTERFACE}" PARENT_SCOPE)
endif()
