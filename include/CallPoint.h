#ifndef CALL_POINT
#define CALL_POINT
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/GL/Mesh.h>

#include "ColoredDrawable.h"

#define EARTH_RADIUS    3.212f
#define DEFAULT_COLOR   0x888888_rgbf

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
class Session;

class CallPoint
{
public:
    CallPoint() = delete;
    explicit CallPoint(Object3D *parent, 
        SceneGraph::DrawableGroup3D *drawableGroup, 
        Shaders::PhongGL *shader,
        GL::Mesh *mesh,
        const float lat = 0.0f,
        const float lon = 0.0f,
        const Color4& color = DEFAULT_COLOR);
    CallPoint(CallPoint&& other);
    ~CallPoint();

    void setColor(const Color4& color);
    void setPosition(const float lat, const float lon);
private:
    Object3D *mObject3D;
    ColoredDrawable *mDrawable;
    SceneGraph::DrawableGroup3D* mDrawableGroup;
    float mLat, mLon;

    void setObjectPostion();
};

#endif
