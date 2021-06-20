#include "Application.h"

CoordinatViewer::CoordinatViewer(const Arguments& arguments):
    Platform::Application{arguments, Configuration{}
        .setTitle("Magnum Primitives Example")}
{
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    PluginManager::Manager<Trade::AbstractImporter> manager;
    auto assimpImporter = manager.loadAndInstantiate("AssimpImporter");
    if (!assimpImporter->openFile("./resources/Earth.fbx")) {
        Error{} << "Cannot load Earth.fbx";
        exit(LOADING_ERROR);
    }

    if (assimpImporter->meshCount() < 1) {
        Error{} << "Cannot find meshes in Earth.fbx";
        exit(LOADING_ERROR);
    }

    auto mesh_data = assimpImporter->mesh(0);
    mEarthMesh = MeshTools::compile(*mesh_data);

    mTranssformation =
        Matrix4::rotationX(30.0_degf)*Matrix4::rotationY(40.0_degf);
    mProjection =
        Matrix4::perspectiveProjection(
            35.0_degf, Vector2{windowSize()}.aspectRatio(), 0.01f, 100.0f)*
        Matrix4::translation(Vector3::zAxis(-30.0f));
    mColor = Color3::fromHsv({35.0_degf, 1.0f, 1.0f});
}

void CoordinatViewer::drawEvent() {
    GL::defaultFramebuffer.clear(
        GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

    mEarthShader.setLightPositions({{1.4f, 1.0f, 0.75f, 0.0f}})
        .setDiffuseColor(mColor)
        .setAmbientColor(Color3::fromHsv({mColor.hue(), 1.0f, 0.3f}))
        .setTransformationMatrix(mTranssformation)
        .setNormalMatrix(mTranssformation.normalMatrix())
        .setProjectionMatrix(mProjection)
        .draw(mEarthMesh);

    swapBuffers();
}

void CoordinatViewer::mousePressEvent(MouseEvent& event) {
    if(event.button() != MouseEvent::Button::Left) return;

    event.setAccepted();
}

void CoordinatViewer::mouseReleaseEvent(MouseEvent& event) {
    mColor = Color3::fromHsv({mColor.hue() + 50.0_degf, 1.0f, 1.0f});

    event.setAccepted();
    redraw();
}

void CoordinatViewer::mouseMoveEvent(MouseMoveEvent& event) {
    if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    Vector2 delta = 3.0f*Vector2{event.relativePosition()}/Vector2{windowSize()};

    mTranssformation =
        Matrix4::rotationX(Rad{delta.y()})*
        mTranssformation*
        Matrix4::rotationY(Rad{delta.x()});

    event.setAccepted();
    redraw();
}
