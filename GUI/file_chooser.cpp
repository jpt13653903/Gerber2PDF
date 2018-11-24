#include "custom_widgets.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace ImGuiExt {
    bool FileChooser(const char *popup_id, FileChooserAction action, std::vector<fs::path> *selected_files) {
        static auto cwd = fs::current_path();
        static std::string cwd_str = cwd.string();
        static std::string save_file_name;

        if(ImGui::BeginPopupModal(popup_id)) {
            auto WIDTH = ImGui::GetWindowWidth();
            auto HEIGHT = ImGui::GetWindowHeight();
            if(HEIGHT<400) {
                HEIGHT = 400;
                ImGui::SetWindowSize(ImVec2(WIDTH, HEIGHT));
            } 
            if(WIDTH<400) {
                WIDTH = 400;
                ImGui::SetWindowSize(ImVec2(WIDTH, HEIGHT));
            } 
            ImGui::PushItemWidth(WIDTH-15);
            if(ImGui::InputText("##CWD", &cwd_str, ImGuiInputTextFlags_AutoSelectAll)) {
                if(fs::is_directory(fs::path(cwd_str.c_str()))) {
                    cwd = fs::path(cwd_str.c_str());
                }
            }
            ImGui::PopItemWidth();
            fs::directory_iterator __dir_iter_end;
            static std::vector<int> selected_rows;
            std::vector<fs::path> files;
            files.push_back(cwd/fs::path(".."));
            files.insert(files.begin()+1, fs::directory_iterator(cwd), __dir_iter_end);
            if(ImGui::ListBoxHeader("##Listbox", ImVec2(WIDTH-15, HEIGHT - 100))) {
                //Iterate over dirs
                int row_index = -1;
                for(auto file: files) {
                    row_index++;
                    if(!fs::is_directory(file)) continue;
                    if(ImGui::Selectable((file.filename().string()+"/").c_str(), false)) {
                        cwd = files[row_index].lexically_normal();
                    }
                }
                if(action == FileChooserAction::OPEN_FILES) {
                    //Iterate over files
                    row_index = -1;
                    for(auto file: files) {
                        row_index++;
                        if(fs::is_directory(file)) continue;
                        bool is_row_selected = 
                            std::find(selected_rows.begin(), selected_rows.end(), row_index) != selected_rows.end();
                        if(ImGui::Selectable(file.filename().string().c_str(), is_row_selected)) {
                            if(is_row_selected) {
                                selected_rows.erase(std::find(selected_rows.begin(), selected_rows.end(), row_index));
                            } else {
                                selected_rows.push_back(row_index);
                            }
                        }
                    }
                    ImGui::ListBoxFooter();
                } else if(action == FileChooserAction::SAVE_FILE) {
                    ImGui::ListBoxFooter();
                    ImGui::Text("Save as: ");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(WIDTH - 175);
                    ImGui::InputText("##Save file name", &save_file_name);
                    ImGui::PopItemWidth();
                    ImGui::SameLine();
                }
            }
            if(action == FileChooserAction::OPEN_FILES) {
                if(ImGui::Button("Open Selected") && !selected_rows.empty()) {
                    ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                    for(auto row: selected_rows) {
                        selected_files->push_back(files[row].string());
                    }
                    return true;
                }
            } else if(action == FileChooserAction::SAVE_FILE) {
                if(ImGui::Button("Save")) {
                    if(save_file_name.empty()) {
                        ImGui::OpenPopup("Please enter a filename");
                    } else {
                        ImGui::CloseCurrentPopup();
                        ImGui::EndPopup();
                        selected_files->push_back(cwd/save_file_name);
                        return true;
                    }
                }
                if(ImGui::BeginPopup("Please enter a filename")) {
                    ImGui::Text("Please enter a file name to save.");
                    ImGui::EndPopup();
                }
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return false;
            }
            ImGui::SameLine();
            ImGui::EndPopup();
        }
        return false;
    }
}