#include "logic.hpp"

void MainState::add_to_list(GerberListEntry&& list_entry) {
    this->gerber_list.push_back(list_entry);
    if(this->cb_list_updated) {
        (*cb_list_updated) (-1, list_entry);
    }
}

void MainState::remove_from_list(int index) {
    if(this->cb_list_updated) {
        (*cb_list_updated) (index, {});
    }
    this->gerber_list.erase(this->gerber_list.begin()+index);
}

void MainState::update_list_at(int index, GerberListEntry&& list_entry) {
    if(this->cb_list_updated) {
        (*cb_list_updated) (index, std::make_optional(list_entry));
    }
    this->gerber_list[index] = list_entry;
}
void MainState::set_cb_list_updated(std::optional<CBListUpdated> cb_list_updated) {
    this->cb_list_updated = cb_list_updated;
}

const std::vector<GerberListEntry> MainState::get_gerber_list() {
    return this->gerber_list;
}