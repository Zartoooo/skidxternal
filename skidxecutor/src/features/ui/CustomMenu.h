#pragma once
#include "../../../ext/imgui/imgui.h"
#include <vector>
#include <string>
#include "../../../ext/imgui/imgui_internal.h"
#include <unordered_map>

class CustomMenu {
public:
    CustomMenu(ImDrawList* drawList)
        : DrawList(drawList), MenuPos(100, 100), MenuSize(400, 300),
        Dragging(false), CurrentTab(0), ActiveControl(-1), ControlID(0) {

        Tabs = { "Aimbot", "Visuals", "Misc", "Settings" };

        TabAnimProgress.resize(Tabs.size(), 0.0f);

        ColBackground = IM_COL32(25, 25, 25, 255);
        ColBorder = IM_COL32(60, 60, 60, 255);
        ColTitleBar = IM_COL32(40, 40, 40, 255);
        ColText = IM_COL32(220, 220, 220, 255);
        ColWidget = IM_COL32(40, 40, 40, 255);
        ColWidgetActive = IM_COL32(48, 85, 255, 255);
        ColButtonActive = IM_COL32(50, 50, 50, 255);
        ColSliderGrab = IM_COL32(180, 180, 180, 255);
        ColTabInactive = IM_COL32(100, 100, 100, 255);
        ColTabHover = IM_COL32(150, 150, 150, 255);
        ColTabActive = IM_COL32(48, 85, 255, 255);
    }

    void Render() {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 mousePos = io.MousePos;
        HandleDragging(mousePos, io);

        DrawList->AddRectFilled(MenuPos, MenuPos + MenuSize, ColBackground, 4);
        DrawList->AddRect(MenuPos, MenuPos + MenuSize, ColBorder, 4);

        ImVec2 titleBarMin = MenuPos;
        ImVec2 titleBarMax = MenuPos + ImVec2(MenuSize.x, 28);
        DrawList->AddRectFilled(titleBarMin, titleBarMax, ColTitleBar, 4, ImDrawFlags_RoundCornersTop);
        DrawList->AddRect(titleBarMin, titleBarMax, ColBorder, 4, ImDrawFlags_RoundCornersTop);
        DrawList->AddText(MenuPos + ImVec2(10, 7), ColText, "Zentra Internal - Insert To Open/Close - Last Updated 6/22/2025");

        DrawTabs();

        ImVec2 footerBarMin = MenuPos + ImVec2(0, MenuSize.y - 24);
        ImVec2 footerBarMax = MenuPos + MenuSize;
        DrawList->AddRectFilled(footerBarMin, footerBarMax, ColTitleBar, 4, ImDrawFlags_RoundCornersBottom);
        DrawList->AddRect(footerBarMin, footerBarMax, ColBorder, 4, ImDrawFlags_RoundCornersBottom);
        const char* footerText = "v0.0.1 (dev-build)";
        ImVec2 textSize = ImGui::CalcTextSize(footerText);
        ImVec2 footerCenter = footerBarMin + ImVec2((MenuSize.x - textSize.x) * 0.5f, (24 - textSize.y) * 0.5f);
        DrawList->AddText(footerCenter, ColText, footerText);

        ContentPos = MenuPos + ImVec2(12, 28 + 6.f + 26.f + 10);
        ContentSize = MenuSize - ImVec2(24, 28 + 26 + 16);
        ContentCursorPos = ContentPos;
    }

    bool Button(const char* label) {
        ImVec2 textSize = ImGui::CalcTextSize(label);
        ImVec2 padding = ImVec2(12, 4);
        ImVec2 size = ImVec2(ImMax(90.0f, textSize.x + padding.x * 2), ImMax(22.0f, textSize.y + padding.y * 2));

        ImVec2 pos = ContentCursorPos;
        ImRect rect(pos, pos + size);

        bool hovered = RectHover(rect);
        bool clicked = hovered && ImGui::GetIO().MouseClicked[0];

        DrawList->AddRectFilled(rect.Min, rect.Max, ColWidget);
        DrawList->AddRect(rect.Min, rect.Max, ColBorder, 3);

        ImVec2 textPos = rect.Min + (size - textSize) * 0.5f;
        DrawList->AddText(textPos, ColText, label);

        ContentCursorPos.y += size.y + 10;

        return clicked;
    }

    bool Checkbox(const char* label, bool* v) {
        ImVec2 boxSize(16, 16);
        ImVec2 pos = ContentCursorPos;
        ImRect box(pos, pos + boxSize);

        ImGuiID id = ImGui::GetID(label);
        float& anim = CheckboxAnimProgress[id];

        bool hovered = RectHover(box);
        bool clicked = hovered && ImGui::GetIO().MouseClicked[0];
        if (clicked) {
            *v = !*v;
            anim = 0.f;
        }

        float target = *v ? 1.0f : 0.0f;
        anim = ImClamp(ImLerp(anim, target, ImGui::GetIO().DeltaTime * 10.f), 0.0f, 1.0f);

        ImColor from = ImColor(ColWidget);
        ImColor to = ImColor(ColWidgetActive);
        ImColor blended = ImColor(
            ImLerp(from.Value.x, to.Value.x, anim),
            ImLerp(from.Value.y, to.Value.y, anim),
            ImLerp(from.Value.z, to.Value.z, anim),
            ImLerp(from.Value.w, to.Value.w, anim)
        );
        ImU32 fillColor = blended;

        DrawList->AddRectFilled(box.Min, box.Max, fillColor, 2);
        DrawList->AddRect(box.Min, box.Max, ColBorder, 2);

        ImVec2 labelSize = ImGui::CalcTextSize(label);
        ImVec2 labelPos = ImVec2(box.Max.x + 8, box.Min.y + (boxSize.y - labelSize.y) * 0.5f);
        DrawList->AddText(labelPos, ColText, label);

        ContentCursorPos.y += boxSize.y + 10;

        return clicked;
    }

    bool SliderInt(const char* label, int* v, int min, int max) {
        ImVec2 pos = ContentCursorPos;
        float barWidth = 250.f;
        float sliderHeight = 10.f;
        ImRect slider(pos + ImVec2(0, 18), pos + ImVec2(barWidth, 18 + sliderHeight));

        ImGuiID id = ImGui::GetID(label);
        float& anim = SliderAnimProgress[id];

        float target = (*v - min) / float(max - min);
        anim = ImLerp(anim, target, ImGui::GetIO().DeltaTime * 10.f);

        ImRect fill = slider;
        fill.Max.x = fill.Min.x + anim * slider.GetWidth();

        ImColor from = ImColor(ColWidgetActive);
        ImColor to = ImColor(ColWidgetActive);
        ImColor blended = ImColor(
            ImLerp(from.Value.x, to.Value.x, anim),
            ImLerp(from.Value.y, to.Value.y, anim),
            ImLerp(from.Value.z, to.Value.z, anim),
            ImLerp(from.Value.w, to.Value.w, anim)
        );
        ImU32 fillColor = blended;

        DrawList->AddText(pos, ColText, label);
        DrawList->AddRectFilled(slider.Min, slider.Max, ColWidget, 2);
        DrawList->AddRectFilled(fill.Min, fill.Max, fillColor, 2);
        DrawList->AddRect(slider.Min, slider.Max, ColBorder, 2);

        float marker_height = sliderHeight + 5.0f;
        float marker_y = slider.Min.y + (sliderHeight - marker_height) * 0.5f;
        DrawList->AddLine(ImVec2(fill.Max.x, marker_y), ImVec2(fill.Max.x, marker_y + marker_height), ColSliderGrab, 4);

        if (RectHover(slider) && ImGui::GetIO().MouseDown[0]) {
            float rel = ImGui::GetIO().MousePos.x - slider.Min.x;
            rel = ImClamp(rel, 0.f, slider.GetWidth());
            *v = min + int((rel / slider.GetWidth()) * (max - min));
        }

        char value_buf[64];
        const char* end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), ImGuiDataType_S32, v, "%d");
        ImVec2 valSize = ImGui::CalcTextSize(value_buf);
        DrawList->AddText(ImVec2(slider.Max.x - valSize.x, pos.y), ColText, value_buf, end);

        ContentCursorPos.y += 30;

        return true;
    }

    void SetTab(int i) { if (i >= 0 && i < Tabs.size()) CurrentTab = i; }
    int GetTab() const { return CurrentTab; }

private:
    ImDrawList* DrawList;
    ImVec2 MenuPos, MenuSize;
    ImVec2 ContentPos, ContentSize, ContentCursorPos;
    std::vector<const char*> Tabs;
    int CurrentTab;
    bool Dragging;
    ImVec2 DragOffset;
    int ActiveControl;
    int ControlID;
    std::vector<float> TabAnimProgress;
    std::unordered_map<ImGuiID, float> CheckboxAnimProgress;
    std::unordered_map<ImGuiID, float> SliderAnimProgress;

    ImU32 ColBackground, ColBorder, ColTitleBar, ColText, ColWidget, ColWidgetActive, ColButtonActive, ColSliderGrab, ColTabInactive, ColTabHover, ColTabActive;

    bool RectHover(const ImRect& r) {
        return r.Contains(ImGui::GetIO().MousePos);
    }

    void HandleDragging(ImVec2 mouse, ImGuiIO& io) {
        ImRect head(MenuPos, MenuPos + ImVec2(MenuSize.x, 28));
        if (Dragging && !io.MouseDown[0]) Dragging = false;
        if (Dragging) MenuPos = mouse - DragOffset;
        else if (io.MouseDown[0] && head.Contains(mouse)) {
            Dragging = true;
            DragOffset = mouse - MenuPos;
        }
    }

    void DrawTabs() {
        int tabCount = Tabs.size();
        float tabHeight = 26.f;
        float gap = 6.f;
        float totalWidth = MenuSize.x - 20.f;
        float totalGaps = gap * (tabCount - 1);
        float tabWidth = (totalWidth - totalGaps) / tabCount;
        ImVec2 start = MenuPos + ImVec2(10, 28 + 6.f);
        float delta = ImGui::GetIO().DeltaTime * 10.0f;

        for (int i = 0; i < tabCount; ++i) {
            ImVec2 pos = start + ImVec2(i * (tabWidth + gap), 0);

            ImRect tabRect(pos, pos + ImVec2(tabWidth, tabHeight));
            bool hovered = tabRect.Contains(ImGui::GetIO().MousePos);

            float& anim = TabAnimProgress[i];
            float target = (i == CurrentTab || hovered) ? 1.0f : 0.0f;
            anim = ImClamp(ImLerp(anim, target, delta), 0.0f, 1.0f);

            ImColor from = ImColor(ColTabInactive);
            ImColor to = ImColor(i == CurrentTab ? ColTabActive : ColTabHover);
            ImColor blended = ImColor(
                ImLerp(from.Value.x, to.Value.x, anim),
                ImLerp(from.Value.y, to.Value.y, anim),
                ImLerp(from.Value.z, to.Value.z, anim),
                ImLerp(from.Value.w, to.Value.w, anim)
            );
            ImU32 color = blended;
            ImVec2 textSize = ImGui::CalcTextSize(Tabs[i]);
            ImVec2 center = pos + ImVec2(tabWidth * 0.5f, tabHeight * 0.5f);
            ImVec2 textPos = ImVec2(center.x - textSize.x * 0.5f, pos.y + 4);

            DrawList->AddText(textPos, color, Tabs[i]);

            if (i == CurrentTab || anim > 0.1f) {
                float underlineWidth = tabWidth * anim;
                float underlineX = pos.x + (tabWidth - underlineWidth) * 0.5f;
                DrawList->AddRectFilled(ImVec2(underlineX, pos.y + tabHeight - 2), ImVec2(underlineX + underlineWidth, pos.y + tabHeight), ColTabActive, 1);
            }

            if (ImGui::IsMouseClicked(0) && hovered) CurrentTab = i;
        }
    }
};