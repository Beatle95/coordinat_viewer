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

#include <chrono>

#include "TexturedDrawable.h"
#include "ImGuiImpl.h"

#define LOADING_ERROR           1
#define SCROLL_DELTA            2.5f
#define MIN_ZOOM_IN             5.0f
#define MAX_ZOOM_OUT            20.0f
#define ROTATION_RATIO          0.125f
#define LIGHT_DISTANCE          30.0f
#define LIGHT_VERTICAL_ANGLE    90.0f

using namespace Magnum;
using namespace Magnum::Math::Literals;
using namespace std::chrono_literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class CoordinatViewer: public Platform::Application 
{
public:
    explicit CoordinatViewer(const Arguments& arguments);

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

    static Vector3 fromPolarCoordinates(const float phi, const float theta, const float r);
    static float mapZeroBased(const float fromMax, const float toMax, const float value);

    ImGuiImpl mImgui;
    GL::Mesh mEarthMesh;
    GL::Texture2D mEarthTexture;
    Shaders::PhongGL mEartShader{
        Shaders::PhongGL::Flag::DiffuseTexture 
        | Shaders::PhongGL::Flag::AmbientTexture 
        | Shaders::PhongGL::Flag::SpecularTexture};

    Scene3D mScene;
    Object3D mManimpulator, mCameraObject;
    Object3D *mSceneLightObj, *mEarthObj;
    SceneGraph::Camera3D *mCamera;
    SceneGraph::DrawableGroup3D mDrawables;
    Vector2i mPreviousPosition;

    float mCameraHorizontalAngle = 0.0f;
    float mCameraVerticalAngle = 90.0f;
    float mCameraDistance = 10.0f;

    bool mIsLightPosRealTimeBased = true;
    std::chrono::system_clock::time_point mLightTime;
    std::chrono::steady_clock::time_point mLightCheckTimestamp;
};
