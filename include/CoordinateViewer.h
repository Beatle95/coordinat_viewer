#ifndef COORDINATE_VIEWER
#define COORDINATE_VIEWER
#include <Magnum/ImageView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Primitives/UVSphere.h>

#include <chrono>
#include <list>

#include "TexturedDrawable.h"
#include "ImGuiImpl.h"
#include "CallPoint.h"
#include "BackgroundWorkManager.h"
#include "CallPointsManager.h"
#include "GrpcServer.h"

namespace CoordinateViewerMain {

#define LOADING_ERROR           1
#define SCROLL_DELTA            2.5f
#define MIN_ZOOM_IN             5.0f
#define MAX_ZOOM_OUT            20.0f
#define ROTATION_RATIO          0.002f
#define CAMERA_VERT_MAX         0.1f
#define CAMERA_VERT_MIN         3.0f
#define LIGHT_DISTANCE          30.0f
#define LIGHT_VERTICAL_ANGLE    Constants::pi() / 2

using namespace Magnum;
using namespace Magnum::Math::Literals;
using namespace std::chrono_literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class CoordinateViewer: public Platform::Application 
{
public:
    explicit CoordinateViewer(const Arguments& arguments);
    friend class ::ImGuiImpl;
    friend class ::CallPointsManager;

private:
    void objectsInit();
    void loadResources();

    void drawEvent() override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;
    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;
    void textInputEvent(TextInputEvent& event) override;
    void viewportEvent(ViewportEvent& event) override;

    void placeCamera();
    void placeLightTimeBased(const std::chrono::system_clock::time_point& time_point);

    static Vector3 fromPolarCoordinates(const float phi_rad, const float theta_rad, const float r);
    static float mapZeroBased(const float fromMax, const float toMax, const float value);

    BackgroundWorkManager mBackgroundManager;
    CallPointsManager mCallPointsManager;

    ImGuiImpl mImgui;
    GL::Mesh mEarthMesh, mPointMesh;
    GL::Texture2D mEarthTexture;
    Shaders::PhongGL mEarthShader{
        Shaders::PhongGL::Flag::DiffuseTexture 
        | Shaders::PhongGL::Flag::AmbientTexture 
        | Shaders::PhongGL::Flag::SpecularTexture};
    Shaders::PhongGL mColorShader;

    Scene3D mScene;
    Object3D mManimpulator, mCameraObject;
    Object3D *mSceneLightObj, *mEarthObj;
    SceneGraph::Camera3D *mCamera;
    SceneGraph::DrawableGroup3D mTexturedDrawables;
    SceneGraph::DrawableGroup3D mColoredDrawables;
    Vector2i mPreviousPosition;

    float mCameraHorizontalAngle = 0.0f;
    float mCameraVerticalAngle = Constants::pi() / 2;
    float mCameraDistance = 10.0f;
    std::chrono::steady_clock::time_point mMainLightTimestamp;

    std::list<CallPoint> mPoints;
};

}

#endif
