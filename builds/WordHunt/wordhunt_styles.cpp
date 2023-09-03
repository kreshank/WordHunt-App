// Style information and macro

// [SECTION] SETUP
// [SECTION] STYLE MODIFIERS
// [SECTION] CUSTOM OBJECTS

#define IMGUI_DEFINE_MATH_OPERATORS

#include "wordhunt_styles.h"
#include "macros.h"

//-------------------------------------------------------------------------
// [SECTION] SETUP
//-------------------------------------------------------------------------

WHStyle* default = new WHStyle();
WHStyle* current = default;
WHStyle* config;

WHStyle::WHStyle()
{
    // Theme
    ThemeCol_Main = IM_COL32(124, 171, 126, 255);
    ThemeCol_Secondary = IM_COL32(236, 205, 155, 255);
    ThemeCol_Tertiary = IM_COL32(141, 233, 133, 255);

    // Window
    WindowBackgroundCol = IM_COL32(127, 150, 118, 255);
    ThemeFontFamily = NotoSerifSemiBold;
    WindowFlags_Default = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiTableFlags_NoSavedSettings;

    // Title
    TitleTextSize = FontSize_64px;
    TitleFontFamily = OpenSansBold;
    TitleCol = IM_COL32(220, 220, 220, 100);

    // Text Graphics
    TextSize = FontSize_20px;
    TextCol = IM_COL32(0, 0, 0, 255);

    // Button
    ButtonTextSize = FontSize_24px;
    ButtonRounding = 15.0f;
    ButtonPadding = ImVec2(30, 10);
    ButtonBackgroundCol = IM_COL32(241, 207, 145, 255);
    ButtonBackgroundCol_Active = IM_COL32(135, 198, 200, 255);
    ButtonBackgroundCol_Hovered = IM_COL32(141, 233, 133, 255); // Make a transition
    ButtonTextCol = IM_COL32(0, 0, 0, 255);
    ButtonBorderCol = IM_COL32(95, 158, 160, 255);

    // Clock Graphics
    ClockCol_Rim = IM_COL32(0, 0, 0, 255);
    ClockCol_Body = IM_COL32(255, 255, 255, 255);
    ClockCol_Tick = IM_COL32(0, 0, 0, 255);
    ClockCol_Elapsed = IM_COL32(180, 0, 0, 120);

    // Tile Graphics
    TileSpacing;
    TilePadding = 12.0f;
    TileRounding = 15.0f;
    TileTextSize = FontSize_80px;
    TileSize = ImVec2(100, 100);
    TileBackgroundCol_Default = IM_COL32(236, 205, 155, 255);
    TileBackgroundCol_Valid = IM_COL32(141, 233, 133, 225);
    TileBackgroundCol_Invalid = IM_COL32(250, 250, 250, 255);
    TileBackgroundCol_Repeated = IM_COL32(241, 243, 119, 225);
    TileBackgroundCol_Unselected = IM_COL32(236, 205, 155, 255);
    TileTextCol = IM_COL32(0, 0, 0, 255);

    // Selection Graphics
    SelectedPath_Thickness = 7.5f;
    SelectedPathCol_Default = IM_COL32(250, 250, 250, 255);
    SelectedPathCol_Valid = IM_COL32(250, 250, 250, 255);
    SelectedPathCol_Invalid = IM_COL32(250, 160, 160, 255);
    SelectedPathCol_Repeated = IM_COL32(250, 250, 250, 255);

    // Solution Graphics
    SolutionTitleSize = FontSize_30px;
    SolutionTitleCol = IM_COL32(0, 0, 0, 255);
    SolutionItemRounding = 5.0f;
    SolutionItemCol_Background;
    SolutionItemCol_Text;
    SolutionExplorerCol;
    SolutionExplorerMaxSize;
    SolutionScoreSize;
    SolutionScoreCol;
}

void WHGui::LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // NotoSerif-Black
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Black.ttf", 80.0f);

    // NotoSerif-Bold
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Bold.ttf", 80.0f);

    // NotoSerif-ExtraBold
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraBold.ttf", 80.0f);

    // NotoSerif-ExtraLight
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-ExtraLight.ttf", 80.0f);

    // NotoSerif-Italic
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Italic.ttf", 80.0f);

    // NotoSerif-Light
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Light.ttf", 80.0f);

    // NotoSerif-Medium
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Medium.ttf", 80.0f);

    // NotoSerif-Regular
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 80.0f);

    // NotoSerif-SemiBold
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 80.0f);

    // NotoSerif-Thin
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Thin.ttf", 80.0f);

    // OpenSans-Bold
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Bold.ttf", 80.0f);

    // OpenSans-ExtraBold
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-ExtraBold.ttf", 80.0f);

    // OpenSans-Italic
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Italic.ttf", 80.0f);

    // OpenSans-Light
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Light.ttf", 80.0f);

    // OpenSans-Medium
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Medium.ttf", 80.0f);

    // OpenSans-Regular
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-Regular.ttf", 80.0f);

    // OpenSans-SemiBold
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 24.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 30.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 36.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 42.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 48.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 64.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/OpenSans-SemiBold.ttf", 80.0f);

}

WHStyle* WHGui::GetWHStyle()
{
    return current;
}

void WHGui::ResetToDefaultWHStyle()
{
    current = default;
}

WHStyle* WHGui::LoadStyleFromFile(const char* filename)
{
    return config;
}

ImFont* WHGui::FontRetrieve(FontFamily ff, FontSize fs)
{
    ImGuiIO& io = ImGui::GetIO();
    return io.Fonts->Fonts[ff * 10 + fs];
}

//-------------------------------------------------------------------------
// [SECTION] STYLE MODIFIERS
//-------------------------------------------------------------------------


void WHGui::PushTitleStyle()
{
    ImGuiIO io = ImGui::GetIO();
    WHStyle* styler = WHGui::GetWHStyle();

    ImGui::PushFont(WHGui::FontRetrieve(styler->TitleFontFamily, styler->TitleTextSize));
    ImGui::PushStyleColor(ImGuiCol_Text, styler->TitleCol);
}

void WHGui::PopTitleStyle()
{
    ImGui::PopFont();
    ImGui::PopStyleColor();
}

void WHGui::PushButtonStyle()
{
    ImGuiIO io = ImGui::GetIO();
    WHStyle* styler = WHGui::GetWHStyle();

    ImGui::PushFont(WHGui::FontRetrieve(styler->ThemeFontFamily, styler->ButtonTextSize));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, styler->ButtonRounding);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, styler->ButtonPadding);

    ImGui::PushStyleColor(ImGuiCol_Button, styler->ButtonBackgroundCol);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, styler->ButtonBackgroundCol_Active);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, styler->ButtonBackgroundCol_Hovered);
    ImGui::PushStyleColor(ImGuiCol_Text, styler->ButtonTextCol);
    ImGui::PushStyleColor(ImGuiCol_Border, styler->ButtonBorderCol);
}

void WHGui::PopButtonStyle()
{
    ImGui::PopFont();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(5);
}

void WHGui::PushTileStyle()
{

}

void WHGui::PopTileStyle()
{

}

void WHGui::PushWindowStyle()
{
    WHStyle* styler = WHGui::GetWHStyle();

    ImGui::PushStyleColor(ImGuiCol_WindowBg, styler->WindowBackgroundCol);
}

void WHGui::PopWindowStyle()
{
    ImGui::PopStyleColor();
}

//-------------------------------------------------------------------------
// [SECTION] CUSTOM OBJECTS
//-------------------------------------------------------------------------

// Returns true if solution item hovered
bool WHGui::SolutionItem(Solution* entry, const ImVec2& size, ImU32 solution_color, ImGuiWindowFlags flags, const std::string prefix)
{
    std::string solution_id = prefix + " : " + entry->to_string();
    solution_id += " tile";
    bool is_hovered = false;
    if (ImGui::BeginChild(solution_id.data(), size, false, flags))
    {
        ImVec2 text_dimensions = ImGui::CalcTextSize(entry->word);
        ImVec2 padding = ImVec2(5, 2.5f);

        static char buf[6] = { 0 };
        int point_value = WordHunt::GetPointVal(entry->length), i = 4;
        while (point_value && i)
        {
            buf[i--] = "0123456789"[point_value % 10];
            point_value /= 10;
        }

        ImVec2 point_dimensions = ImGui::CalcTextSize(&buf[i + 1]);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, solution_color);

        if (ImGui::BeginChild(entry->word, text_dimensions + padding * 2))
        {
            ImGui::SetCursorPos(padding);
            ImGui::Text(entry->word);
        }
        ImGui::EndChild();

        ImGui::PopStyleColor();

        ImGui::SetCursorPos(ImGui::GetWindowSize() - point_dimensions - padding);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::Text(&buf[i + 1]);
        ImGui::PopStyleColor();
    }
    is_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
    ImGui::EndChild();
    return is_hovered;
}

bool WHGui::Button(const char* label, const ImVec2& size)
{
    WHGui::PushButtonStyle();
    bool ret_val = ImGui::Button(label, size);
    WHGui::PopButtonStyle();
    return ret_val;
}
