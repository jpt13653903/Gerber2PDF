#include <vector>
#include <string>
#include "logic.hpp"
#include "imgui.h"
#include <boost/filesystem.hpp>



namespace ImGuiExt {
    void GerberListBox(std::vector<GerberListEntry> *gerber_list, 
                       size_t *selected_index,
                       ImVec2 size);

    enum class FileChooserAction {
        OPEN_FILES, SAVE_FILE
    };
    /**
     * Returns true just after it is closed.
     * `selected_files` vector is never cleared by this function.
     * File chooser is implemented as a `Popup`. 
     * Use `ImGui::OpenPopup(popup_id)` or `ImGui::OpenPopupOnItemClick()`
     * to open the popup.
     */
    bool FileChooser(const char *popup_id, FileChooserAction action, std::vector<boost::filesystem::path> *selected_files);
}