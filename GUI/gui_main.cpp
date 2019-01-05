#include "imgui.h"
#include "imgui_stdlib.h"
#include "gui_main.hpp"
#include "logic.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/variant.hpp>
#include <boost/dll.hpp>
#include <algorithm>
#include <cstring>
#include "custom_widgets.hpp"

namespace fs = boost::filesystem;

/** Roadmap:
 *      [x] Implement file chooser
 *      [x] Implement add files button
 *      [x] ! Refactor Gerber ListBox
 *      [x] ! Refactor Filechooser
 *      [x] Implement Save to batch file button
 *      [ ] Improve modularity
 *      [x] Make it compile in C++14
 *      [x] Try compiling on windows
 *      [x] Fix resize glitch
 *      [ ] Remove spaces around Gerber Listbox
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
    auto prog_path = boost::dll::program_location().parent_path();
    auto font_path = prog_path / "fonts" / "FiraSans-Regular.ttf";
    if(fs::exists(font_path)) {
        // auto font_path_str = fs::canonical(font_path).string();
        // Note on above: fs::canonical breaks on Windows directory junctions
        auto font_path_str = font_path.string();
        g_imgui_view.normal_font = io.Fonts->AddFontFromFileTTF(font_path_str.c_str(), 14.0f);
        g_imgui_view.title_font = io.Fonts->AddFontFromFileTTF(font_path_str.c_str(), 36.0f);
    } else {
        std::cout << "Warning: Couldn't load fonts. The UI may appear ugly. \n";
        std::cout << "   " << font_path << " not found\n";
    }

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
    auto LISTBOX_SIZE = ImGui::GetWindowSize() - LISTBOX_ACTION_BTNS_SIZE - MAIN_ACTION_BTNS_SIZE - ImVec2(25.0, 200);

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
    if(ImGuiExt::FileChooser("Add gerber files..", ImGuiExt::FileChooserAction::OPEN_FILES, &files)) {
        for(auto file: files) {
            // Fix `C:/` forward slash in windows
            file.make_preferred();  
            main_state->gerber_list.push_back(GerberFile{file.filename().string(), file.string(), false, {0, 0, 0, 1}});
        }
        files.clear();
    }


    ImGui::SameLine();
    if(ImGui::Button("Insert page break")) {
        auto insert_after = ui_state->active_row_index != SIZE_MAX ? 
                            ui_state->active_row_index :
                            main_state->gerber_list.size() - 1;
        if(
            // currently selected item is not a page break
            // doesn't make sense to add two consecutive page breaks
            (
                !main_state->gerber_list.empty() &&
                !boost::get<PageBreak>(&*(main_state->gerber_list.begin()+insert_after))
            ) &&
            // there is no item after, or it is not a pagebreak
            (
                insert_after == (main_state->gerber_list.size() - 1) ||
                !boost::get<PageBreak>(&*(main_state->gerber_list.begin()+insert_after+1))
            )
        ) {
            main_state->gerber_list.insert(
                main_state->gerber_list.begin()+insert_after+1,
                PageBreak{}
            );
        }
    }
    ImGui::EndChild();
}

static void render_main_action_btns(MainState *main_state, ImVec2 size) {
    ImGui::BeginChild("##MAIN_ACTIONS", ImVec2(0, 40));
    if(ImGui::Button("Save to batch file")) {
        if(main_state->output_file.empty()) {
            main_state->output_file = std::string("output.pdf");
        } 
        ImGui::OpenPopup("##SAVE_BATCH_FILE");
    }

    static std::vector<fs::path> batch_file;
    if(ImGuiExt::FileChooser("##SAVE_BATCH_FILE", ImGuiExt::FileChooserAction::SAVE_FILE, &batch_file)) {
        auto batch_content = generate_batch_script(*main_state);
        std::ofstream ofstream{batch_file[0].string().c_str()};
        ofstream << batch_content;
        ofstream.close();
        batch_file.clear();
        ImGui::OpenPopup("##BATCH_FILE_SAVED");
    }

    if(ImGui::BeginPopup("##NO_OUTPUT_FILE")) {
        ImGui::Text("Please select an output file");
        ImGui::EndPopup();
    }

    if(ImGui::BeginPopup("##BATCH_FILE_SAVED")) {
        ImGui::Text("Batch file saved successfully");
        ImGui::EndPopup();
    }

    ImGui::SameLine();
    if(ImGui::Button("Execute")) {
        if(main_state->output_file.empty()) {
            ImGui::OpenPopup("##NO_OUTPUT_FILE");
        } else {
            execute_gerber2pdf(*main_state);
        }
    }
    ImGui::EndChild();
}


static void render_output_file(MainState *main_state) {
    ImGui::Text("Output File: ");
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 140);
    ImGui::InputText("", &main_state->output_file);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if(ImGui::Button("Browse##OUTPUT_FILE")) {
        ImGui::OpenPopup("###FILE_CHOOSER_OUTPUT_FILE");
    } 
    static std::vector<fs::path> selected_file;
    if(ImGuiExt::FileChooser("Browse Output File###FILE_CHOOSER_OUTPUT_FILE", ImGuiExt::FileChooserAction::SAVE_FILE, &selected_file)) {
        main_state->output_file = selected_file[0].string();
        selected_file.clear();
    }
    ImGui::NewLine();
}

static bool page_size_to_string_cb(void *data, int index, const char **text) {
    *text = page_size_to_string(static_cast<PageSize>(index));
    return true;
}

static void render_page_options(MainState *main_state) {
    ImVec2 COLOR_BTN_SIZE(40, 40);
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth()-200);
    ImGui::BeginGroup();
    ImGui::Text("Background Layer");
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
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
    if(ImGui::BeginPopup("Background Color")) {
        ImGui::ColorPicker4("Background Color", main_state->bg_color_rgba, ImGuiColorEditFlags_AlphaBar);
        ImGui::EndPopup();
    }
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
    ImGui::Text("Page size "); ImGui::SameLine();
    ImGui::PushItemWidth(100);
    ImGui::Combo("##PAGE_SIZE", &main_state->page_size, page_size_to_string_cb, NULL, 5);
    ImGui::PopItemWidth();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
    ImGui::Checkbox(" Stroke to Fills", &main_state->is_stroke2fills);
    ImGui::EndGroup();
    ImGui::EndGroup();
}
