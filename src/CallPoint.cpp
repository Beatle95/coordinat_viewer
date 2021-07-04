#include "CallPoint.h"

CallPoint::CallPoint(Object3D *parent, 
        SceneGraph::DrawableGroup3D *drawableGroup, 
        Shaders::PhongGL *shader,
        GL::Mesh *mesh,
        const float lat,
        const float lon,
        const Color4& color) 
    : mDrawableGroup(drawableGroup), mLat(lat), mLon(lon)
{
    mObject3D = new Object3D(parent);
    mDrawable = new ColoredDrawable(*mObject3D, *shader, *mesh, color, *drawableGroup);
    setObjectPostion();
}

CallPoint::CallPoint(CallPoint&& other) 
{
    mObject3D       = other.mObject3D;
    mDrawable       = other.mDrawable;
    mDrawableGroup  = other.mDrawableGroup;
    mLat            = other.mLat;
    mLon            = other.mLon;
    other.mObject3D         = nullptr;
    other.mDrawable         = nullptr;
    other.mDrawableGroup    = nullptr;
}

CallPoint::~CallPoint() 
{
    mDrawableGroup->remove(*mDrawable);
    delete mDrawable;
    delete mObject3D;
}

void CallPoint::setColor(const Color4& color) 
{
    mDrawable->setColor(color);
}

void CallPoint::setPosition(const float lat, const float lon) 
{
    mLat = lat;
    mLon = lon;
    setObjectPostion();
}

void CallPoint::setObjectPostion() 
{
    Vector3 pos;
    float lat_ = mLat - 90.0f;
    auto rotation = Matrix4::rotation(Deg(-(mLat - 180.0f)), Vector3(1, 0, 0));
    rotation = Matrix4::rotation(Deg(mLon), Vector3(0, 1, 0)) * rotation;
    auto scaling = Matrix4::scaling(Vector3(0.05f, 0.05f, 0.05f));
    pos.x() = EARTH_RADIUS * sin(Deg(-lat_)) * sin(Deg(mLon));
    pos.y() = EARTH_RADIUS * cos(Deg(-lat_));
    pos.z() = EARTH_RADIUS * sin(Deg(-lat_)) * cos(Deg(mLon));
    mObject3D->setTransformation(Matrix4::translation(pos) * rotation * scaling);
}
