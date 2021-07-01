#ifndef CALL_POINT
#define CALL_POINT
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/GL/Mesh.h>

#include "ColoredDrawable.h"

#define EARTH_RADIUS 3.212f
#define DEFAULT_COLOR 0x888888_rgbf

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;

class CallPoint
{
public:
    CallPoint() = delete;
    explicit CallPoint(Object3D *manipulator, 
        SceneGraph::DrawableGroup3D *drawableGroup, 
        Shaders::PhongGL *shader,
        GL::Mesh *mesh,
        const float lat,
        const float lon);
    ~CallPoint();
private:
    Object3D *mObject3D;
    ColoredDrawable *mDrawable;
    SceneGraph::DrawableGroup3D *mColoredDrawables;

    float mLat, mLon;
    bool isCaller;

    void setObjectPostion();
};

#endif
