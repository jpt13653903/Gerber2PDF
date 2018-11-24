#include "custom_widgets.hpp"
#include <iostream>
#include <cstdio>
#include <boost/variant.hpp>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

/* Colors */
static ImU32 ROW_BG = IM_COL32(0,10,15, 255);
static ImU32 ROW_BG_HOVERED = IM_COL32(0, 68, 102, 255);
static ImU32 ROW_BG_ACTIVE = IM_COL32(0, 114, 171, 255);

static const size_t __BUF_LEN = 100;
static char char_buf[__BUF_LEN];

namespace ImGuiExt {

    enum class RowAction {
        Delete,
        Click,
        Hover,
        None
    };

    enum class RowEffect {
        None,
        Hovered,
        Selected
    };

    static RowAction GerberFileRow(size_t index, GerberFile *entry, RowEffect effect);
    static RowAction PageBreakRow(size_t index, RowEffect effect);

    void GerberListBox(std::vector<GerberListEntry> *gerber_list, size_t* selected_index, ImVec2 size) {
        static size_t hovered_index = SIZE_MAX;
        size_t index = 0;
        ImGui::BeginChild("Gerber List", size, false);
        if(!ImGui::IsWindowHovered()) hovered_index = SIZE_MAX;
        for(GerberListEntry &entry: *gerber_list) {
            RowAction action;
            RowEffect effect = RowEffect::None;
            if(hovered_index == index) effect = RowEffect::Hovered;
            if(*selected_index == index) effect = RowEffect::Selected;

            if(boost::get<GerberFile>(&entry)) {
                action = GerberFileRow(index, boost::get<GerberFile>(&entry), effect);
            } else {
                action = PageBreakRow(index, effect);
            }
            if(action == RowAction::Click) {
                *selected_index = index;
            } else if(action == RowAction::Delete) {
                gerber_list->erase(gerber_list->begin()+index);
                break;
            } else if(action == RowAction::Hover) {
                hovered_index = index;
            }
            if(action != RowAction::Hover && hovered_index == index) {
                hovered_index = SIZE_MAX;
            }
            index++;
        }
        if(gerber_list->empty()) {
            ImGui::Indent(20.0);
            ImGui::NewLine();
            ImGui::Text("Gerber files list is empty.");
            ImGui::Text("Please use the buttons below to add files to the list.");
            ImGui::Unindent(20.0);
        }
        ImGui::EndChild();
    }

    static RowAction GerberFileRow(size_t index, GerberFile *entry, RowEffect effect) {
        ImU32 bg_color = (effect == RowEffect::Hovered)? ROW_BG_HOVERED :
                         (effect == RowEffect::Selected)? ROW_BG_ACTIVE :
                         ROW_BG;

        static auto MARGIN = 5;
        auto ROW_WIDTH = ImGui::GetWindowContentRegionWidth();
        static auto const ROW_HEIGHT = 60;
        auto DELETE_BTN_SIZE = ImVec2(100, 40);
        ImVec2 COLOR_BTN_SIZE(ROW_HEIGHT-2*MARGIN, ROW_HEIGHT-2*MARGIN);
        auto row_action_to_return = RowAction::None;

        auto initial_cursor = ImGui::GetCursorPos();
        ImGui::BeginGroup();
        {
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos(),
                ImGui::GetCursorScreenPos() + ImVec2(ROW_WIDTH, ROW_HEIGHT),
                bg_color
            );
            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(MARGIN, MARGIN));
            { //Color Btn
                std::snprintf(char_buf, __BUF_LEN, "Layer Color: %s", entry->file_name.c_str());
                auto im_color = ImColor(entry->color_rgba[0],
                                        entry->color_rgba[1], 
                                        entry->color_rgba[2], 
                                        entry->color_rgba[3]);
                if(ImGui::ColorButton(char_buf, 
                                    im_color,
                                    ImGuiColorEditFlags_AlphaPreview,
                                    COLOR_BTN_SIZE)) {
                    ImGui::OpenPopup(char_buf);
                }
                if(ImGui::BeginPopup(char_buf)) {
                    ImGui::ColorPicker4(char_buf, entry->color_rgba, ImGuiColorEditFlags_AlphaBar);
                    ImGui::EndPopup();
                }
            }

            ImGui::SameLine();

            ImGui::BeginGroup();
            { // Mid section
               ImGui::Text(entry->file_name.c_str());
               ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
               std::snprintf(char_buf, __BUF_LEN, "Mirrored##%lu", index);
               ImGui::Checkbox(char_buf, &entry->is_mirrored);
            }
            ImGui::EndGroup();

            ImGui::SameLine();
            ImGui::SetCursorPosX(ROW_WIDTH - DELETE_BTN_SIZE.x - MARGIN);
            std::snprintf(char_buf, __BUF_LEN, "Delete##%lu", index);
            if(ImGui::Button(char_buf, DELETE_BTN_SIZE)) {
                row_action_to_return = RowAction::Delete;
            }

        }
        ImGui::EndGroup();
        ImGui::SetCursorPos(initial_cursor + ImVec2(0, ROW_HEIGHT));

        if(row_action_to_return == RowAction::None && 
            ImGui::IsMouseClicked(0) && effect == RowEffect::Hovered) {
            row_action_to_return = RowAction::Click;
        }
        if(row_action_to_return == RowAction::None &&
            ImGui::IsItemHovered()) {
            row_action_to_return = RowAction::Hover;
        }
        return row_action_to_return;
    }

    static RowAction PageBreakRow(size_t index, RowEffect effect) {
        ImU32 bg_color = (effect == RowEffect::Hovered)? ROW_BG_HOVERED :
                         (effect == RowEffect::Selected)? ROW_BG_ACTIVE :
                         ROW_BG;
        static auto MARGIN = 5;
        auto ROW_WIDTH = ImGui::GetWindowWidth();
        auto ROW_HEIGHT = 50;
        auto DELETE_BTN_SIZE = ImVec2(100, 40);
        
        auto row_action_to_return = RowAction::None;
        ImGui::BeginGroup();
        {
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos(),
                ImGui::GetCursorScreenPos() + ImVec2(ROW_WIDTH, ROW_HEIGHT),
                bg_color
            );
            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(MARGIN, MARGIN));
            ImGui::Text("Page Break");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ROW_WIDTH - DELETE_BTN_SIZE.x - MARGIN);
            std::snprintf(char_buf, __BUF_LEN, "Delete##%lu", index);
            if(ImGui::Button(char_buf, DELETE_BTN_SIZE)) {
                row_action_to_return = RowAction::Delete;
            }
        }
        ImGui::EndGroup();
        if(row_action_to_return == RowAction::None &&
            ImGui::IsItemHovered()) {
            row_action_to_return = RowAction::Hover;
        }
        return row_action_to_return;
    }
}