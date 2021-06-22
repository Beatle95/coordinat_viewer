#include "Application.h"

CoordinatViewer::CoordinatViewer(const Arguments& arguments):
    Platform::Application{arguments, 
        Configuration{}
        .setTitle("Coordinat Viewer")
        .setSize(Vector2i{800, 800})
        .setWindowFlags(Magnum::Platform::Sdl2Application::Configuration::WindowFlag::Resizable)}
{
    {
        ImGui::CreateContext();
        mImgui = ImGuiIntegration::Context(*ImGui::GetCurrentContext(),
            Vector2{windowSize()}/dpiScaling(), windowSize(), framebufferSize());

        /* Set up proper blending to be used by ImGui. There's a great chance
        you'll need this exact behavior for the rest of your scene. If not, set
        this only for the drawFrame() call. */
        GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
            GL::Renderer::BlendEquation::Add);
        GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
            GL::Renderer::BlendFunction::OneMinusSourceAlpha);

        #if !defined(MAGNUM_TARGET_WEBGL) && !defined(CORRADE_TARGET_ANDROID)
        /* Have some sane speed, please */
        setMinimalLoopPeriod(16);
        #endif
    }
    
    /* Every scene needs a camera */
    Vector3 cameraPos{0.0f, 0.0f, mCameraDistance};
    mCameraObject
        .setParent(&mScene)
        .transform(Matrix4::lookAt(cameraPos, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 1.0f, 0.0f}));
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
    GL::Renderer::setClearColor(0x050505_rgbf);
    mTextureShader
        .setAmbientColor(0x111111_rgbf)
        .setDiffuseColor(0x999999_rgbf)
        .setSpecularColor(0x333333_rgbf)
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

    mEarthObj = new Object3D(&mManimpulator);
    new TexturedDrawable(*mEarthObj, mTextureShader, mEarthMesh, mEarthTexture, mDrawables, mSceneLightObj);
    mEarthObj->rotateX(Magnum::Deg(-90.0f));

    /* Loop at 60 Hz max */
    setSwapInterval(1);
    setMinimalLoopPeriod(16);
}

void CoordinatViewer::drawEvent() 
{
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

    mImgui.newFrame();

    mCamera->draw(mDrawables);
    
    ImGui::Begin("Window", nullptr);
    /* 1. Show a simple window.
       Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appear in
       a window called "Debug" automatically */
    {
        ImGui::Text("Hello, world!");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0/Double(ImGui::GetIO().Framerate), Double(ImGui::GetIO().Framerate));
    }
    ImGui::End();
    /* Update application cursor */
    mImgui.updateApplicationCursor(*this);
    /* Set appropriate states. If you only draw ImGui, it is sufficient to
       just enable blending and scissor test in the constructor. */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    mImgui.drawFrame();
    /* Reset state. Only needed if you want to draw something else with
       different state after. */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    swapBuffers();
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
    redraw();
}

void CoordinatViewer::mouseScrollEvent(MouseScrollEvent& event) 
{
    if(!event.offset().y()) return;
    /* Distance to origin */
    const Float distance = mCameraObject.transformation().translation().z();
    /* Move 15% of the distance back or forward */
    float offset = (event.offset().y() < 0 ? SCROLL_DELTA : -SCROLL_DELTA);
    if (mCameraDistance + offset > MIN_ZOOM_IN || mCameraDistance + offset < MAX_ZOOM_OUT)
        mCameraDistance += offset;
    placeCamera();
    redraw();
}

void CoordinatViewer::viewportEvent(ViewportEvent& event) 
{    
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    mCamera->setViewport(event.windowSize());
    mImgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
        event.windowSize(), event.framebufferSize());
}

void CoordinatViewer::placeCamera() 
{
    Vector3 cameraPos;
    cameraPos.x() = mCameraDistance * sin(Deg(mCameraVerticalAngle)) * sin(Deg(mCameraHorizontalAngle));
    cameraPos.y() = mCameraDistance * cos(Deg(mCameraVerticalAngle));
    cameraPos.z() = mCameraDistance * sin(Deg(mCameraVerticalAngle)) * cos(Deg(mCameraHorizontalAngle));
    mCameraObject.setTransformation(Matrix4::lookAt(cameraPos, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 1.0f, 0.0f}));
}
