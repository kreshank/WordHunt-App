#include <iostream>

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui/imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include "wordhunt.h"
#include "wordhunt_styles.h"
#include "macros.h"
#include <chrono>
#include <random>
#include <set>

// Forward Declarations
static void ShowGameSelector(bool* p_open);
static void ShowGameScreen(bool* p_open);
static void ShowGameCreator(bool* p_open);
static void ShowSeedSelector(bool* p_open);
static void ShowSolver(bool* p_open);
static void ShowSettings(bool* p_open);

// Helpers
// Creates tool tips
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static void FullScreenNextWindow()
{
    // Use entire work area (without menu-bars, task-bars etc.)
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
}

// Main Menu
void WordHunt::WordHuntMenu(bool* p_open)
{
    static bool show_game_selector = false;
    static bool show_solver = false;
    static bool show_settings = false;
    static bool show_imgui_demo = false;

    if (show_game_selector) ShowGameSelector(&show_game_selector);
    if (show_solver)        ShowSolver(&show_solver);
    if (show_settings)      ShowSettings(&show_settings);
    if (show_imgui_demo)    ImGui::ShowDemoWindow(&show_imgui_demo);

    ImGuiWindowFlags main_menu_window_flags = ImGuiWindowFlags_NoResize;
    main_menu_window_flags |= ImGuiWindowFlags_NoMove;
    main_menu_window_flags |= ImGuiWindowFlags_NoDecoration;
    main_menu_window_flags |= ImGuiWindowFlags_NoCollapse;
    main_menu_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    main_menu_window_flags |= ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImU32(IM_COL32(30, 144, 255, 255)));
    
    FullScreenNextWindow();
    if (!ImGui::Begin("Main Menu", p_open, main_menu_window_flags))
    {
        // Early escape as optimization
        ImGui::End();
        return;
    }
    
    WHGui::PushTitleStyle();
    CENTERED_CONTROL(ImGui::Text("WORDHUNT"));
    WHGui::PopTitleStyle();

    if (CENTERED_CONTROL(WHGui::Button("Play!")))
    {
        ImGui::SetWindowFocus();
        show_game_selector = true;
        //game_phase = WordHuntGamePhase_Selection;
        //game_seed = new Seed(std::uniform_int_distribution<int>(0, INT_MAX)(rng));
        //discovered.clear();
        //found_words.clear();
    }
    if (CENTERED_CONTROL(WHGui::Button("Solver")))
    {
        ImGui::SetWindowFocus();
        show_solver = true;
    }
    if (CENTERED_CONTROL(WHGui::Button("Settings")))
    {
        // TODO
    }
    if (CENTERED_CONTROL(WHGui::Button("Show demo/debug window")))
    {
        ImGui::SetWindowFocus();
        show_imgui_demo = true;
    }

    ImGui::End();
    ImGui::PopStyleColor();
}

void ShowGameSelector(bool* p_open)
{
    static bool show_game_screen = false;
    static bool show_game_creator = false;
    static bool show_seed_selector = false;

    if (show_game_screen)   ShowGameScreen(&show_game_screen);
    if (show_game_creator)  ShowGameCreator(&show_game_creator);
    if (show_seed_selector) ShowSeedSelector(&show_seed_selector);

    FullScreenNextWindow();
    WHGui::PushWindowStyle();
    if (!ImGui::Begin("Game Selector", p_open, WHGui::GetWHStyle()->WindowFlags_Default))
    {
        ImGui::End();
        return;
    }

    WHGui::PushTitleStyle();
    CENTERED_CONTROL(ImGui::Text("WORDHUNT"));
    WHGui::PopTitleStyle();

    if (CENTERED_CONTROL(WHGui::Button("Quick Play")))
    {
        show_game_creator = true;
        p_open = false;
    }
    if (CENTERED_CONTROL(WHGui::Button("Custom")))
    {
        show_game_creator = true;
        p_open = false;
    }
    if (CENTERED_CONTROL(WHGui::Button("Set Seed")))
    {
        p_open = false;
    }

    ImGui::End();
    WHGui::PopWindowStyle();
}

void ShowGameScreen(bool* p_open)
{

}

void ShowGameCreator(bool* p_open)
{

}

void ShowSeedSelector(bool* p_open)
{

}

void ShowSolver(bool* p_open)
{

}

void ShowSettings(bool* p_open)
{

}
