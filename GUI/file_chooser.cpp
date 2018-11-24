#include "custom_widgets.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace ImGuiExt {
    bool FileChooser(const char *popup_id, std::vector<fs::path> *selected_files) {
        static auto cwd = fs::current_path();
        static bool show_gbr_files_only = true;
        static std::string cwd_str = cwd.string();

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
            if(ImGui::InputText("", &cwd_str, ImGuiInputTextFlags_AutoSelectAll)) {
                if(fs::is_directory(fs::path(cwd_str.c_str()))) {
                    cwd = fs::path(cwd_str.c_str());
                }
            }
            fs::directory_iterator __dir_iter_end;
            static std::vector<int> selected_rows;
            std::vector<fs::path> files;
            files.push_back(cwd/fs::path(".."));
            files.insert(files.begin()+1, fs::directory_iterator(cwd), __dir_iter_end);
            if(ImGui::ListBoxHeader("Listbox", ImVec2(WIDTH-10, HEIGHT - 100))) {
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
                    if(ImGui::Selectable(file.filename().string().c_str(), is_row_selected)) {
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
                for(auto row: selected_rows) {
                    selected_files->push_back(files[row].string());
                }
                return true;
            }
            ImGui::SameLine();
            if(ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
                return false;
            }
            ImGui::SameLine();
            ImGui::Checkbox("Show *.gbr only", &show_gbr_files_only);
            ImGui::EndPopup();
        }
        return false;
    }
}