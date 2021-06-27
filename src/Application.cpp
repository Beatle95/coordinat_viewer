#include "Application.h"

CoordinatViewer::CoordinatViewer(const Arguments& arguments):
    Platform::Application{arguments, 
        Configuration{}
        .setTitle("Coordinat Viewer")
        .setSize(Vector2i{800, 800})
        .setWindowFlags(Magnum::Platform::Sdl2Application::Configuration::WindowFlag::Resizable)}
{
    mImgui.init(this, windowSize(), dpiScaling(), framebufferSize());

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::setClearColor(0x050505_rgbf);

    loadResources();
    objectsInit();

    /* Loop at 60 Hz max */
    setSwapInterval(1);
    setMinimalLoopPeriod(16);
}

void CoordinatViewer::drawEvent() 
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);
    if (std::chrono::steady_clock::now() - mLightCheckTimestamp > 20s){
        mLightCheckTimestamp = std::chrono::steady_clock::now();
        placeLightTimeBased();
    }
    mCamera->draw(mDrawables);    
    mImgui.draw();
    swapBuffers();
    redraw();
}

void CoordinatViewer::mousePressEvent(MouseEvent& event) 
{
    if(event.button() == MouseEvent::Button::Left)
        mPreviousPosition = event.position();
}

void CoordinatViewer::mouseReleaseEvent(MouseEvent& event) 
{
    if(event.button() == MouseEvent::Button::Left)
        mPreviousPosition = Vector2i();
}

void CoordinatViewer::mouseMoveEvent(MouseMoveEvent& event) 
{
    if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    auto currentPosition = event.position();
    float horAngle = (currentPosition.x() - mPreviousPosition.x()) * ROTATION_RATIO;
    float verAngle = (currentPosition.y() - mPreviousPosition.y()) * ROTATION_RATIO;

    auto newAngle = mCameraHorizontalAngle - horAngle;
    if (newAngle > 360.0f)
        newAngle -= 360.0f;
    if (newAngle < -360.0f)
        newAngle += 360.0f;
    mCameraHorizontalAngle = newAngle;

    newAngle = mCameraVerticalAngle - verAngle;
    if (newAngle > 10.0f && newAngle < 170.0f)
        mCameraVerticalAngle -= verAngle;

    mPreviousPosition = currentPosition;
    placeCamera();
}

void CoordinatViewer::mouseScrollEvent(MouseScrollEvent& event) 
{
    if(!event.offset().y()) return;
    const Float distance = mCameraObject.transformation().translation().z();
    float offset = (event.offset().y() < 0 ? SCROLL_DELTA : -SCROLL_DELTA);
    if (mCameraDistance + offset > MIN_ZOOM_IN && mCameraDistance + offset < MAX_ZOOM_OUT)
        mCameraDistance += offset;
    placeCamera();
}

void CoordinatViewer::viewportEvent(ViewportEvent& event) 
{    
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    mCamera->setViewport(event.windowSize());
    mImgui.viewportEvent(event);
}

void CoordinatViewer::placeCamera() 
{
    auto pos = fromPolarCoordinates(mCameraHorizontalAngle, mCameraVerticalAngle, mCameraDistance);
    mCameraObject.setTransformation(Matrix4::lookAt(pos, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 1.0f, 0.0f}));
}

void CoordinatViewer::placeLightTimeBased() 
{
    time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm utc_time = *gmtime(&tt);
    float minutesInCurDay = utc_time.tm_hour * utc_time.tm_min;
    mLightAngle = mapZeroBased(60 * 24, 360.0f, minutesInCurDay);
    mSceneLightObj->translate(fromPolarCoordinates(-mLightAngle, LIGHT_VERTICAL_ANGLE, LIGHT_DISTANCE));
}

void CoordinatViewer::objectsInit() 
{
    // set renderer and shader defaults
    mEartShader
        .setAmbientColor(0x505050_rgbf)
        .setDiffuseColor(0xb0b0b0_rgbf)
        .setSpecularColor(0x777777_rgbf)
        .setShininess(200.0f);
    
    /* Every scene needs a camera */
    mCameraObject
        .setParent(&mScene)
        .transform(Matrix4::lookAt(fromPolarCoordinates(mCameraHorizontalAngle, mCameraVerticalAngle, mCameraDistance), 
            Vector3{0.0f, 0.0f, 0.0f}, 
            Vector3{0.0f, 1.0f, 0.0f}));
    mCamera = new SceneGraph::Camera3D{mCameraObject};
    (*mCamera)
        .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    /* Base object, parent of all (for easy manipulation) */
    mManimpulator.setParent(&mScene);
    mSceneLightObj = new Object3D(&mManimpulator);   

    // place light on scene
    mLightCheckTimestamp = std::chrono::steady_clock::now();
    placeLightTimeBased();

    // initializing Eart object
    mEarthObj = new Object3D(&mManimpulator);
    new TexturedDrawable(*mEarthObj, mEartShader, mEarthMesh, mEarthTexture, mDrawables, mSceneLightObj);
    mEarthObj->rotateX(Magnum::Deg(-90.0f));
}

void CoordinatViewer::loadResources() 
{
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
}

Vector3 CoordinatViewer::fromPolarCoordinates(float phi, float theta, float r) 
{
    Vector3 pos;
    pos.x() = r * sin(Deg(theta)) * sin(Deg(phi));
    pos.y() = r * cos(Deg(theta));
    pos.z() = r * sin(Deg(theta)) * cos(Deg(phi));
    return pos;
}

float CoordinatViewer::mapZeroBased(float fromMax, float toMax, float value) 
{
    float result = 0.0f;
    result = value / fromMax * toMax;
    return result;
}
