#include "CallPoint.h"

CallPoint::CallPoint(Object3D *manipulator, 
        SceneGraph::DrawableGroup3D *drawableGroup, 
        Shaders::PhongGL *shader,
        GL::Mesh *mesh,
        const float lat,
        const float lon) 
    : mColoredDrawables(drawableGroup), mLat(lat), mLon(lon)
{
    mObject3D = new Object3D(manipulator);
    mDrawable = new ColoredDrawable(*mObject3D, *shader, *mesh, DEFAULT_COLOR, *mColoredDrawables);
    mObject3D->scale(Vector3(0.05f, 0.05f, 0.05f));
    setObjectPostion();
}

CallPoint::~CallPoint() 
{
    mColoredDrawables->remove(*mDrawable);
    delete mDrawable;
    delete mObject3D;
}

void CallPoint::setObjectPostion() 
{
    Vector3 pos;
    float lat_ = mLat - 90.0f;
    mObject3D->rotateX(Deg(-(mLat - 180.0f)));
    mObject3D->rotateY(Deg(mLon));
    pos.x() = EARTH_RADIUS * sin(Deg(-lat_)) * sin(Deg(mLon));
    pos.y() = EARTH_RADIUS * cos(Deg(-lat_));
    pos.z() = EARTH_RADIUS * sin(Deg(-lat_)) * cos(Deg(mLon));
    mObject3D->translate(pos);
}
