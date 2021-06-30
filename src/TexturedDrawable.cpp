#include "TexturedDrawable.h"

void TexturedDrawable::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera) 
{
    auto lightPos = mSceneLightObj->transformation().translation();
    auto view = camera.cameraMatrix();
    auto VP = view;
    auto absoluteLightPos = VP * Vector4(lightPos, 0);
    mShader
        .setLightPositions({
            absoluteLightPos
        })
        .setTransformationMatrix(transformationMatrix)
        .setNormalMatrix(transformationMatrix.normalMatrix())
        .setProjectionMatrix(camera.projectionMatrix())
        .bindTextures(&mTexture, &mTexture, &mTexture)
        .draw(mMesh);
}
