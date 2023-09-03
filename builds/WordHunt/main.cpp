// Dear ImGui: standalone example application for DirectX 12
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// Important: to compile on 32-bit systems, the DirectX12 backend requires code to be compiled with '#define ImTextureID ImU64'.
// This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
// This define is set in the example .vcxproj file and need to be replicated in your app or by adding it to your imconfig.h file.

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

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    UINT64                  FenceValue;
};

// Data
static int const                    NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT                         g_frameIndex = 0;

static int const                    NUM_BACK_BUFFERS = 3;
static ID3D12Device*                g_pd3dDevice = nullptr;
static ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = nullptr;
static ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = nullptr;
static ID3D12CommandQueue*          g_pd3dCommandQueue = nullptr;
static ID3D12GraphicsCommandList*   g_pd3dCommandList = nullptr;
static ID3D12Fence*                 g_fence = nullptr;
static HANDLE                       g_fenceEvent = nullptr;
static UINT64                       g_fenceLastSignaledValue = 0;
static IDXGISwapChain3*             g_pSwapChain = nullptr;
static HANDLE                       g_hSwapChainWaitableObject = nullptr;
static ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Forward declarations of drawing functions
//bool SolutionItem(Solution* entry, const ImVec2& size, ImU32 solution_color, ImGuiWindowFlags flags = 0, const std::string prefix = "");
bool DrawSelectionPath(const ImVec2* tile_path_pos, const int word_length, ImU32 line_color, const float line_thickness);

// Forward declarations of static WordHunt variables


// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Word Hunt", WS_OVERLAPPEDWINDOW, 0, 0, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 5.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
        DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
        g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
        g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerifSC-Regular.otf", 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    WHGui::LoadFonts();

    // WordHunt setup
    WordHunt::Setup("../../misc/files/dictionary.txt");
    Dictionary* current_dictionary = WordHunt::GetDefaultDictionary();

    // Our state
    bool        show_demo_window = false;
    bool        show_main_menu = true;
    bool        show_random_game = false;
    bool        show_custom_game = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Constants
    const static int    max_row = 15;
    const static int    max_columns = 15;
    const static int    default_num_rows = 4;
    const static int    default_num_columns = 4;
    const static int    num_tiles = max_row * max_columns;
    const static ImU32  default_board_color = IM_COL32(236, 205, 155, 255);

    // Board State
    static int          num_rows = default_num_rows;
    static int          num_columns = default_num_columns;
    static char         board[max_row][max_columns];
    static bool         active_tiles[max_row][max_columns];
    static char         letters[256] = "";

    // Selection Variables
    static ImVec2       tile_centers[max_row][max_columns]; // Entries are center coordinates (center.x, center.y) of a tile
    static ImVec2       tile_path_pos[num_tiles]; // Center positions of currently selected tiles
    static ImVec2       tile_path_id[num_tiles]; // Holds the (Row, Column) of the tiles selected
    static bool         activated[max_row][max_columns]; // Is activated in path

    // Solution
    static char         word[num_tiles];
    static int          word_length = 0;
    static std::string  word_str;
    static char         previous_word[256];
    static int          previous_word_length = 0;
    static std::string  previous_word_str;
    static bool         currently_is_word = false;
    static std::set<std::string> found_words;
    static std::set<Solution*, SolutionPointerComparator> discovered;

    // Line Variables
    // White lines, green background on real word
    // Red line, white background on wrong
    const ImU32         pale_white = IM_COL32(250, 250, 250, 255);
    const ImU32         pale_red = IM_COL32(250, 160, 160, 255);
    const ImU32         pale_green = IM_COL32(190, 225, 193, 225);
    const ImU32         bright_yellow = IM_COL32(241, 243, 119, 225);
    static float        line_thickness = 7.5f;
    static ImU32        line_color = pale_white;


    static auto         start_timer = std::chrono::high_resolution_clock::now();
    static auto         end_timer = std::chrono::high_resolution_clock::now();
    static auto         timer_delta = std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer);
    static const float  game_length_seconds = 75.0f;

    std::mt19937        rng((unsigned int)__rdtsc()); // random function
    static int          game_phase = WordHuntGamePhase_Selection;
    static Seed*        game_seed;
    static char         seed_string[256] = "";

    static Solver* solver;

    WHStyle* styler = WHGui::GetWHStyle();

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // our window
        ImGuiWindowFlags main_game_window_flags = ImGuiWindowFlags_NoResize;
        main_game_window_flags |= ImGuiWindowFlags_NoMove;
        main_game_window_flags |= ImGuiWindowFlags_NoDecoration;
        main_game_window_flags |= ImGuiWindowFlags_NoCollapse;
        main_game_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        WordHunt::WordHuntMenu(&show_main_menu);
        //if (show_main_menu)
        //{
        //    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImU32(IM_COL32(30, 144, 255, 255)));
        //    ImGui::SetNextWindowPos(viewport->WorkPos);
        //    ImGui::SetNextWindowSize(viewport->WorkSize);
        //    ImGui::Begin("Main Menu", &show_main_menu, main_game_window_flags);

        //    WHGui::PushTitleStyle();
        //    CENTERED_CONTROL(ImGui::Text("WORDHUNT"));
        //    WHGui::PopTitleStyle();

        //    if (CENTERED_CONTROL(WHGui::Button("Play!")))
        //    {
        //        ImGui::SetWindowFocus();
        //        show_random_game = true;
        //        game_phase = WordHuntGamePhase_Selection;
        //        game_seed = new Seed(std::uniform_int_distribution<int>(0, INT_MAX)(rng));
        //        discovered.clear();
        //        found_words.clear();
        //    }
        //    if (CENTERED_CONTROL(WHGui::Button("Solver")))
        //    {
        //        ImGui::SetWindowFocus();
        //        show_custom_game = true;
        //    }
        //    if (CENTERED_CONTROL(WHGui::Button("Settings")))
        //    {
        //        // TODO
        //    }
        //    if (CENTERED_CONTROL(WHGui::Button("Show demo/debug window")))
        //    {
        //        ImGui::SetWindowFocus();
        //        show_demo_window = true;
        //    }

        //    ImGui::End();
        //    ImGui::PopStyleColor();
        //}

        if (show_random_game)
        {
            // Forward declaration of variable
            WHGui::PushWindowStyle();

            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);

            if (ImGui::Begin("##Word Hunt", &show_random_game, styler->WindowFlags_Default))
            {

                WHGui::PushTitleStyle();
                CENTERED_CONTROL(ImGui::Text("WORDHUNT"));
                WHGui::PopTitleStyle();

                // TODO - Create a window or selection system for the board shape / size
                // Replace with a button that starts the game

                // [SECTION] - GAME SETTING SELECTORS

                if (game_phase == WordHuntGamePhase_Selection)
                {
                    static char seed_buffer[256] = "Enter a seed or ignore and continue.";
                    ImGui::InputText("##seed input", seed_buffer, 256, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsNoBlank);
                    if (ImGui::IsItemClicked())
                    {
                        strncpy_s(seed_buffer, "", 256);
                    }

                    static std::string seed_button_text = "Load seed";
                    ImGui::SameLine();

                    if (ImGui::Button(seed_button_text.data()))
                    {
                        if (WordHunt::IsValidSeed(seed_buffer) == -1)
                        {
                            seed_button_text = "Invalid seed";
                        }
                        else {
                            game_seed = new Seed(seed_buffer);
                            seed_button_text = "Seed loaded!";
                            game_phase = WordHuntGamePhase_Generate;
                        }
                    }

                    if (ImGui::Button("Begin Random Game"))
                    {
                        game_phase = WordHuntGamePhase_Generate;
                    }
                    if (game_phase != WordHuntGamePhase_Selection)
                    {
                        strcpy_s(seed_buffer, "Enter a seed or ignore and continue.");
                    }
                }

                if (game_phase == WordHuntGamePhase_Generate)
                {
                    found_words.clear();
                    discovered.clear();
                    char* temp = game_seed->to_string();
                    int index = 0;
                    while (temp[index] != 0)
                    {
                        std::cout << temp[index]; // temp magically changes because of compiler optimization deadass deleted the second half of my string
                        seed_string[index] = temp[index];
                        index++;
                    }
                    for (int i = 0; i < num_rows; i++)
                    {
                        for (int j = 0; j < num_columns; j++)
                        {
                            active_tiles[i][j] = true;
                        }
                    }

                    WordHunt::GenerateGame(letters, num_tiles, game_seed->seed_value);
                    WordHunt::SetCurrentSeed(game_seed);
                    game_phase = WordHuntGamePhase_Play;
                    start_timer = std::chrono::high_resolution_clock::now();
                    end_timer = start_timer + std::chrono::seconds(int(game_length_seconds));
                }

                if (game_phase == WordHuntGamePhase_Play || game_phase == WordHuntGamePhase_Result)
                {
                    // DRAW CLOCK
                    if (game_phase == WordHuntGamePhase_Play)
                    {
                        static const float clock_width = 150.0f;
                        static const float clock_height = 150.0f;
                        static const float clock_outer_radius = clock_width / 2.0f;
                        static const float clock_rim_width = 5.0f;
                        static const float clock_inner_radius = clock_outer_radius - clock_rim_width;
                        static const float clock_tick_length = 7.5f;
                        static const int   clock_tick_quantity = 12;
                        static const float clock_angle_step = IM_PI / 6.0f;

                        static const ImU32 clock_rim_color = IM_COL32(0, 0, 0, 255);
                        static const ImU32 clock_center_color = IM_COL32(255, 255, 255, 255);
                        static const ImU32 clock_tick_color = IM_COL32(0, 0, 0, 255);
                        static const ImU32 clock_passed_color = IM_COL32(180, 0, 0, 120);

                        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_timer);
                        float passed_percentage = float(delta.count() / (game_length_seconds * 1000.0f));

                        ImVec2 clock_position = ImVec2((ImGui::GetWindowWidth() - clock_width) * 0.5f, ImGui::GetCursorPosY());
                        ImGui::SetCursorPos(clock_position);
                        ImVec2 clock_absolute_center = ImGui::GetCursorScreenPos() + ImVec2(clock_width * 0.5f, clock_height * 0.5f);
                        if (ImGui::BeginChild("Clock", ImVec2(clock_width, clock_height), false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs))
                        {
                            ImGui::GetForegroundDrawList()->AddCircleFilled(clock_absolute_center, clock_outer_radius, clock_rim_color);
                            ImGui::GetForegroundDrawList()->AddCircleFilled(clock_absolute_center, clock_inner_radius, clock_center_color);
                            if (passed_percentage < 1.0f)
                            {
                                ImGui::GetForegroundDrawList()->_Path.push_back(clock_absolute_center);
                                ImGui::GetForegroundDrawList()->PathArcTo(clock_absolute_center, clock_inner_radius, -IM_PI * 0.5f, IM_PI * (passed_percentage * 2.0f - 0.5f));
                                ImGui::GetForegroundDrawList()->PathFillConvex(clock_passed_color);
                            }
                            else
                            {
                                game_phase = WordHuntGamePhase_Result;
                                word_length = 0;
                                memset(activated, 0, sizeof(activated));
                                ImGui::GetForegroundDrawList()->_ResetForNewFrame();
                                ImGui::GetForegroundDrawList()->AddCircleFilled(clock_absolute_center, clock_inner_radius, clock_passed_color);
                                solver = WordHunt::SolveCurrentSeed(letters);
                            }

                            for (int i = 0; i < clock_tick_quantity; i++)
                            {
                                ImVec2 angle = ImVec2(cosf(i * clock_angle_step), sinf(i * clock_angle_step));
                                ImVec2 p1 = ImVec2(clock_inner_radius, clock_inner_radius) * angle + clock_absolute_center;
                                ImVec2 p2 = p1 - ImVec2(clock_tick_length, clock_tick_length) * angle;
                                ImGui::GetForegroundDrawList()->AddLine(p1, p2, clock_tick_color);
                            }
                        }
                        ImGui::EndChild();
                    }

                    // DRAW GAME BOARD 
                    const float         TilePadding = 12.0f;
                    const ImVec2        tile_size = ImVec2(100, 100);

                    static ImVec2       previous_tile = ImVec2(-1, -1);
                    ImVec2 board_size = ImVec2(tile_size.x, tile_size.y) * ImVec2(float(num_columns), float(num_rows)) + style.ItemSpacing * ImVec2(float(num_columns - 1), float(num_rows - 1));

                    // Set correct color theme
                    currently_is_word = current_dictionary->IsWord(word, word_length);
                    if (currently_is_word && word_length > 2)
                    {
                        line_color = pale_white;
                    }
                    else
                    {
                        line_color = pale_red;
                    }

                    // Draw selection path

                    DrawSelectionPath(tile_path_pos, word_length, line_color, line_thickness);

                    // Create a style for the Tiles
                    // Rounded square tiles, black serif font
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 15.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
                    ImGui::PushFont(io.Fonts->Fonts[NotoSerifSemiBold * 10 + FontSize_80px]);

                    // Center the board
                    ImVec2 board_pos = ImVec2((ImGui::GetWindowWidth() - board_size.x) * 0.5f, ImGui::GetCursorPosY());
                    ImGui::SetCursorPos(board_pos);
                    if (ImGui::BeginChild("Board", board_size, false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
                    {
                        for (int row = 0; row < num_rows; row++)
                        {
                            for (int column = 0; column < num_columns; column++)
                            {
                                if (column) ImGui::SameLine();

                                // Creating a unique tile ID for each
                                char TileID[7] = "Tile  ";
                                TileID[4] = char(row + 1);
                                TileID[5] = char(column + 1);

                                ImU32 tile_color = default_board_color;
                                if (activated[row][column])
                                {
                                    // TODO - Find a suitable way to make the after-game hover work
                                    if (game_phase == WordHuntGamePhase_Result)
                                    {
                                        tile_color = pale_green;
                                    }
                                    else if (word_length > 2 && currently_is_word)
                                    {
                                        word_str = std::string(word, word + word_length);
                                        if (found_words.find(word_str) == found_words.end())
                                        {
                                            tile_color = pale_green;
                                        }
                                        else
                                        {
                                            tile_color = bright_yellow;
                                        }
                                    }
                                    else
                                    {
                                        tile_color = pale_white;
                                    }
                                }

                                ImGui::PushStyleColor(ImGuiCol_ChildBg, tile_color);
                                // Tile Creation
                                ImGuiWindowFlags tile_flag = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove;
                                if (!active_tiles[row][column])
                                {
                                    tile_flag |= ImGuiWindowFlags_NoBackground;
                                }
                                if (ImGui::BeginChild(TileID, tile_size, true, tile_flag))
                                {
                                    tile_centers[row][column] = ImGui::GetWindowPos() + tile_size / 2.0f;

                                    // Center and print tile
                                    char* visible_character = &letters[(row * num_rows + column) * 2];
                                    ImVec2 text_dimensions = ImGui::CalcTextSize(visible_character);
                                    ImGui::SetCursorPos((tile_size - text_dimensions) * 0.5f);
                                    ImGui::Text(visible_character);

                                    // Check if hovering over a tile
                                    if (game_phase == WordHuntGamePhase_Play)
                                    {
                                        ImGui::SetCursorPos(ImVec2());
                                        ImVec2 hitbox_min = ImGui::GetCursorScreenPos() + ImVec2(TilePadding, TilePadding);
                                        ImVec2 hitbox_max = hitbox_min + tile_size;
                                        if (ImGui::IsMouseHoveringRect(hitbox_min, hitbox_max) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                                        {
                                            if (!activated[row][column] && ((previous_tile.x == -1 && previous_tile.y == -1) || (abs(previous_tile.x - row) <= 1 && abs(previous_tile.y - column) <= 1)))
                                            {
                                                activated[row][column] = 1;
                                                char letter = visible_character[0];
                                                word[word_length] = letter;
                                                tile_path_id[word_length] = ImVec2(float(row), float(column));
                                                tile_path_pos[word_length++] = tile_centers[row][column];
                                                word[word_length] = 0;
                                                previous_tile = ImVec2(float(row), float(column));
                                            }
                                            if (word_length)
                                            {
                                                ImGui::GetForegroundDrawList()->AddLine(tile_path_pos[word_length - 1], io.MousePos, line_color, line_thickness);
                                                ImGui::GetForegroundDrawList()->AddCircleFilled(io.MousePos, line_thickness / 2, line_color);
                                            }
                                        }
                                    }
                                }
                                ImGui::EndChild();

                                ImGui::PopStyleColor();
                            }
                        }
                    }
                    ImGui::EndChild();

                    ImGui::PopFont();
                    ImGui::PopStyleColor();
                    ImGui::PopStyleVar();


                    // WORD DETECTION LOGIC
                    if (game_phase == WordHuntGamePhase_Play)
                    {
                        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                        {
                            previous_word_length = word_length;
                            for (int i = 0; i <= word_length; i++)
                            {
                                previous_word[i] = word[i];
                            }
                            word_length = 0;

                            previous_word_str = std::string(previous_word, previous_word + previous_word_length);
                            if (previous_word_length > 2 && current_dictionary->IsWord(previous_word_str) && found_words.find(previous_word_str) == found_words.end())
                            {
                                found_words.insert(previous_word_str);
                                std::cout << previous_word_str << "\t\t of length [" << previous_word_length;
                                Tile* head = new Tile((int)tile_path_id[0].x, (int)tile_path_id[0].y, letters[(int)(tile_path_id[0].x * num_rows + tile_path_id[0].y) * 2]);
                                Tile* temp = head;
                                for (int i = 1; i < previous_word_length; i++)
                                {
                                    temp->next = new Tile((int)tile_path_id[i].x, (int)tile_path_id[i].y, letters[(int)(tile_path_id[i].x * num_rows + tile_path_id[i].y) * 2]);
                                    temp = temp->next;
                                }
                                discovered.insert(new Solution(head));
                                std::cout << "] is successfully added to [discovered]\n";
                            }
                            previous_tile = ImVec2(-1, -1);
                            memset(activated, 0, sizeof(activated));
                        }

                        // Display Previous
                        ImGui::Text("Previous Selection");
                        ImGui::SameLine();
                        ImGui::Text(previous_word);

                        // Display Current
                        ImGui::Text("Current Selection:");
                        ImGui::SameLine();
                        ImGui::Text(word);
                    }

                    // DRAW FOUND WORDS LIST
                    ImVec2 found_words_explorer_pos = ImVec2(0, board_pos.y);
                    ImGui::SetCursorPos(found_words_explorer_pos);
                    if (ImGui::BeginChild("found words", ImVec2(board_pos.x, board_size.y), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                        ImGui::Text("WORDS FOUND");
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
                        ImGui::PushFont(io.Fonts->Fonts[NotoSerifRegular * 10 + FontSize_20px]);


                        static const ImVec2 solution_size = ImVec2(0, 25);
                        for (Solution* a : discovered)
                        {
                            if (WHGui::SolutionItem(a, solution_size, default_board_color, ImGuiWindowFlags_NoMouseInputs, "found words") && game_phase == WordHuntGamePhase_Result)
                            {
                                word_length = a->length;
                                Tile* head = a->head;
                                int i = 0;
                                while (head)
                                {
                                    tile_path_pos[i++] = tile_centers[head->x][head->y];
                                    activated[head->x][head->y] = true;
                                    head = head->next;
                                }
                                line_color = pale_white;
                                DrawSelectionPath(tile_path_pos, word_length, line_color, line_thickness);

                                head = a->head;
                                i = 0;
                                while (head)
                                {
                                    activated[head->x][head->y] = false;
                                    head = head->next;
                                }
                                word_length = 0;
                            }
                        }
                        ImGui::PopFont();
                        ImGui::PopStyleColor();
                        ImGui::PopStyleVar();
                    }
                    ImGui::EndChild();

                    // DRAW SOLUTION LIST
                    if (game_phase == WordHuntGamePhase_Result)
                    {
                        ImVec2 solution_explorer_pos = ImVec2(board_pos.x + board_size.x, board_pos.y);
                        ImGui::SetCursorPos(solution_explorer_pos);
                        if (ImGui::BeginChild("solution", ImVec2(board_pos.x, board_size.y), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
                        {
                            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                            ImGui::Text("POSSIBLE WORDS");
                            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
                            ImGui::PushFont(io.Fonts->Fonts[NotoSerifRegular * 10 + FontSize_20px]);


                            static const ImVec2 solution_size = ImVec2(0, 25);
                            for (Solution* a : solver->sol_list)
                            {
                                if (WHGui::SolutionItem(a, solution_size, default_board_color, ImGuiWindowFlags_NoMouseInputs, "solution"))
                                {
                                    word_length = a->length;
                                    Tile* head = a->head;

                                    int i = 0;
                                    while (head)
                                    {
                                        tile_path_pos[i++] = tile_centers[head->x][head->y];
                                        activated[head->x][head->y] = true;
                                        head = head->next;
                                    }
                                    line_color = pale_white;
                                    DrawSelectionPath(tile_path_pos, word_length, line_color, line_thickness);

                                    head = a->head;
                                    i = 0;
                                    while (head)
                                    {
                                        activated[head->x][head->y] = false;
                                        head = head->next;
                                    }
                                    word_length = 0;
                                }
                            }
                            ImGui::PopFont();
                            ImGui::PopStyleColor();
                            ImGui::PopStyleVar();
                        }
                        ImGui::EndChild();
                    }

                    // DRAW SEED
                    ImVec2 seed_text_dimensions = ImGui::CalcTextSize(seed_string);
                    ImGui::SetCursorPos(ImGui::GetWindowSize() - seed_text_dimensions - ImVec2(10, 10));
                    ImGui::Text(seed_string);

                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                        if (ImGui::IsItemClicked())
                        {
                            ImGui::LogToClipboard();
                            ImGui::LogText(seed_string);
                        }
                    }
                }
            }
            ImGui::End();
            WHGui::PopWindowStyle();
        }

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        
        // Rendering
        ImGui::Render();

        FrameContext* frameCtx = WaitForNextFrameResources();
        UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
        frameCtx->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        g_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
        g_pd3dCommandList->ResourceBarrier(1, &barrier);

        // Render Dear ImGui graphics
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
        g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
        g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        g_pd3dCommandList->ResourceBarrier(1, &barrier);
        g_pd3dCommandList->Close();

        g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault(nullptr, (void*)g_pd3dCommandList);
        }

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync

        UINT64 fenceValue = g_fenceLastSignaledValue + 1;
        g_pd3dCommandQueue->Signal(g_fence, fenceValue);
        g_fenceLastSignaledValue = fenceValue;
        frameCtx->FenceValue = fenceValue;
    }

    WaitForLastSubmittedFrame();

    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

    // [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
    ID3D12Debug* pdx12Debug = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
        pdx12Debug->EnableDebugLayer();
#endif

    // Create device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
        return false;

    // [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
    if (pdx12Debug != nullptr)
    {
        ID3D12InfoQueue* pInfoQueue = nullptr;
        g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->Release();
        pdx12Debug->Release();
    }
#endif

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
            return false;

        SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            g_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
            return false;
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
            return false;
    }

    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
            return false;

    if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
        g_pd3dCommandList->Close() != S_OK)
        return false;

    if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
        return false;

    g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (g_fenceEvent == nullptr)
        return false;

    {
        IDXGIFactory4* dxgiFactory = nullptr;
        IDXGISwapChain1* swapChain1 = nullptr;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
            return false;
        if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, nullptr, nullptr, &swapChain1) != S_OK)
            return false;
        if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
            return false;
        swapChain1->Release();
        dxgiFactory->Release();
        g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
        g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->SetFullscreenState(false, nullptr); g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_hSwapChainWaitableObject != nullptr) { CloseHandle(g_hSwapChainWaitableObject); }
    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = nullptr; }
    if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = nullptr; }
    if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = nullptr; }
    if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = nullptr; }
    if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = nullptr; }
    if (g_fence) { g_fence->Release(); g_fence = nullptr; }
    if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }

#ifdef DX12_ENABLE_DEBUG_LAYER
    IDXGIDebug1* pDebug = nullptr;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
    {
        pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        pDebug->Release();
    }
#endif
}

void CreateRenderTarget()
{
    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource* pBackBuffer = nullptr;
        g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
        g_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void CleanupRenderTarget()
{
    WaitForLastSubmittedFrame();

    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = nullptr; }
}

void WaitForLastSubmittedFrame()
{
    FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue == 0)
        return; // No fence was signaled

    frameCtx->FenceValue = 0;
    if (g_fence->GetCompletedValue() >= fenceValue)
        return;

    g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
    WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameResources()
{
    UINT nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, nullptr };
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue != 0) // means no fence was signaled
    {
        frameCtx->FenceValue = 0;
        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            WaitForLastSubmittedFrame();
            CleanupRenderTarget();
            HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            assert(SUCCEEDED(result) && "Failed to resize swapchain.");
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break; 
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}



bool DrawSelectionPath(const ImVec2* tile_path_pos, const int word_length, ImU32 line_color, const float line_thickness)
{
    for (int i = 1; i < word_length; i++)
    {
        ImGui::GetForegroundDrawList()->AddLine(tile_path_pos[i - 1], tile_path_pos[i], line_color, line_thickness);
        ImGui::GetForegroundDrawList()->AddCircleFilled(tile_path_pos[i - 1], line_thickness / 2, line_color);
    }
    return true;
}
