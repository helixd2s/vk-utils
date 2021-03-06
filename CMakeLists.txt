cmake_minimum_required(VERSION 3.20)
project(vku VERSION 0.1.0)
set(PROJECT_NAME vku-test)

#
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

# 
IF (WIN32)
  IF(NOT DEFINED VCPKG_PATH OR NOT DEFINED ${VCPKG_PATH})
    set (VCPKG_PATH "C:/vcpkg/")
  ENDIF()

  # trying load GLFW3 from VCPKG in Windows
  IF(NOT DEFINED CMAKE_TOOLCHAIN_FILE OR NOT DEFINED ${CMAKE_TOOLCHAIN_FILE})
    set (CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake")
  ENDIF()

  IF(NOT DEFINED CMAKE_MODULE_PATH OR NOT DEFINED ${CMAKE_MODULE_PATH})
    set (CMAKE_MODULE_PATH "${VCPKG_PATH}/installed/x64-windows")
  ENDIF()
  
  IF(NOT DEFINED glfw3_DIR OR NOT DEFINED ${glfw3_DIR})
    set (glfw3_DIR "${CMAKE_MODULE_PATH}/share/glfw3")
  ENDIF()
ENDIF()

# 
find_package(Vulkan REQUIRED FATAL_ERROR)

# 
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(_src_root_path "${PROJECT_SOURCE_DIR}/")
file(GLOB_RECURSE _source_list 
  LIST_DIRECTORIES false
  "${_src_root_path}/cmake_pch.hxx"
  "${_src_root_path}/cmake_pch.cxx"
  "${_src_root_path}/chain.hpp"
  "${_src_root_path}/sizes.h"
  "${_src_root_path}/cpp21.hpp"
  "${_src_root_path}/cpp21/*.cpp"
  "${_src_root_path}/cpp21/*.hpp"
  "${_src_root_path}/test/*.cpp"
)

# 
add_executable(${PROJECT_NAME} ${_source_list})
target_compile_definitions(${PROJECT_NAME} PUBLIC USE_CMAKE_PCH)
target_include_directories(${PROJECT_NAME} PUBLIC 
  ${PROJECT_SOURCE_DIR}/
  ${PROJECT_SOURCE_DIR}/*/include 
  ${PROJECT_SOURCE_DIR}/interval-tree/include 
  ${PROJECT_SOURCE_DIR}/generator/include 
  ${PROJECT_SOURCE_DIR}/glm/glm
  ${PROJECT_SOURCE_DIR}/glm
  ${PROJECT_SOURCE_DIR}/submodules/include 
  ${Vulkan_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${Vulkan_LIBRARIES})
target_link_directories(${PROJECT_NAME} PUBLIC 
)

#
set (HSOURCES "${PROJECT_SOURCE_DIR}/cpp21/core.hpp")

#
ADD_MSVC_PRECOMPILED_HEADER(
  "${PROJECT_SOURCE_DIR}/cmake_pch.hpp" 
  "${PROJECT_SOURCE_DIR}/cmake_pch.cpp" 
  HSOURCES)

# 
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w dupbuild=warn -std=c++2b /d1FastFail /std:c17 /std:c++latest /bigobj /MP /EHa /fsanitize=address /Ot /Od /Oy /Ob0 /GT /GL /GR")
    set(CMAKE_EXE_LINKER_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /MP /MAP /incremental /debug:fastlink /experimental /errorReport")
endif()

# 
foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
