#include "Menu.hpp"
#include "fonts/JetBrainsMono.h"
#include "../../sdk/Types.hpp"
#include "CustomMenu.h"
bool Menu::IsMouseInRect(ImVec2 min, ImVec2 max)
{
    auto mpos = ImGui::GetMousePos();

    return (mpos.x > min.x && mpos.x < max.x) && (mpos.y > min.y && mpos.y < max.y);
}

bool Menu::IsMouseClicked(ImVec2 min, ImVec2 max, ImGuiMouseButton button)
{
    return ImGui::IsMouseClicked(button) && IsMouseInRect(min, max);
}

void Menu::OnInit()
{
}

void Menu::OnShutdown()
{
}

void Menu::OnRender() {
	static bool& m_bMenuOpen = ConfigSys.Get<bool>("menu.open");
    static bool wasPressed = false;
    bool isPressed = GetAsyncKeyState(VK_INSERT) & 0x8000;

    if (isPressed && !wasPressed) {
        m_bMenuOpen = !m_bMenuOpen;

        // --- UPDATE WINDOW STATE ONLY ONCE PER TOGGLE ---
        LONG_PTR exStyle = GetWindowLongPtr(Overlay.g_hWindow, GWL_EXSTYLE);
        if (m_bMenuOpen) {
            // Remove click-through
            exStyle &= ~WS_EX_TRANSPARENT;
            exStyle &= ~WS_EX_NOACTIVATE;
            SetWindowLongPtr(Overlay.g_hWindow, GWL_EXSTYLE, exStyle);

            // FORCE the window to be interactable and focused
            SetForegroundWindow(Overlay.g_hWindow);
        }
        else {
            // Re-enable click-through
            exStyle |= WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;
            SetWindowLongPtr(Overlay.g_hWindow, GWL_EXSTYLE, exStyle);

            // Return focus to the game
            SetForegroundWindow(FindWindow("WINDOWSCLIENT", "Roblox"));
        }
    }
    wasPressed = isPressed;

    if (m_bMenuOpen) {
        // Draw a dim background to show the menu is active
        Overlay.BackgroundDrawList->AddRectFilled({ 0.f, 0.f }, ImGui::GetIO().DisplaySize, IM_COL32(0, 0, 0, 150));

        static CustomMenu menu(Overlay.BackgroundDrawList);

        menu.Render();
        static bool test1 = false;
        static int test2 = 0;
        int tab = menu.GetTab();
        if (tab == 0) {
            menu.Button("Test Button");
            menu.Checkbox("Enable Test1", &test1);
            menu.SliderInt("SliderInt TestInt1", &test2, 1, 100);
        }
        else if (tab == 1) {
            // visuals
        }
        else if (tab == 2) {
            // misc
        }
        else if (tab == 3) {
            // settings
        }

        ImGui::SetNextWindowSize({ 600.f, 700.f }, ImGuiCond_Once);
        if (ImGui::Begin("skidxternal (UD = Ultra Detected)", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            static int currenttab = 0;
			
			if (ImGui::Button("Aimbot",   { 60.f, 20.f })) currenttab = 0; ImGui::SameLine();
            if (ImGui::Button("Visuals",  { 60.f, 20.f })) currenttab = 1; ImGui::SameLine();
            if (ImGui::Button("Misc",     { 60.f, 20.f })) currenttab = 2; ImGui::SameLine();
            if (ImGui::Button("Settings", { 60.f, 20.f })) currenttab = 3;

            if (currenttab == 0) {
				static bool& aimbotEnabled = ConfigSys.Get<bool>("aimbot.enabled");
				static int& aimbotFov = ConfigSys.Get<int>("aimbot.fov");
				static bool& aimbotDrawFov = ConfigSys.Get<bool>("aimbot.drawfov");
				static float& aimbotSmoothingx = ConfigSys.Get<float>("aimbot.smoothing.x");
				static float& aimbotSmoothingy = ConfigSys.Get<float>("aimbot.smoothing.y");

				ImGui::Checkbox("Enabled", &aimbotEnabled);
				ImGui::Checkbox("Draw FOV", &aimbotDrawFov);
			    ImGui::SliderInt("FOV", &aimbotFov, 0, 360);

				ImGui::SliderFloat("Smoothing X", &aimbotSmoothingx, 1.f, 10.f);
				ImGui::SliderFloat("Smoothing Y", &aimbotSmoothingy, 1.f, 10.f);
            }
            else if (currenttab == 1) {
				static bool& espEnabled = ConfigSys.Get<bool>("visuals.playeresp");
				static ColorRGB& espBoxColor = ConfigSys.Get<ColorRGB>("visuals.playeresp.color");

                ImGui::Checkbox("Player ESP", &espEnabled);

				static float col[4] = { espBoxColor.r / 255.f, espBoxColor.g / 255.f, espBoxColor.b / 255.f, 1.f };
				ImGui::ColorPicker4("Player Esp Color", col);
				espBoxColor.FromArray(col);
            }
            else if (currenttab == 2) {
				static int& walkspeed = ConfigSys.Get<int>("misc.walkspeed");

				ImGui::SliderInt("Walkspeed (0=disable)", &walkspeed, 0, 300);
            }
            else if (currenttab == 3) {
                static std::string selectedConfig = "";
				static char newConfigName[MAX_PATH] = { 0 };

				ImGui::InputText("New Config Name", newConfigName, MAX_PATH);

                // Build a safe list of config names for the ListBox
                std::vector<const char*> configNamesVec;
                configNamesVec.reserve(ConfigSys.m_configs.size());
                for (const auto& [name, cfgptr] : ConfigSys.m_configs)
                {
                    configNamesVec.push_back(name.c_str());
                }

				static int selectedConfigIndex = 0;
                int configCount = static_cast<int>(configNamesVec.size());
                if (configCount > 0) {
                    // clamp index
                    if (selectedConfigIndex < 0) selectedConfigIndex = 0;
                    if (selectedConfigIndex >= configCount) selectedConfigIndex = configCount - 1;
                } else {
                    selectedConfigIndex = 0;
                }

                // Show configs list
                ImGui::ListBox("Configs", &selectedConfigIndex, configNamesVec.empty() ? nullptr : configNamesVec.data(), configCount);
                if (configCount > 0) {
                    selectedConfig = configNamesVec[selectedConfigIndex];
                } else {
                    selectedConfig.clear();
                }

                if (ImGui::Button("Save Config", { 100.f, 20.f })) 
                {
					// prefer the typed new name, otherwise use selected
					std::string nameToUse = std::string(newConfigName);
					if (nameToUse.empty()) nameToUse = selectedConfig;

                    if (nameToUse.empty()) {
                        // nothing to save / no name provided
                    } else {
                        // If the config exists already -> update it then persist
                        if (ConfigSys.m_configs.contains(nameToUse)) 
                        {
                            auto cfgptr = ConfigSys.m_configs[nameToUse];
                            if (!cfgptr) cfgptr = std::make_shared<CConfig>();
                            // write current vars into the config and persist
                            ConfigSys.SaveConfig(*cfgptr.get());
                            cfgptr->name = nameToUse;
                            ConfigSys.SaveToFile(std::format("{}.json", nameToUse), cfgptr);
                        }
                        else
                        {
                            // create new config, populate and persist
                            auto newCfg = std::make_shared<CConfig>();
                            newCfg->name = nameToUse;
                            ConfigSys.SaveConfig(*newCfg.get());
                            ConfigSys.m_configs[nameToUse] = newCfg;
                            ConfigSys.SaveToFile(std::format("{}.json", nameToUse), newCfg);
                            // update list index to the new config
                            selectedConfig = nameToUse;
                            // rebuild index to point to new entry
                            // naive approach: find its index
                            int idx = 0;
                            for (const auto& [n, _] : ConfigSys.m_configs) {
                                if (n == nameToUse) {
                                    selectedConfigIndex = idx;
                                    break;
                                }
                                ++idx;
                            }
                        }
                    }

                    memset(newConfigName, 0, sizeof(newConfigName)); // clears the input
                }

                ImGui::SameLine();
                if (ImGui::Button("Load Config", { 100.f, 20.f })) 
                {
					if (!selectedConfig.empty() && ConfigSys.m_configs.contains(selectedConfig))
					{
						ConfigSys.LoadConfig(*ConfigSys.m_configs[selectedConfig].get());
					}
                }
            }
            
        }
        ImGui::End();
    }
}

void Menu::RegisterFonts()
{
	Menu::Fonts::Main = ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)JetBrainsMono_Raw, sizeof(JetBrainsMono_Raw), 14.f);
}
