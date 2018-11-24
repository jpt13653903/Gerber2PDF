#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_main.hpp"
#include "logic.hpp"
#include <iostream>
#include <optional>
#include <sstream>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <cstring>
#include "custom_widgets.hpp"

namespace fs = boost::filesystem;

/** Roadmap:
 *      [x] Implement file chooser
 *      [x] Implement add files button
 *      [x] ! Refactor Gerber ListBox
 *      [x] ! Refactor Filechooser
 *      [ ] Implement Save to batch file button
 *      [ ] Refactor for reusability
 *      [ ] Try compiling on windows
 */

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }


struct UIState {
    size_t active_row_index;
};

static void render_title();
static void render_list_box_action_btns(MainState *main_state, UIState *ui_state, ImVec2 size);
static void render_main_action_btns(MainState *main_state, ImVec2 size);
static void render_output_file(MainState *main_state);
static void render_page_options(MainState *main_state);

static ImGuiView g_imgui_view;
static MainState main_state;
static UIState ui_state;

void gui_setup(int argc, char **argv) {
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowRounding = 0.0f;
    auto io = ImGui::GetIO();
    g_imgui_view.normal_font = io.Fonts->AddFontFromFileTTF("fonts/FiraSans-Regular.ttf", 16.0f);
    g_imgui_view.title_font = io.Fonts->AddFontFromFileTTF("fonts/FiraSans-Regular.ttf", 36.0f);
    ui_state.active_row_index = -1;
}

void gui_loop() {
    ImGui::SetNextWindowPos(ImVec2(0.0f,0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Main", NULL, 
                    ImGuiWindowFlags_NoTitleBar|
                    ImGuiWindowFlags_NoResize|
                    ImGuiWindowFlags_NoCollapse|
                    ImGuiWindowFlags_NoMove);

    ImGui::SetCursorPosX(ImGui::GetCursorPosX()+20);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY()+20);
    render_title();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY()-20);
    render_page_options(&main_state);

    static auto LISTBOX_ACTION_BTNS_SIZE = ImVec2(0.0, 40);
    static auto MAIN_ACTION_BTNS_SIZE = ImVec2(0.0, 40);
    auto LISTBOX_SIZE = ImGui::GetWindowSize() - LISTBOX_ACTION_BTNS_SIZE - MAIN_ACTION_BTNS_SIZE - ImVec2(35.0, 200);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY()+20);
    ImGuiExt::GerberListBox(&main_state.gerber_list, &ui_state.active_row_index, LISTBOX_SIZE);
    render_list_box_action_btns(&main_state, &ui_state, LISTBOX_ACTION_BTNS_SIZE);
    render_output_file(&main_state);
    render_main_action_btns(&main_state, MAIN_ACTION_BTNS_SIZE);
    ImGui::End();
}

static void render_title() {
    ImGui::PushFont(g_imgui_view.title_font);
    ImGui::Text("Gerber2PDF");
    ImGui::PopFont();
}

static void render_list_box_action_btns(MainState *main_state, UIState *ui_state, ImVec2 size) {
    ImGui::BeginChild("Actions", ImVec2(0, 40));
    if(ImGui::Button("Add files")) {
        ImGui::OpenPopup("Add gerber files..");
    }
    static std::vector<fs::path> files;
    // this needs to be outside the previous `if`
    if(ImGuiExt::FileChooser("Add gerber files..", &files)) {
        for(auto file: files) {
            main_state->gerber_list.push_back(GerberFile{file.filename().string(), file.string(), true, {0, 0, 0}});
        }
        files.clear();
    }


    ImGui::SameLine();
    if(ImGui::Button("Insert page break")) {
        if(
            ui_state->active_row_index != SIZE_MAX &&
            ui_state->active_row_index < main_state->gerber_list.size() - 1 &&
            !std::holds_alternative<PageBreak>(*(main_state->gerber_list.begin()+ui_state->active_row_index)) &&
            !std::holds_alternative<PageBreak>(*(main_state->gerber_list.begin()+ui_state->active_row_index+1))
        ) {
            main_state->gerber_list.insert(
                main_state->gerber_list.begin()+ui_state->active_row_index+1,
                PageBreak{}
            );
        }
    }
    ImGui::EndChild();
}

static void render_main_action_btns(MainState *main_state, ImVec2 size) {
    ImGui::BeginChild("Main Actions", ImVec2(0, 40));
    if(ImGui::Button("Save to batch file")) {
        if(main_state->output_file.empty()) {
            ImGui::OpenPopup("No output file specified");
        } else {
            std::cout << generate_batch_script(*main_state) << "\nEND\n";
        }
    }

    if(ImGui::BeginPopup("No output file specified")) {
        ImGui::Text("Please select an output file");
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    if(ImGui::Button("Execute")) {
        if(main_state->output_file.empty()) {
            ImGui::OpenPopup("No output file specified");
        } else {
            std::cout << "TODO! Execute\n";
        }
    }
    ImGui::EndChild();
}


static void render_output_file(MainState *main_state) {
    ImGui::Text("Output File: ");
    ImGui::SameLine();
    ImGui::InputText("", &main_state->output_file);
    ImGui::SameLine();
    ImGui::Button("Browse");
    ImGui::NewLine();
}

static void render_page_options(MainState *main_state) {
    ImVec2 COLOR_BTN_SIZE(40, 40);
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth()-200);
    auto im_color = ImColor(main_state->bg_color_rgba[0],
                            main_state->bg_color_rgba[1], 
                            main_state->bg_color_rgba[2], 
                            main_state->bg_color_rgba[3]);
    if(ImGui::ColorButton("Background Color", 
                        im_color,
                        ImGuiColorEditFlags_AlphaPreview,
                        COLOR_BTN_SIZE)) {
        ImGui::OpenPopup("Background Color");
    }
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);
    ImGui::Text("Background Layer");
    if(ImGui::BeginPopup("Background Color")) {
        ImGui::ColorPicker4("Background Color", main_state->bg_color_rgba, ImGuiColorEditFlags_AlphaBar);
        ImGui::EndPopup();
    }
    static const char *page_size_names[3] = {"Page Size: A3", "Page Size: A4", "Page Size: Extents"};
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
    ImGui::BeginChild("Page Size", ImVec2(200, 30));
    ImGui::Combo("", &main_state->page_size, page_size_names, 3);
    ImGui::EndChild();
    ImGui::EndGroup();
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth()-200);
    ImGui::Checkbox("  Stroke to Fills", &main_state->is_stroke2fills);
}