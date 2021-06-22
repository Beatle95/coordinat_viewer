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
find_package(MagnumIntegration REQUIRED ImGui)
find_package(imgui REQUIRED)


add_executable (coordinat_viewer ${SRC})

target_include_directories(
    coordinat_viewer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MAGNUM_INCLUDE_DIR}
    ${CORRADE_INCLUDE_DIR}
    ${MAGNUMINTEGRATION_INCLUDE_DIR}
    imgui::imgui
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
    ${MAGNUMINTEGRATION_IMGUI_LIBRARY_RELEASE}          # some problems with imgui, so adding by hand
    imgui::imgui
)

# copy runtime of the manually added libraries
file(
    COPY d:/lib/vcpkg/installed/x64-windows/bin/MagnumImGuiIntegration.dll
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug/
)
file(
    COPY d:/lib/vcpkg/installed/x64-windows/bin/MagnumImGuiIntegration.dll
    DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release/
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
