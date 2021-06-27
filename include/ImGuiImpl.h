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
    void init(
        Platform::Application *application, 
        const Vector2i& windowSize, 
        const Vector2& dpiScaling, 
        const Vector2i& framebufferSize);
    void draw();

    inline bool keyPressEvent(Platform::Application::KeyEvent& event)
    {
        return mImgui.handleKeyPressEvent(event);
    }

    inline bool keyReleaseEvent(Platform::Application::KeyEvent& event)
    {
        return mImgui.handleKeyReleaseEvent(event);
    }

    inline bool mousePressEvent(Platform::Application::MouseEvent& event)
    {
        return mImgui.handleMousePressEvent(event);
    }

    inline bool mouseReleaseEvent(Platform::Application::MouseEvent& event)
    {
        return mImgui.handleMouseReleaseEvent(event);
    }

    inline bool mouseMoveEvent(Platform::Application::MouseMoveEvent& event)
    {
        return mImgui.handleMouseMoveEvent(event);
    }

    inline bool mouseScrollEvent(Platform::Application::MouseScrollEvent& event)
    {
        return mImgui.handleMouseScrollEvent(event);
    }

    inline bool textInputEvent(Platform::Application::TextInputEvent& event)
    {
        return mImgui.handleTextInputEvent(event);
    }
    
    inline void viewportEvent(Platform::Application::ViewportEvent& event)
    {
        mImgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
            event.windowSize(), event.framebufferSize());
    }

private:
    Platform::Application *mApplication;
    ImGuiIntegration::Context mImgui{NoCreate};
};

#endif
