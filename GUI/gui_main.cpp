#include "imgui.h"
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
 *      [ ] ! Refactor Gerber ListBox .
 *      [ ] ! Refactor Filechooser into a class.
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
static void render_main_action_btns(MainState *main_stat, ImVec2 size);
static std::optional<std::vector<fs::path>> open_file_chooser();

static ImGuiView g_imgui_view;
static MainState main_state;
static UIState ui_state;

void gui_setup(int argc, char **argv) {
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowRounding = 0.0f;
    auto io = ImGui::GetIO();
    g_imgui_view.normal_font = io.Fonts->AddFontFromFileTTF("fonts/FiraSans-Regular.ttf", 16.0f);
    g_imgui_view.title_font = io.Fonts->AddFontFromFileTTF("fonts/FiraSans-Regular.ttf", 36.0f);
    main_state.gerber_list.push_back(GerberFile{"File.gbr", "file://File.gbr", true, {155, 205, 255}});
    main_state.gerber_list.push_back(GerberFile{"File1.gbr", "file://File1.gbr", false, {155, 205, 255}});
    main_state.gerber_list.push_back(PageBreak{});
    main_state.gerber_list.push_back(GerberFile{"File3.gbr", "file://File.gbr", true, {155, 205, 255}});
    main_state.gerber_list.push_back(GerberFile{"File4.gbr", "file://File1.gbr", false, {155, 205, 255}});
    main_state.gerber_list.push_back(PageBreak{});
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

    static auto LISTBOX_ACTION_BTNS_SIZE = ImVec2(0.0, 40);
    static auto MAIN_ACTION_BTNS_SIZE = ImVec2(0.0, 40);
    auto LISTBOX_SIZE = ImGui::GetWindowSize() - LISTBOX_ACTION_BTNS_SIZE - MAIN_ACTION_BTNS_SIZE - ImVec2(40.0, 120);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY()+20);
    ImGuiExt::GerberListBox(&main_state.gerber_list, &ui_state.active_row_index, LISTBOX_SIZE);
    render_list_box_action_btns(&main_state, &ui_state, LISTBOX_ACTION_BTNS_SIZE);
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
        ImGui::OpenPopup("File Chooser");
    }

    // this needs to be outside the previous `if`
    if(auto files = open_file_chooser()) {
        for(auto file: files.value()) {
            main_state->gerber_list.push_back(GerberFile{file.filename().string(), file.string(), true, {0, 0, 0}});
        }
    }


    ImGui::SameLine();
    if(ImGui::Button("Insert page break")) {
        if(
            ui_state->active_row_index != -1 &&
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

static void render_main_action_btns(MainState *main_stat, ImVec2 size) {
    ImGui::BeginChild("Main Actions", ImVec2(0, 40));
    ImGui::Button("Save to batch file");
    ImGui::SameLine();
    ImGui::Button("Execute");
    ImGui::EndChild();
}

static std::optional<std::vector<fs::path>> open_file_chooser() {
    static auto cwd = fs::current_path();
    static bool show_gbr_files_only = true;

    if(ImGui::BeginPopupModal("File Chooser")) {
        ImGui::Text(cwd.c_str());
        fs::directory_iterator __dir_iter_end;
        static std::vector<int> selected_rows;
        std::vector<fs::path> files;
        files.push_back(cwd/fs::path(".."));
        files.insert(files.begin()+1, fs::directory_iterator(cwd), __dir_iter_end);
        if(ImGui::ListBoxHeader("Listbox")) {
            //Iterate over dirs
            int row_index = -1;
            for(auto file: files) {
                row_index++;
                if(!fs::is_directory(file)) continue;
                if(ImGui::Selectable((file.filename().string()+"/").c_str(), false)) {
                    cwd = files[row_index].lexically_normal();
                }
            }
            //Iterate over files
            row_index = -1;
            for(auto file: files) {
                row_index++;
                if(fs::is_directory(file)) continue;
                if(show_gbr_files_only &&
                    file.extension() != std::string(".gbr")) continue;
                bool is_row_selected = 
                    std::find(selected_rows.begin(), selected_rows.end(), row_index) != selected_rows.end();
                if(ImGui::Selectable(file.filename().c_str(), is_row_selected)) {
                    if(is_row_selected) {
                        selected_rows.erase(std::find(selected_rows.begin(), selected_rows.end(), row_index));
                    } else {
                        selected_rows.push_back(row_index);
                    }
                }
            }
            ImGui::ListBoxFooter();
        }
        if(ImGui::Button("Open Selected") && !selected_rows.empty()) {
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            std::vector<fs::path> selected_files;
            for(auto row: selected_rows) {
                selected_files.push_back(files[row].string());
            }
            return selected_files;
        }
        ImGui::SameLine();
        ImGui::Checkbox("Show *.gbr only", &show_gbr_files_only);
        ImGui::EndPopup();
    }
    return {};
}