#include "pch.h"

#include "Debug.h"


extern ImFont* font_subtitle;


void Debug::_imgui_debug_status_text(bool happy, std::string text)
{
    if (happy)
    {
        ImGui::TextColored(ImVec4{ 0, 1, 0, 1 }, "(good)");
    }
    else
    {
        ImGui::TextColored(ImVec4{ 1, 0, 0, 1 }, "(fail)");
    }

    ImGui::SameLine();

    ImGui::Text(text.c_str());
}

Debug::Debug() : items({}), demo_window_open(false)
{

    //::global_message = done ? "done cover_browser.jpg" : "D: failed.";
}

//void Debug::print(const char* fmt)
//{
//    items.push_back(fmt);
//}

void Debug::print(std::string text)
{
    items.push_back(text);
    printf(std::format("{0}\n", text).c_str());
}

void Debug::render(/* bool* p_open */)
{
    #ifdef _DEBUG

    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        ImGui::Begin("debug", NULL, window_flags);
        ImGui::SetWindowSize(ImVec2(432, 400), ImGuiCond_Once);
        ImGui::SetWindowPos(ImVec2(800, 600), ImGuiCond_Once);

        ImGui::SeparatorText("demo");
        // ImGui::ShowDemoWindow adds ~0.5mb to exe
        if (ImGui::Button("demo")) demo_window_open = true;
        if (demo_window_open)
            ImGui::ShowDemoWindow(&demo_window_open);

        {
            ImGui::SliderFloat("alpha", &(ImGui::GetStyle().Alpha), 0.4f, 1.0f, "%.2f");
        }


        ImGui::End();
    }

#endif
}
