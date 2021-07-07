cmake_minimum_required (VERSION 3.15)
project(coordinat_viewer VERSION 0.1)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 17)

file (
    GLOB_RECURSE SRC
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

find_package(Corrade REQUIRED Main)
find_package(
    Magnum REQUIRED
    GL 
    MeshTools
    Primitives
    Trade
    Shaders
    SceneGraph
    Sdl2Application
    AnySceneImporter
)
find_package(MagnumPlugins REQUIRED)
find_package(MagnumIntegration REQUIRED ImGui)

# find protobuf and make sources from .proto file
set (Protobuf_USE_STATIC_LIBS ON)
find_package (Protobuf REQUIRED)
protobuf_generate_cpp (
    PROTO_CPP
    PROTO_H
    ${CMAKE_CURRENT_SOURCE_DIR}/CoordinateViewer.proto
)

set (gRPC_USE_STATIC_LIBS ON)
find_package (gRPC REQUIRED)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)
function(PROTOBUF_GENERATE_GRPC_CPP SRCS HDRS)
    if(NOT ARGN)
        message(SEND_ERROR "Error: PROTOBUF_GENERATE_GRPC_CPP() called without any proto files")
        return()
    endif()

    if(PROTOBUF_GENERATE_CPP_APPEND_PATH) # This variable is common for all types of output.
        # Create an include path for each file specified
        foreach(FIL ${ARGN})
            get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
            get_filename_component(ABS_PATH ${ABS_FIL} PATH)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    else()
        set(_protobuf_include_path -I ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    if(DEFINED PROTOBUF_IMPORT_DIRS)
        foreach(DIR ${Protobuf_IMPORT_DIRS})
            get_filename_component(ABS_PATH ${DIR} ABSOLUTE)
            list(FIND _protobuf_include_path ${ABS_PATH} _contains_already)
            if(${_contains_already} EQUAL -1)
                list(APPEND _protobuf_include_path -I ${ABS_PATH})
            endif()
        endforeach()
    endif()

    set(${SRCS})
    set(${HDRS})
    foreach(FIL ${ARGN})
        get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
        get_filename_component(FIL_WE ${FIL} NAME_WE)

        list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc")
        list(APPEND ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h")
        add_custom_command(
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.cc"
                "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}.grpc.pb.h"
                COMMAND  ${Protobuf_PROTOC_EXECUTABLE}
                ARGS --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
                --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
                ${_protobuf_include_path} ${ABS_FIL}
                DEPENDS ${ABS_FIL} ${Protobuf_PROTOC_EXECUTABLE}
                COMMENT "Running gRPC C++ protocol buffer compiler on ${FIL}"
                VERBATIM)
    endforeach()

    set_source_files_properties(${${SRCS}} ${${HDRS}} PROPERTIES GENERATED TRUE)
    set(${SRCS} ${${SRCS}} PARENT_SCOPE)
    set(${HDRS} ${${HDRS}} PARENT_SCOPE)
endfunction()
PROTOBUF_GENERATE_GRPC_CPP(
    GRPC_CPP 
    GRPC_H 
    ${CMAKE_CURRENT_SOURCE_DIR}/CoordinateViewer.proto
)

add_executable (coordinat_viewer ${SRC} ${PROTO_CPP} ${GRPC_CPP})

target_include_directories(
    coordinat_viewer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MAGNUM_INCLUDE_DIR}
    ${CORRADE_INCLUDE_DIR}
    ${MAGNUMINTEGRATION_INCLUDE_DIR}
    imgui::imgui
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(
    coordinat_viewer PRIVATE
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::SceneGraph
    Magnum::Shaders
    Magnum::Primitives
    Magnum::Trade
    Magnum::AnySceneImporter
    MagnumIntegration::ImGui
    ${Protobuf_LIBRARIES}
    gRPC::grpc++_reflection
)

# copy resources to output directories
file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug/resources/
)
file(
    COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release/resources/
)
