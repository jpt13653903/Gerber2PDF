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
        bool okay_to_exit = false;

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
                            // Allow selection of multiple files whe action is OPEN_FILES
                            if(action == FileChooserAction::OPEN_FILES ||
                               selected_rows.empty()) {
                                selected_rows.push_back(row_index);
                                save_file_name = file.filename().string();
                            }
                        }
                    }
                }
                ImGui::ListBoxFooter();
                if(action == FileChooserAction::SAVE_FILE) {
                    ImGui::Text("Save as: ");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(WIDTH - 175);
                    if(ImGui::InputText("##SAVE_FILE_NAME", &save_file_name)) {
                        // clear selection when inputing new file name
                        selected_rows.clear();
                    }
                    ImGui::PopItemWidth();
                    ImGui::SameLine();
                }
            }
            if(action == FileChooserAction::OPEN_FILES) {
                if(ImGui::Button("Open Selected") && !selected_rows.empty()) {
                    for(auto row: selected_rows) {
                        selected_files->push_back(files[row].string());
                    }
                    okay_to_exit = true;
                }
            } else if(action == FileChooserAction::SAVE_FILE) {
                if(ImGui::Button("Save")) {
                    if(save_file_name.empty()) {
                        ImGui::OpenPopup("##NO_FILE_NAME");
                    } else {
                        if(fs::exists(cwd/save_file_name)) {
                            ImGui::OpenPopup("##FILE_EXISTS");
                        } else {
                            selected_files->push_back(cwd/save_file_name);
                            okay_to_exit = true;
                        }
                    }
                }
                if(ImGui::BeginPopup("##NO_FILE_NAME")) {
                    ImGui::Text("Please enter a file name to save.");
                    ImGui::EndPopup();
                }
                if(ImGui::BeginPopup("##FILE_EXISTS")) {
                    ImGui::Text("Are you sure to overwrite this file?");
                    ImGui::Text("File: "); ImGui::SameLine();
                    ImGui::Text(save_file_name.c_str());
                    ImGui::Text("Location: "); ImGui::SameLine();
                    ImGui::Text(cwd.string().c_str());

                    if(ImGui::Button("Yes")) {
                        selected_files->push_back(cwd/save_file_name);
                        okay_to_exit = true;
                    }
                    ImGui::SameLine();
                    if(ImGui::Button("No")) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if(okay_to_exit) ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        return okay_to_exit;
    }
}