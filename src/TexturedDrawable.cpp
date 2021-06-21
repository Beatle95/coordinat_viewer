#include "TexturedDrawable.h"

void TexturedDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) 
{
    auto lightPos = mSceneLightObj->transformation().translation();
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