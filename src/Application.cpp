#include "Application.h"

CoordinatViewer::CoordinatViewer(const Arguments& arguments):
    Platform::Application{arguments, 
        Configuration{}
        .setTitle("Coordinat Viewer")
        .setSize(Vector2i{800, 800})
        .setWindowFlags(Magnum::Platform::Sdl2Application::Configuration::WindowFlag::Resizable)}
{
    /* Every scene needs a camera */
    mCameraObject
        .setParent(&mScene)
        .translate(Vector3{0.0f, 0.0f, 20.0f});
    mCamera = new SceneGraph::Camera3D{mCameraObject};
    (*mCamera)
        .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    /* Base object, parent of all (for easy manipulation) */
    mManimpulator.setParent(&mScene);
    mSceneLightObj = new Object3D(&mManimpulator);
    mSceneLightObj->translate(Vector3{0.0f, 0.0f, 30.0f});

    // set renderer and shader defaults
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    mTextureShader.setAmbientColor(0x111111_rgbf)
        .setSpecularColor(0x777777_rgbf)
        .setShininess(80.0f);

    PluginManager::Manager<Trade::AbstractImporter> manager;
    auto assimpImporter = manager.loadAndInstantiate("AssimpImporter");
    if (!assimpImporter->openFile("./resources/Earth.fbx")) {
        Error{} << "Cannot load Earth.fbx";
        exit(LOADING_ERROR);
    }
    auto pngImporter = manager.loadAndInstantiate("PngImporter");
    if (!pngImporter->openFile("./resources/Diffuse.png")) {
        Error{} << "Cannot load Diffuse.png";
        exit(LOADING_ERROR);
    }

    if (assimpImporter->meshCount() < 1) {
        Error{} << "Cannot find meshes in Earth.fbx";
        exit(LOADING_ERROR);
    }
    auto meshData = assimpImporter->mesh(0);
    mEarthMesh = MeshTools::compile(*meshData);

    if (pngImporter->image2DCount() < 1) {
        Error{} << "Cannot find images in Diffuse.png";
        exit(LOADING_ERROR);
    }
    auto image = pngImporter->image2D(0);
    
    mEarthTexture.setMagnificationFilter(GL::SamplerFilter::Linear)
        .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
        .setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
        .setStorage(1, GL::textureFormat(image->format()), image->size())
        .setSubImage(0, {}, *image);

    auto* object = new Object3D(&mManimpulator);
    new TexturedDrawable(*object, mTextureShader, mEarthMesh, mEarthTexture, mDrawables, mSceneLightObj);
}

void CoordinatViewer::drawEvent() 
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);
    mCamera->draw(mDrawables);
    swapBuffers();
}

void CoordinatViewer::mousePressEvent(MouseEvent& event) 
{
    if(event.button() != MouseEvent::Button::Left)
        mPreviousPosition = positionOnSphere(event.position());
}

void CoordinatViewer::mouseReleaseEvent(MouseEvent& event) 
{
    if(event.button() != MouseEvent::Button::Left)
        mPreviousPosition = Vector3();
}

void CoordinatViewer::mouseMoveEvent(MouseMoveEvent& event) 
{
    if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    const Vector3 currentPosition = positionOnSphere(event.position());
    const Vector3 axis = Math::cross(mPreviousPosition, currentPosition);

    //if(mPreviousPosition.length() < 0.001f || axis.length() < 0.001f) return;

    mManimpulator.rotate(Math::angle(mPreviousPosition, currentPosition), axis.normalized());
    mPreviousPosition = currentPosition;

    redraw();
}

void CoordinatViewer::viewportEvent(ViewportEvent& event) 
{    
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    mCamera->setViewport(event.windowSize());
}

Vector3 CoordinatViewer::positionOnSphere(const Vector2i& position) 
{
    const Vector2 positionNormalized = Vector2{position}/Vector2{mCamera->viewport()} - Vector2{0.5f};
    const Float length = positionNormalized.length();
    const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
    return (result*Vector3::yScale(-1.0f)).normalized();
}

void TexturedDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) 
{
    auto lightPos = mSceneLightObj->absoluteTransformation() * Vector4{0.0f, 10.0f, 10.0f, 0.0f};
    mShader
    .setLightPositions({
        lightPos
    })
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.normalMatrix())
    .setProjectionMatrix(camera.projectionMatrix())
    .bindDiffuseTexture(mTexture)
    .draw(mMesh);
}
