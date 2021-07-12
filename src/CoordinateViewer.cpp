#include "CoordinateViewer.h"

CoordinateViewer::CoordinateViewer(const Arguments& arguments):
    Platform::Application{arguments, 
        Configuration{}
        .setTitle("Coordinat Viewer")
        .setSize(Vector2i{800, 800})
        .setWindowFlags(Magnum::Platform::Sdl2Application::Configuration::WindowFlag::Resizable | 
        Magnum::Platform::Sdl2Application::Configuration::WindowFlag::Maximized)}
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

void CoordinateViewer::objectsInit() 
{
    // set shader defaults
    mEarthShader
        .setAmbientColor(0x505050_rgbf)
        .setDiffuseColor(0xb0b0b0_rgbf)
        .setSpecularColor(0x777777_rgbf)
        .setShininess(200.0f);
    mColorShader
        .setDiffuseColor(0x555555_rgbf)
        .setSpecularColor(0x000000_rgbf);
    
    // every scene needs a camera
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

    // base object, parent of all (for easy manipulation)
    mManimpulator.setParent(&mScene);
    mSceneLightObj = new Object3D(&mManimpulator);   

    // place light on scene
    mMainLightTimestamp = std::chrono::steady_clock::now();
    placeLightTimeBased(std::chrono::system_clock::now());

    // initializing Eart object
    mEarthObj = new Object3D(&mManimpulator);
    new TexturedDrawable(*mEarthObj, mEarthShader, mEarthMesh, mEarthTexture, mTexturedDrawables, mSceneLightObj);
    mEarthObj->rotateX(-Rad(Constants::pi() / 2));
}

void CoordinateViewer::loadResources() 
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
    auto earthMeshData = assimpImporter->mesh(0);
    mEarthMesh = MeshTools::compile(*earthMeshData);

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

    if (!assimpImporter->openFile("./resources/Point.fbx")) {
        Error{} << "Cannot load Point.fbx";
        exit(LOADING_ERROR);
    }
    if (assimpImporter->meshCount() < 1) {
        Error{} << "Cannot find meshes in Point.fbx";
        exit(LOADING_ERROR);
    }
    auto pointMeshData = assimpImporter->mesh(0);
    mPointMesh = MeshTools::compile(*pointMeshData);
}

void CoordinateViewer::drawEvent() 
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    if (std::chrono::steady_clock::now() - mMainLightTimestamp > 20s){
        mMainLightTimestamp = std::chrono::steady_clock::now();
        placeLightTimeBased(std::chrono::system_clock::now());
    }

    mCamera->draw(mColoredDrawables); 
    mCamera->draw(mTexturedDrawables);   
    mImgui.draw();
    swapBuffers();
    redraw();
    // now we can do some long time operations cause we swaped buffers just few tics ago
    
}

void CoordinateViewer::keyPressEvent(KeyEvent& event) 
{
    if (mImgui.keyPressEvent(event)) return;
}

void CoordinateViewer::keyReleaseEvent(KeyEvent& event) 
{
    if (mImgui.keyReleaseEvent(event)) return;
}

void CoordinateViewer::mousePressEvent(MouseEvent& event) 
{
    if (mImgui.mousePressEvent(event)) return;
    if (event.button() == MouseEvent::Button::Left)
        mPreviousPosition = event.position();
}

void CoordinateViewer::mouseReleaseEvent(MouseEvent& event) 
{
    if (mImgui.mouseReleaseEvent(event)) return;
    if (event.button() == MouseEvent::Button::Left)
        mPreviousPosition = Vector2i();
}

void CoordinateViewer::mouseMoveEvent(MouseMoveEvent& event) 
{
    if (mImgui.mouseMoveEvent(event)) return;
    if (!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    auto currentPosition = event.position();
    float horAngle = (currentPosition.x() - mPreviousPosition.x()) * ROTATION_RATIO;
    float verAngle = (currentPosition.y() - mPreviousPosition.y()) * ROTATION_RATIO;

    auto newAngle = mCameraHorizontalAngle - horAngle;
    if (newAngle > Constants::pi() * 2)
        newAngle -= Constants::pi() * 2;
    if (newAngle < -Constants::pi() * 2)
        newAngle += Constants::pi() * 2;
    mCameraHorizontalAngle = newAngle;

    newAngle = mCameraVerticalAngle - verAngle;
    if (newAngle > CAMERA_VERT_MAX && newAngle < CAMERA_VERT_MIN)
        mCameraVerticalAngle -= verAngle;

    mPreviousPosition = currentPosition;
    placeCamera();
}

void CoordinateViewer::mouseScrollEvent(MouseScrollEvent& event) 
{
    if (mImgui.mouseScrollEvent(event)) {
        event.setAccepted();
        return;
    }
    if (!event.offset().y()) return;
    const Float distance = mCameraObject.transformation().translation().z();
    float offset = (event.offset().y() < 0 ? SCROLL_DELTA : -SCROLL_DELTA);
    if (mCameraDistance + offset > MIN_ZOOM_IN && mCameraDistance + offset < MAX_ZOOM_OUT)
        mCameraDistance += offset;
    placeCamera();
}

void CoordinateViewer::textInputEvent(TextInputEvent& event) 
{
    if (mImgui.textInputEvent(event)) return;
}

void CoordinateViewer::viewportEvent(ViewportEvent& event) 
{    
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    mCamera->setViewport(event.windowSize());
    mImgui.viewportEvent(event);
}

void CoordinateViewer::placeCamera() 
{
    auto pos = fromPolarCoordinates(mCameraHorizontalAngle, mCameraVerticalAngle, mCameraDistance);
    mCameraObject.setTransformation(Matrix4::lookAt(pos, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 1.0f, 0.0f}));
}

void CoordinateViewer::placeLightTimeBased(const std::chrono::system_clock::time_point& time_point) 
{
    time_t tt = std::chrono::system_clock::to_time_t(time_point);
    tm utc_time = *gmtime(&tt);
    float minutesInCurDay = utc_time.tm_hour * 60 + utc_time.tm_min;
    auto lightAngle = mapZeroBased(60 * 24, Constants::pi() * 2, minutesInCurDay) - Constants::pi();
    // TODO: check in the future
    mSceneLightObj->translate(fromPolarCoordinates(-lightAngle, LIGHT_VERTICAL_ANGLE, LIGHT_DISTANCE));
}

Vector3 CoordinateViewer::fromPolarCoordinates(const float phi_rad, const float theta_rad, const float r) 
{
    Vector3 pos;
    pos.x() = r * sin(theta_rad) * sin(phi_rad);
    pos.y() = r * cos(theta_rad);
    pos.z() = r * sin(theta_rad) * cos(phi_rad);
    return pos;
}

float CoordinateViewer::mapZeroBased(const float fromMax, const float toMax, const float value) 
{
    float result = 0.0f;
    result = value / fromMax * toMax;
    return result;
}
