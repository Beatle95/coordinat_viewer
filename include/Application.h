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
#include <Magnum/Primitives/Cube.h>
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

#define LOADING_ERROR 1
    
using namespace Magnum;
using namespace Magnum::Math::Literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class CoordinatViewer: public Platform::Application 
{
public:
    explicit CoordinatViewer(const Arguments& arguments);

private:
    void drawEvent() override;
    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void viewportEvent(ViewportEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    Vector3 positionOnSphere(const Vector2i& position);

    GL::Mesh mEarthMesh;
    GL::Texture2D mEarthTexture;
    Shaders::PhongGL mTextureShader{Shaders::PhongGL::Flag::DiffuseTexture};

    Scene3D mScene;
    Object3D mManimpulator, mCameraObject;
    SceneGraph::Camera3D *mCamera;
    SceneGraph::DrawableGroup3D mDrawables;
    Vector3 mPreviousPosition;
    Object3D *mSceneLightObj, *mEarthObj;
};

class TexturedDrawable: public SceneGraph::Drawable3D 
{
public:
    explicit TexturedDrawable(
        Object3D& object, 
        Shaders::PhongGL& shader, 
        GL::Mesh& mesh, 
        GL::Texture2D& texture, 
        SceneGraph::DrawableGroup3D& group,
        Object3D* sceneLightObj)
        : SceneGraph::Drawable3D{object, &group}, 
        mShader(shader), 
        mMesh(mesh), 
        mTexture(texture),
        mSceneLightObj(sceneLightObj) {}

private:
    void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

    Shaders::PhongGL& mShader;
    GL::Mesh& mMesh;
    GL::Texture2D& mTexture;
    Object3D *mSceneLightObj;
};
