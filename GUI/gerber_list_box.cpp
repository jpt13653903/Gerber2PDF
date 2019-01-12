#include "custom_widgets.hpp"
#include "imgui_utils.hpp"
#include <iostream>
#include <cstdio>
#include <boost/variant.hpp>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

/* Colors */
static ImU32 TITLE_BAR_BG = IM_COL32(26,101,161, 255);
static ImU32 ROW_BG = IM_COL32(0,10,15, 255);
static ImU32 ROW_BG_HOVERED = IM_COL32(0, 68, 102, 255);
static ImU32 ROW_BG_ACTIVE = IM_COL32(0, 114, 171, 255);

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

        auto draw_list = ImGui::GetWindowDrawList();
        draw_list->ChannelsSplit(2);      // Split into two layers (channels)

        // Draw title
        {
            auto start_cur = ImGui::GetCursorScreenPos();
            ImGuiExt::MoveCursor(2, 2);
            draw_list->ChannelsSetCurrent(1); // Switch to foreground channel
            ImGui::TextUnformatted("Gerber List");
            draw_list->ChannelsSetCurrent(0); // Switch to background channel
            auto end_cur = ImVec2(start_cur.x+size.x, ImGui::GetCursorScreenPos().y);
            draw_list->AddRectFilled(start_cur, end_cur, TITLE_BAR_BG);
        }


        // Draw list content
        auto start_cur = ImGui::GetCursorScreenPos();
        draw_list->ChannelsSetCurrent(1); // Switch to foreground channel
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

        draw_list->ChannelsSetCurrent(0); // Switch to BG Channel
        draw_list->AddRectFilled(ImVec2(0, 0) + start_cur, size + start_cur, IM_COL32_BLACK);
        draw_list->ChannelsMerge();

        ImGui::EndChild();
    }
    // ? Remove effect parameter by using multiple BeginGroup with Same ID?
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
        ImGui::PushID(index);
        ImGui::BeginGroup();
        {
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImGui::GetCursorScreenPos(),
                ImGui::GetCursorScreenPos() + ImVec2(ROW_WIDTH, ROW_HEIGHT),
                bg_color
            );
            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(MARGIN, MARGIN));
            { //Color Btn
                std::string layer_color_label = std::string("Layer Color: ") + entry->file_name;
                auto im_color = ImColor(entry->color_rgba[0],
                                        entry->color_rgba[1], 
                                        entry->color_rgba[2], 
                                        entry->color_rgba[3]);
                if(ImGui::ColorButton(entry->file_name.c_str(), 
                                    im_color,
                                    ImGuiColorEditFlags_AlphaPreview,
                                    COLOR_BTN_SIZE)) {
                    ImGui::OpenPopup("##LAYER_COLOR");
                }
                if(ImGui::BeginPopup("##LAYER_COLOR")) {
                    ImGui::ColorPicker4(layer_color_label.c_str(), entry->color_rgba, ImGuiColorEditFlags_AlphaBar);
                    ImGui::EndPopup();
                }
            }

            ImGui::SameLine();

            ImGui::BeginGroup();
            { // Mid section
               ImGui::Text(entry->file_name.c_str());
               ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
               ImGui::Checkbox("Mirrored", &entry->is_mirrored);
               ImGui::SameLine();
               ImGui::Checkbox("Strokes to Fills", &entry->strokes_to_fills);
            }
            ImGui::EndGroup();

            ImGui::SameLine();
            ImGui::SetCursorPosX(ROW_WIDTH - DELETE_BTN_SIZE.x - MARGIN);
            if(ImGui::Button("Delete", DELETE_BTN_SIZE)) {
                row_action_to_return = RowAction::Delete;
            }

        }
        ImGui::EndGroup();
        ImGui::PopID();
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
        ImGui::PushID(index);
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
            if(ImGui::Button("Delete", DELETE_BTN_SIZE)) {
                row_action_to_return = RowAction::Delete;
            }
        }
        ImGui::EndGroup();
        ImGui::PopID();
        if(row_action_to_return == RowAction::None &&
            ImGui::IsItemHovered()) {
            row_action_to_return = RowAction::Hover;
        }
        return row_action_to_return;
    }
}