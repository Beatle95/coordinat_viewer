#include "ColoredDrawable.h"

void ColoredDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) 
{
    mShader
        .setAmbientColor(mColor)
        .setTransformationMatrix(transformationMatrix)
        .setNormalMatrix(transformationMatrix.normalMatrix())
        .setProjectionMatrix(camera.projectionMatrix())
        .draw(mMesh);
}
