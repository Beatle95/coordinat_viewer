#include <Magnum/ImageView.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

using namespace Magnum;
using namespace Magnum::Math::Literals;

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class ColoredDrawable: public SceneGraph::Drawable3D 
{
public:
    explicit ColoredDrawable(
        Object3D& object, 
        Shaders::PhongGL& shader, 
        GL::Mesh& mesh,
        const Color4& color,
        SceneGraph::DrawableGroup3D& group)
        : SceneGraph::Drawable3D{object, &group}, 
        mShader(shader), 
        mMesh(mesh),
        mColor(color) {}

private:
    void draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) override;

    Shaders::PhongGL& mShader;
    GL::Mesh& mMesh;
    const Color4 mColor;
};