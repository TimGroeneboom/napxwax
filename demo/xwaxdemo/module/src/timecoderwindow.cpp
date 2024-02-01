#include "timecoderwindow.h"
#include <appguiservice.h>
#include <imgui.h>
#include <timecodernode.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <imguiservice.h>
#include <nap/core.h>

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::TimecoderWindow)
    RTTI_CONSTRUCTOR(nap::AppGUIService&)
    RTTI_PROPERTY("TimecoderComponent", &nap::TimecoderWindow::mTimecoderComponent, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

namespace nap
{
    static std::vector<const char*> control_names =
    {
        "serato_2a",
        "serato_2b",
        "serato_cd",
        "traktor_a",
        "traktor_b",
        "mixvibes_v2",
        "mixvibes_7inch",
        "pioneer_a",
        "pioneer_b"
    };

    static std::unordered_map<std::string, audio::ETimecodeContol> lookup_map =
    {
        { "serato_2a", audio::ETimecodeContol::SERATO_2A },
        { "serato_2b", audio::ETimecodeContol::SERATO_2B },
        { "serato_cd", audio::ETimecodeContol::SERATO_CD },
        { "traktor_a", audio::ETimecodeContol::TRACTOR_A },
        { "traktor_b", audio::ETimecodeContol::TRACTOR_B },
        { "mixvibes_v2", audio::ETimecodeContol::MIXVIBES_V2 },
        { "mixvibes_7inch", audio::ETimecodeContol::MIXVIBES_7INCH },
        { "pioneer_a", audio::ETimecodeContol::PIONEER_A },
        { "pioneer_b", audio::ETimecodeContol::PIONEER_B }
    };

    static std::unordered_map<audio::ETimecodeContol, const char*> reverse_loopkup_map =
    {
        { audio::ETimecodeContol::SERATO_2A, "serato_2a" },
        { audio::ETimecodeContol::SERATO_2B, "serato_2b" },
        { audio::ETimecodeContol::SERATO_CD, "serato_cd" },
        { audio::ETimecodeContol::TRACTOR_A, "traktor_a" },
        { audio::ETimecodeContol::TRACTOR_B, "traktor_b" },
        { audio::ETimecodeContol::MIXVIBES_V2, "mixvibes_v2" },
        { audio::ETimecodeContol::MIXVIBES_7INCH, "mixvibes_7inch" },
        { audio::ETimecodeContol::PIONEER_A, "pioneer_a" },
        { audio::ETimecodeContol::PIONEER_B, "pioneer_b" }
    };

    TimecoderWindow::TimecoderWindow(AppGUIService& guiService)
        : AppGUIWindow(guiService)
    {
    }

    TimecoderWindow::~TimecoderWindow()
    {
    }

    void TimecoderWindow::drawContent(double deltaTime)
    {
        if(mTimecoderComponent->hasInstance())
        {
            auto* imgui_service = mAppGUIService.getCore().getService<IMGuiService>();
            auto& timecoder = mTimecoderComponent->getInstance();

            float reference_speed = timecoder.getReferenceSpeed();
            if(ImGui::SliderFloat("Reference Speed", &reference_speed, 0.0f, 5.0f))
            {
                timecoder.setReferenceSpeed(reference_speed);
            }

            double relative_time = timecoder.getRelativeTime();
            if(ImGui::InputDouble("Relative Time", &relative_time))
            {
                timecoder.setRelativeTime(relative_time);
            }

            const auto& control_method_str = reverse_loopkup_map[timecoder.getControl()];
            int idx = 0;
            for(int i = 0 ; i < control_names.size(); ++i)
            {
                if(control_names[i] == control_method_str)
                {
                    idx = i;
                    break;
                }
            }
            if(ImGui::Combo("Control", &idx, control_names.data(), control_names.size()))
            {
                timecoder.setControl(lookup_map[control_names[idx]]);
            }
            double time_code = timecoder.getTimecode();
            ImGui::Text("Timecode:");
            ImGui::SameLine();
            if(time_code < 0.0)
            {
                ImGui::TextColored(imgui_service->getPalette().mHighlightColor2, "invalid");
            }
            else
            {
                ImGui::TextColored(imgui_service->getPalette().mHighlightColor2, "%.2f", time_code);
            }

            double pitch = timecoder.getPitch();
            ImGui::Text("Pitch:");
            ImGui::SameLine();
            ImGui::TextColored(imgui_service->getPalette().mHighlightColor2, "%.2f", pitch);

            double time = timecoder.getRelativeTime();
            float angle = fmodf(static_cast<float>((time / 1.8) * 360), 360.0f);
            ImGui::Text("Angle:");
            ImGui::SameLine();
            ImGui::TextColored(imgui_service->getPalette().mHighlightColor2, "%.2f", angle);

            int framerate = static_cast<int>(1.0 / deltaTime);
            ImGui::Text("Framerate:");
            ImGui::SameLine();
            ImGui::TextColored(imgui_service->getPalette().mHighlightColor2, "%d", framerate);

            // draw circle
            auto* draw_list = ImGui::GetWindowDrawList();

            ImVec2 o = { 25, 25 };
            ImVec2 s = { 100, 100 };
            ImVec2 p = ImGui::GetCursorScreenPos();
            p = { p.x + o.x, p.y + o.y };

            draw_list->AddCircleFilled({p.x + s.x, p.y + s.y}, s.x, IM_COL32(255, 0, 0, 255), 32);

            float rad = math::radians(angle);

            ImVec2 up = { 0, -1 };
            float x = up.x * cos(rad) - up.y * sin(rad);
            float y = up.x * sin(rad) + up.y * cos(rad);
            draw_list->AddLine({p.x + s.x, p.y + s.y}, {p.x + s.x + x * s.x, p.y + s.y + y * s.x}, IM_COL32(255, 255, 255, 255), 2);
        }
    }
}