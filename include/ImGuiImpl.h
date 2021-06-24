#ifndef IMGUI_IMPL
#define IMGUI_IMPL
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <imgui.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

using namespace Magnum;

class ImGuiImpl
{
public:
    void init(Platform::Application *application, Vector2i& windowSize, Vector2& dpiScaling, Vector2i& framebufferSize);
    void draw();
    void viewportEvent(Platform::Application::ViewportEvent& event);
private:
    Platform::Application *mApplication;
    ImGuiIntegration::Context mImgui{NoCreate};
};

#endif
