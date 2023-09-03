#pragma once
#include "imgui/imgui.h"
#include "wordhunt.h"

typedef int TileState;
typedef int FontFamily;
typedef int FontSize;

struct WHStyle;

enum TileState_
{
    TileState_Default,
    TileState_Valid,
    TileState_Invalid,
    TileState_Repeated
};

enum FontFamily_
{
    NotoSerifBlack,
    NotoSerifBold,
    NotoSerifExtraBold,
    NotoSerifExtraLight,
    NotoSerifItalic,
    NotoSerifLight,
    NotoSerifMedium,
    NotoSerifRegular,
    NotoSerifSemiBold,
    NotoSerifThin,
    OpenSansBold,
    OpenSansExtraBold,
    OpenSansItalic,
    OpenSansLight,
    OpenSansMedium,
    OpenSansRegular,
    OpenSansSemiBold
};

enum FontSize_
{
    FontSize_16px,
    FontSize_18px,
    FontSize_20px,
    FontSize_24px,
    FontSize_30px,
    FontSize_36px,
    FontSize_42px,
    FontSize_48px,
    FontSize_64px,
    FontSize_80px
};

namespace WHGui
{
    // Setup
    WHStyle* GetWHStyle();
    void ResetToDefaultWHStyle();
    WHStyle* LoadStyleFromFile(const char* filename);
    void LoadFonts();
    ImFont* FontRetrieve(FontFamily ff, FontSize fs);

    // Style Modifiers
    void PushTitleStyle();
    void PopTitleStyle();
    void PushButtonStyle();
    void PopButtonStyle();
    void PushTileStyle();
    void PopTileStyle();
    void PushWindowStyle();
    void PopWindowStyle();

    // Custom Objects
    bool SolutionItem(Solution* entry, const ImVec2& size, ImU32 solution_color, ImGuiWindowFlags flags = 0, const std::string prefix = "");
    bool Clock(ImVec2 &size, bool border, ImGuiWindowFlags flags = 0);
    bool Button(const char* label, const ImVec2& size = ImVec2(0, 0));
}

struct WHStyle
{
    // Theme
    ImU32           ThemeCol_Main;
    ImU32           ThemeCol_Secondary;
    ImU32           ThemeCol_Tertiary;
    FontFamily      ThemeFontFamily;

    // Window
    ImU32           WindowBackgroundCol;
    float           WindowRounding;
    ImGuiWindowFlags WindowFlags_Default;

    // Title
    FontSize        TitleTextSize;
    FontFamily      TitleFontFamily;
    ImU32           TitleCol;

    // Text Graphics
    FontSize        TextSize;
    ImU32           TextCol;

    // Button
    FontSize        ButtonTextSize;
    float           ButtonRounding;
    ImVec2          ButtonPadding;
    ImU32           ButtonBackgroundCol;
    ImU32           ButtonBackgroundCol_Active;
    ImU32           ButtonBackgroundCol_Hovered;
    ImU32           ButtonTextCol;
    ImU32           ButtonBorderCol;

    // Clock Graphics
    ImVec2 ClockSize;
    float ClockRadius_Inner;
    float ClockRadius_Outer;
    float ClockRimWidth;
    ImU32           ClockCol_Rim;
    ImU32           ClockCol_Body;
    ImU32           ClockCol_Tick;
    ImU32           ClockCol_Elapsed;

    // Tile Graphics
    float           TileSpacing;
    float           TilePadding;
    float           TileRounding;
    FontSize        TileTextSize;
    ImVec2          TileSize;
    ImU32           TileBackgroundCol_Default;
    ImU32           TileBackgroundCol_Valid;
    ImU32           TileBackgroundCol_Invalid;
    ImU32           TileBackgroundCol_Repeated;
    ImU32           TileBackgroundCol_Unselected;
    ImU32           TileTextCol;

    // Selection Graphics
    float           SelectedPath_Thickness;
    ImU32           SelectedPathCol_Default;
    ImU32           SelectedPathCol_Valid;
    ImU32           SelectedPathCol_Invalid;
    ImU32           SelectedPathCol_Repeated;

    // Solution Graphics
    float           SolutionTitleSize;
    ImU32           SolutionTitleCol;
    float           SolutionItemRounding;
    ImU32           SolutionItemCol_Background;
    ImU32           SolutionItemCol_Text;
    ImU32           SolutionExplorerCol;
    ImVec2          SolutionExplorerMaxSize;
    float           SolutionScoreSize;
    ImU32           SolutionScoreCol;

    WHStyle();
};
