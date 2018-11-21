#include "imgui.h"
#include "gui_main.hpp"
#include "logic.hpp"
#include <iostream>
#include <sstream>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

struct UIState {
    size_t active_row_index;
};

static void render_title();
static void render_list_box(MainState *main_state, UIState *ui_state, ImVec2 size);
static bool render_list_box_gerber_file(MainState *main_state, 
                                        int index, GerberFile* entry, bool is_row_active);
static bool render_list_box_page_break(MainState *main_state, 
                                        int index, bool is_row_active);
static void render_list_box_action_btns(MainState *main_state, ImVec2 size);
static void render_main_action_btns(MainState *main_stat, ImVec2 size);

static ImGuiView g_imgui_view;
static MainState main_state;
static UIState ui_state;

void gui_setup(int argc, char **argv) {
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowRounding = 0.0f;
    // ImGui::GetStyle().WindowMinSize = ImVec2(300, 500);
    auto io = ImGui::GetIO();
    g_imgui_view.normal_font = io.Fonts->AddFontFromFileTTF("fonts/FiraSans-Regular.ttf", 16.0f);
    g_imgui_view.title_font = io.Fonts->AddFontFromFileTTF("fonts/FiraSans-Regular.ttf", 36.0f);
    main_state.gerber_list.push_back(GerberFile{"File.gbr", true, {155, 205, 255}});
    main_state.gerber_list.push_back(GerberFile{"File1.gbr", false, {155, 205, 255}});
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
                    ImGuiWindowFlags_NoMove|
                    ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::SetCursorPosX(ImGui::GetCursorPosX()+20);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY()+20);
    render_title();


    static auto LISTBOX_ACTION_BTNS_SIZE = ImVec2(0.0, 40);
    static auto MAIN_ACTION_BTNS_SIZE = ImVec2(0.0, 40);
    auto LISTBOX_SIZE = ImGui::GetWindowSize() - LISTBOX_ACTION_BTNS_SIZE - MAIN_ACTION_BTNS_SIZE - ImVec2(10.0, 120);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY()+20);
    render_list_box(&main_state, &ui_state, LISTBOX_SIZE);
    render_list_box_action_btns(&main_state, LISTBOX_ACTION_BTNS_SIZE);
    render_main_action_btns(&main_state, MAIN_ACTION_BTNS_SIZE);
    ImGui::End();
}

static void render_title() {
    ImGui::PushFont(g_imgui_view.title_font);
    ImGui::Text("Gerber2PDF");
    ImGui::PopFont();
}

static void render_list_box(MainState *main_state, UIState *ui_state, ImVec2 size) {
    // Default padding setting is saved and 
    // restored at the end of this function.
    auto saved_padding = ImGui::GetStyle().WindowPadding;
    ImGui::GetStyle().WindowPadding = ImVec2(0, 0);

    ImGui::BeginChild("gerber_list", size, true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    for(size_t i = 0; i<(main_state->gerber_list.size()); i++) {
        auto entry = &main_state->gerber_list[i];
        if(std::holds_alternative<GerberFile>(*entry)) {
            if(render_list_box_gerber_file(main_state, 
                i, 
                std::get_if<GerberFile>(entry),
                ui_state->active_row_index == i) ) {
                ui_state->active_row_index = i;
            }
        } else {
            if(render_list_box_page_break(main_state, i, ui_state->active_row_index == i) ) {
                ui_state->active_row_index = i;
            }
        }
    }
    ImGui::EndChild();

    ImGui::GetStyle().WindowPadding = saved_padding;
}
static bool render_list_box_gerber_file(MainState *main_state, int index, GerberFile* entry, bool is_row_active) {
    int SPACING = 5;
    auto ROW_SIZE = ImVec2(ImGui::GetWindowWidth(), 70);
    auto DELETE_BTN_SIZE = ImVec2(100, 30);
    auto COLOR_BTN_SIZE = ImVec2(ROW_SIZE.y, ROW_SIZE.y) - ImVec2(SPACING*2, SPACING*2);
    auto MID_SIZE = ROW_SIZE - ImVec2(COLOR_BTN_SIZE.x + DELETE_BTN_SIZE.x + SPACING*5, 0);
    static ImU32 ROW_BG = IM_COL32(0,0,0, 100);
    static ImU32 ROW_BG_HOVERED = IM_COL32(0,0,0, 255);
    static ImU32 ROW_BG_ACTIVE = IM_COL32(255,255,255, 100);

    std::ostringstream row_id;
    row_id << "Row_"<<index;
    ImGui::BeginChild(row_id.str().c_str(), ROW_SIZE, false, ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoScrollWithMouse);
    auto is_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
    auto is_clicked = is_hovered && ImGui::IsMouseClicked(0);

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ROW_SIZE,
        is_row_active?ROW_BG_ACTIVE:is_hovered?ROW_BG_HOVERED:ROW_BG
    );

    // topleft padding
    ImGui::SetCursorPosX(ImGui::GetCursorPosX()+SPACING);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY()+SPACING);

    std::ostringstream color_btn_id;
    color_btn_id << "Layer " << index << " Color";
    std::ostringstream color_popup_id;
    color_popup_id << "Layer " << index << " Color";

    auto im_color = ImColor(entry->color_rgba[0],
                            entry->color_rgba[1], 
                            entry->color_rgba[2], 
                            entry->color_rgba[3]);
    if(ImGui::ColorButton(color_btn_id.str().c_str(), im_color, ImGuiColorEditFlags_AlphaPreview, COLOR_BTN_SIZE)) {
        ImGui::OpenPopup(color_popup_id.str().c_str());
    }
    ImGui::SameLine();
    ImGui::BeginChild("", MID_SIZE, false, ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Text(entry->file_uri.c_str());
        ImGui::SetCursorPosY(ImGui::GetCursorPosY()+SPACING);
        ImGui::Checkbox("Mirrored", &entry->is_mirrored);
        ImGui::EndChild();
    ImGui::SameLine();

    if(ImGui::BeginPopup(color_popup_id.str().c_str())) {
        ImGui::ColorPicker4("Layer color", entry->color_rgba, ImGuiColorEditFlags_AlphaBar);
        ImGui::EndPopup();
    }

    if(ImGui::Button("Delete", DELETE_BTN_SIZE)) {
        main_state->gerber_list.erase(main_state->gerber_list.begin()+index);
    }

    ImGui::EndChild();
    return is_clicked;
}

static bool render_list_box_page_break(MainState *main_state, int index, bool is_row_active) {
    auto ROW_SIZE = ImVec2(ImGui::GetWindowWidth(), 40);
    auto TEXT_SIZE = ImGui::CalcTextSize("Page Break");
    auto DELETE_BTN_SIZE = ImVec2(100, 30);
    static ImU32 ROW_BG = IM_COL32(0,0,0, 100);
    static ImU32 ROW_BG_HOVERED = IM_COL32(0,0,0, 255);
    static ImU32 ROW_BG_ACTIVE = IM_COL32(255,255,255, 100);

    std::ostringstream row_id;
    row_id << "Row_"<<index;
    ImGui::BeginChild(row_id.str().c_str(), ROW_SIZE, false, ImGuiWindowFlags_NoDecoration);
    auto is_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
    auto is_clicked = is_hovered && ImGui::IsMouseClicked(0);

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImGui::GetCursorScreenPos() + ROW_SIZE,
        is_row_active?ROW_BG_ACTIVE:is_hovered?ROW_BG_HOVERED:ROW_BG
    );

    auto cursor = ROW_SIZE-TEXT_SIZE;
    cursor.x/=2;
    cursor.y/=2;
    ImGui::SetCursorPos(cursor);
    ImGui::Text("Page Break");

    ImGui::SameLine();    
    ImGui::SetCursorPos(ROW_SIZE-DELETE_BTN_SIZE - ImVec2(5,5));
    if(ImGui::Button("Delete", DELETE_BTN_SIZE)) {
        main_state->gerber_list.erase(main_state->gerber_list.begin()+index);
    }
    ImGui::EndChild();
    return is_clicked;
}

static void render_list_box_action_btns(MainState *main_state, ImVec2 size) {
    ImGui::BeginChild("Actions", ImVec2(0, 40));
    ImGui::Button("Add files");
    ImGui::SameLine();
    ImGui::Button("Insert page break");
    ImGui::EndChild();
}

static void render_main_action_btns(MainState *main_stat, ImVec2 size) {
    ImGui::BeginChild("Main Actions", ImVec2(0, 40));
    ImGui::Button("Save to batch file");
    ImGui::SameLine();
    ImGui::Button("Execute");
    ImGui::EndChild();
}