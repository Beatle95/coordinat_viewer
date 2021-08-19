#include "ImGuiImpl.h"
#include "CoordinateViewer.h"

void ImGuiImpl::init(
    Platform::Application *application, 
    const Vector2i& windowSize, 
    const Vector2& dpiScaling, 
    const Vector2i& framebufferSize) 
{
    mApplication = application;
    ImGui::CreateContext();
    mImgui = ImGuiIntegration::Context(*ImGui::GetCurrentContext(),
        Vector2{windowSize}/dpiScaling, windowSize, framebufferSize);

    /* Set up proper blending to be used by ImGui. There's a great chance
    you'll need this exact behavior for the rest of your scene. If not, set
    this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
        GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
        GL::Renderer::BlendFunction::OneMinusSourceAlpha);
}

void ImGuiImpl::draw() 
{
    mImgui.newFrame();
    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !mApplication->isTextInputActive())
        mApplication->startTextInput();
    else if(!ImGui::GetIO().WantTextInput && mApplication->isTextInputActive())
        mApplication->stopTextInput();
    // draw light position window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Windows", nullptr);
    {
        ImGui::Text("Call points");

        if (ImGui::BeginTable("Call Points", 5, ImGuiTableFlags_Borders | ImGuiTableColumnFlags_WidthStretch)) {
            ImGui::TableSetupColumn("IMEI");
            ImGui::TableSetupColumn("Number");
            ImGui::TableSetupColumn("Latitude");
            ImGui::TableSetupColumn("Longitude");
            ImGui::TableSetupColumn("Actions");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("11122223333");
            ImGui::TableNextColumn();
            ImGui::Text("+7(911) 908-46-53");
            ImGui::EndTable();
        }
    }
    ImGui::End();
    /* Update application cursor */
    mImgui.updateApplicationCursor(*mApplication);
    /* Set appropriate states. If you only draw ImGui, it is sufficient to
       just enable blending and scissor test in the constructor. */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    mImgui.drawFrame();
    /* Reset state. Only needed if you want to draw something else with
       different state after. */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);
}
