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
    Trade
    Shaders
    SceneGraph
    Sdl2Application
    AnySceneImporter
)
find_package(MagnumPlugins REQUIRED)

add_executable (coordinat_viewer ${SRC})

target_include_directories(
    coordinat_viewer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MAGNUM_INCLUDE_DIR}
    ${CORRADE_INCLUDE_DIR}
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
    Magnum::Trade
    Magnum::AnySceneImporter
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
