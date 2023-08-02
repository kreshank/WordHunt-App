// Dear ImGui: standalone example application for DirectX 12
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// Important: to compile on 32-bit systems, the DirectX12 backend requires code to be compiled with '#define ImTextureID ImU64'.
// This is because we need ImTextureID to carry a 64-bit value and by default ImTextureID is defined as void*.
// This define is set in the example .vcxproj file and need to be replicated in your app or by adding it to your imconfig.h file.

#include <iostream>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>
#include "wordhunt.h"
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

// Helper Macros

#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void GenerateRandomGame(char* s, const size_t len, int seed = 0);

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Word Hunt", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

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
        style.Colors[ImGuiCol_WindowBg] = ImVec4(ImGui::ColorConvertU32ToFloat4(IM_COL32(127, 150, 118, 255)));
        style.Colors[ImGuiCol_Button] = ImVec4(ImGui::ColorConvertU32ToFloat4(IM_COL32(95, 158, 160, 255)));
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
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-Regular.ttf", 20.0f);
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/NotoSerif-SemiBold.ttf", 80.0f);

    // WordHunt setup
    Dictionary* current_dictionary = WordHunt::CreateDictionary("../../misc/files/dictionary.txt");

    // Our state
    bool        show_demo_window = false;
    bool        show_main_menu = true;
    bool        show_random_game = false;
    bool        show_custom_game = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Constants
    const static int    maxRows = 15;
    const static int    maxColumns = 15;
    const static int    numTiles = maxRows * maxColumns;
    const static ImU32  default_board_color = IM_COL32(236, 205, 155, 255);

    // Board State
    static int          numRows = 4;
    static int          numColumns = 4;
    static char         board[maxRows][maxColumns];
    static bool         isActive[maxRows][maxColumns];

    // Selection Variables
    static ImVec2       TilePosition[maxRows][maxColumns]; // Holds pointer to the window position of the
    static ImVec2       TilePath[numTiles];
    static bool         activated[maxRows][maxColumns];

    // Solution
    static char         word[numTiles];
    static int          word_length = 0;
    static std::string  word_str;
    static char         previous_word[256];
    static int          previous_word_length = 0;
    static std::string  previous_word_str;
    static bool         currently_is_word = false;
    static std::set<std::string> found_words;

    // Line Variables
    // White lines, green background on real word
    // Red line, white background on wrong
    const ImU32         pale_white = IM_COL32(250, 250, 250, 255);
    const ImU32         pale_red = IM_COL32(250, 160, 160, 255);
    const ImU32         pale_green = IM_COL32(190, 225, 193, 225);
    const ImU32         bright_yellow = IM_COL32(241, 243, 119, 225);
    static float        line_thickness = 7.5f;
    static ImU32        line_color = pale_white;


    static auto start_timer = std::chrono::high_resolution_clock::now();
    static auto end_timer = std::chrono::high_resolution_clock::now() + std::chrono::seconds(80);
    static auto timer_delta = std::chrono::duration_cast<std::chrono::seconds>(end_timer - start_timer);

    std::mt19937 rng(__rdtsc()); // random function
    static bool game_begin = false;
    static int seed;
    static Seed* game_seed = new Seed(std::uniform_int_distribution<int>(0)(rng));


    // Main loop
    bool done = false;
    while (!done)
    {
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
        }
        // our window
        ImGuiWindowFlags main_game_window_flags = ImGuiWindowFlags_NoResize;
        main_game_window_flags |= ImGuiWindowFlags_NoMove;
        main_game_window_flags |= ImGuiWindowFlags_NoDecoration;
        main_game_window_flags |= ImGuiWindowFlags_NoCollapse;
        main_game_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (show_main_menu)
        {
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImU32(IM_COL32(30, 144, 255, 255)));
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::Begin("Main Menu", &show_main_menu, main_game_window_flags);

            if (ImGui::Button("Play a new game!"))
            {
                ImGui::SetWindowFocus();
                show_random_game = true;
                game_begin = false;
            }
            if (ImGui::Button("Create a game!"))
            {
                ImGui::SetWindowFocus();
                show_custom_game = true;
            }
            if (ImGui::Button("Show demo/debug window"))
            {
                ImGui::SetWindowFocus();
                show_demo_window = true;
            }

            ImGui::End();
            ImGui::PopStyleColor();
        }

        if (show_random_game)
        {
            // Forward declaration of variable

            ImGui::Begin("Word Hunt", &show_random_game, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse );

            // Game state
            const float         TilePadding = 12.0f;
            static float        TileWidth = IM_CLAMP(ImGui::GetWindowWidth() / (numRows + 1), 75, 100);
            static float        TileHeight = IM_CLAMP(ImGui::GetWindowHeight() / (numColumns + 1), 75, TileWidth);
            
            static ImVec2       previous_tile = ImVec2(-1, -1);
            static char         letters[256] = "";
            float board_width = TileWidth * numColumns + style.ItemSpacing.x * (numColumns - 1);
            float board_height = TileHeight * numRows + style.ItemSpacing.y * (numRows - 1);

            TileWidth = TileHeight;

            // Set correct color theme
            if (currently_is_word = current_dictionary->IsWord(word, word_length))
            {
                line_color = pale_white;
            }
            else
            {
                line_color = pale_red;
            }

            // TODO - Create a window or selection system for the board shape / size
            // Replace with a button that starts the game

            // [SECTION] - GAME SETTING SELECTORS

            if (!game_begin)
            {
                static char seed_buffer[256] = "Enter a seed or ignore and continue.";
                ImGui::InputText("##seed input", seed_buffer, 256, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll);
                if (ImGui::IsItemClicked())
                {
                    strncpy(seed_buffer, "", 256);
                }

                static std::string seed_button_text = "Load seed";

                if (ImGui::Button(seed_button_text.data()))
                {
                    if (WordHunt::IsValidSeed(seed_buffer) == -1)
                    {
                        seed_button_text = "Invalid seed";
                    }
                    else {
                        game_seed = new Seed(seed_buffer);
                        seed_button_text = "Seed loaded!";
                    }
                }

                if (!game_begin && ImGui::Button("Begin Random Game"))
                {
                    seed = std::uniform_int_distribution<int>(0)(rng);
                    GenerateRandomGame(letters, numTiles, seed);
                    found_words.clear();
                    game_begin = true;
                }
            }

            if (game_begin)
            {
                // Create a style for the Tiles
                // Rounded square tiles, black serif font
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 15.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
                ImGui::PushFont(io.Fonts->Fonts[1]);

                // Center the board
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - board_width) * 0.5f);
                ImGui::BeginChild("Board", ImVec2(board_width, board_height), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);

                for (int row = 0; row < numRows; row++)
                {
                    for (int column = 0; column < numColumns; column++)
                    {
                        if (column) ImGui::SameLine();

                        // Creating a unique tile ID for each
                        char TileID[7] = "Tile  ";
                        TileID[4] = row + 1;
                        TileID[5] = column + 1;

                        ImU32 tile_color = default_board_color;
                        if (word_length > 2 && activated[row][column])
                        {
                            if (currently_is_word)
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
                        ImGui::BeginChild(TileID, ImVec2(TileWidth, TileHeight), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
                        TilePosition[row][column] = ImVec2(ImGui::GetWindowPos().x + TileWidth / 2, ImGui::GetWindowPos().y + TileHeight / 2);

                        // Center and print tile
                        char* visible_character = &letters[(row * numRows + column) * 2];
                        ImVec2 text_dimensions = ImGui::CalcTextSize(visible_character);
                        ImGui::SetCursorPos(ImVec2((TileWidth - text_dimensions.x) * 0.5f, (TileHeight - text_dimensions.y) * 0.5f));
                        ImGui::Text(visible_character);

                        // Check if hovering over a tile
                        ImGui::SetCursorPos(ImVec2());
                        ImVec2 hitboxMin(ImGui::GetCursorScreenPos().x + TilePadding, ImGui::GetCursorScreenPos().y + TilePadding), hitboxMax(ImGui::GetCursorScreenPos().x + TileWidth - TilePadding, ImGui::GetCursorScreenPos().y + TileHeight - TilePadding);
                        if (game_begin && ImGui::IsMouseHoveringRect(hitboxMin, hitboxMax) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                        {
                            if (!activated[row][column] && ((previous_tile.x == -1 && previous_tile.y == -1) || (abs(previous_tile.x - row) <= 1 && abs(previous_tile.y - column) <= 1)))
                            {
                                activated[row][column] = 1;
                                char letter = visible_character[0];
                                word[word_length] = letter;
                                TilePath[word_length++] = TilePosition[row][column];
                                word[word_length] = 0;
                                previous_tile = ImVec2(row, column);
                            }
                            if (word_length)
                            {
                                ImGui::GetForegroundDrawList()->AddLine(TilePath[word_length - 1], io.MousePos, line_color, line_thickness);
                                ImGui::GetForegroundDrawList()->AddCircleFilled(io.MousePos, line_thickness / 2, line_color);
                            }
                        }

                        ImGui::EndChild();

                        ImGui::PopStyleColor();
                    }
                }

                ImGui::EndChild();

                ImGui::PopFont();
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();

                if (game_begin && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                {
                    previous_word_length = word_length;
                    for (int i = 0; i <= word_length; i++)
                    {
                        previous_word[i] = word[i];
                    }
                    word_length = 0;

                    previous_word_str = std::string(previous_word, previous_word + previous_word_length);
                    if (previous_word_length > 2 && current_dictionary->IsWord(previous_word_str))
                    {
                        found_words.insert(previous_word_str);
                    }

                    previous_tile = ImVec2(-1, -1);
                    memset(activated, 0, sizeof(activated));
                }
            }
            for (auto a : found_words)
            {
                ImGui::Text(a.data());
            }

            // Draw selection path
            for (int i = 1; i < word_length; i++)
            {
                ImGui::GetForegroundDrawList()->AddLine(TilePath[i - 1], TilePath[i], line_color, line_thickness);
                ImGui::GetForegroundDrawList()->AddCircleFilled(TilePath[i - 1], line_thickness / 2, line_color);
            }

            // Display Previous
            ImGui::Text("Previous Selection");
            ImGui::SameLine();
            ImGui::Text(previous_word);

            // Display Current
            ImGui::Text("Current Selection:");
            ImGui::SameLine();
            ImGui::Text(word);

            if (0)
            {
                static const char* seed_string = game_seed->to_string();
                static ImVec2 seed_text_dimensions = ImGui::CalcTextSize(seed_string);
                ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - (int)seed_text_dimensions.x, ImGui::GetWindowHeight() - (int)seed_text_dimensions.y));
                ImGui::Text(seed_string);
            }

            ImGui::End();
        }

        if (show_custom_game)
        {
            ImGui::Begin("Main Game Window", &show_custom_game);
            static char buf[128] = " test ";
            ImGui::InputText("label 1", buf, 10);

            const static int max = 10;

            static int numRows = 4;
            static int numCols = 4;

            // Variable size
            {
                // Row Changing
                if (ImGui::Button("Increment Rows") && numRows != max)
                    numRows++;
                ImGui::SameLine();
                if (ImGui::Button("Decrement Rows") && numRows != 0)
                    numRows--;

                if (ImGui::Button("Increment Col") && numCols != max)
                    numCols++;
                ImGui::SameLine();
                if (ImGui::Button("Decrement Col") && numCols != 0)
                    numCols--;

                static char data[200] = "";

                if (ImGui::BeginTable("Game Board", numCols, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
                {
                    for (int row = 0; row < numRows; row++)
                    {
                        ImGui::TableNextRow();
                        for (int col = 0; col < numCols; col++)
                        {
                            ImGui::TableSetColumnIndex(col);
                            ImU32 cell_bg_color = IM_COL32(236, 205, 155, 255);
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);

                            ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AlwaysOverwrite | ImGuiInputTextFlags_NoHorizontalScroll;


                            char randomLabel[10] = "##RTile  ";
                            randomLabel[7] = row + '1';
                            randomLabel[8] = col + '1';

                            ImGui::InputText(randomLabel, &data[(col * 10 + row) * 2], 2, textFlags);

                            if (ImGui::IsItemHovered())
                                ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(190, 229, 176, 255));
                        }
                    }
                    ImGui::EndTable();
                }

            }

            // Static size
            /*
            static char buf1[128] = "";
            static char buf2[128] = "";
            static char buf3[128] = "";
            static char buf4[128] = "";

            if (ImGui::BeginTable("Static Table", 4))
            {
                for (int row = 0; row < numRows; row++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::InputText("", buf1, 2);
                    ImGui::TableNextColumn();
                    ImGui::InputText("", buf2, 2);
                    ImGui::TableNextColumn();
                    ImGui::InputText("", buf3, 2);
                    ImGui::TableNextColumn();
                    ImGui::InputText("", buf4, 2);
                }
                ImGui::EndTable();
            }
            */
            if (ImGui::Button("Quit"))
                done = true;
            if (ImGui::Button("Show Demo"))
                show_demo_window = true;
            ImGui::End();
        }

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        


        {
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

void GenerateRandomGame(char* s, const size_t len, int seed) {
    std::mt19937 rng(seed);
    char* characterSet = "EEEEEEEEEEEEAAAAAAAAAIIIIIIIIIOOOOOOOONNNNNNRRRRRRTTTTTTLLLLSSSSUUUUDDDDGGGBBCCMMPPFFHHVVWWYYKJXQZ";
    for (size_t i = 0; i < len * 2; i += 2) {
        s[i] = characterSet[std::uniform_int_distribution<int>(0, 98)(rng)];
        s[i + 1] = 0;
    }
    s[len * 2] = 0;
}
