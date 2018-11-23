#include <vector>
#include <string>
#include <optional>
#include "logic.hpp"
#include "imgui.h"



namespace ImGuiExt {
    void GerberListBox(std::vector<GerberListEntry> *gerber_list, 
                       size_t *selected_index,
                       ImVec2 size);
    /**
     * Returns true just after it is closed.
     * `selected_files` vector is never cleared by this function.
     */
    bool FileChooser(const char *popup_id, std::vector<std::string> *selected_files);
}