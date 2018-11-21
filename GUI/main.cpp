#include <gtkmm.h>
#include <string>
#include <optional>
#include <variant>
#include <iostream>
#include "logic.hpp"
#include "view.hpp"
#include "gtk_utils.hpp"

void cb_add_seperate(MainState* state, Gtk::Window *parent_window);
void cb_add_combined(MainState* state, Gtk::Window *parent_window);
Gtk::Widget* cb_create_listbox_row(const Glib::RefPtr<GerberListEntry> list_entry);

int main(int argc, char** argv) {
    MainState state;
    auto app = Gtk::Application::create(argc, argv, "happycoder97.Gerber2PdfGui");
    auto main_window = new_main_window();
    auto list_store = ListStoreWrapper<Glib::Value<GerberListEntry>>();
    main_window.add_combined_btn->signal_clicked().connect(
        sigc::bind(
            sigc::ptr_fun(cb_add_combined), 
            &state, 
            main_window.main_window));
    main_window.add_seperate_btn->signal_clicked().connect(
        sigc::bind(
            sigc::ptr_fun(cb_add_seperate), 
            &state, 
            main_window.main_window));
    main_window.gerber_list->bind_list_store(Gtk::make_managed(list_store), sigc::ptr_fun(cb_create_listbox_row));
    app->run(*main_window.main_window);
    return 0;
}


Gtk::Widget* cb_create_listbox_row(const Glib::RefPtr<Glib::Value<GerberListEntry>> list_entry) {
//    if(std::holds_alternative<PageBreak>(*list_entry.get())) {
        auto row = new_page_break_row();
        return Gtk::manage(row.row);
//    }
}

void cb_add_seperate(MainState* state, Gtk::Window* parent_window) {
    std::optional<Glib::RefPtr<Gio::File>> file = open_gerber_file(*parent_window);
    if(file) {
        GerberListEntry file_row = GerberFile {
            file->get()->get_uri(),
            false //is_mirrored
        };
        GerberListEntry page_break = PageBreak {};
        state->add_to_list(file_row);
        state->add_to_list(page_break);
        std::cout << "seperate: " << std::get<GerberFile>(file_row).file_uri << "\n";
    }
}

void cb_add_combined(MainState* state, Gtk::Window *parent_window) {
    std::optional<Glib::RefPtr<Gio::File>> file = open_gerber_file(*parent_window);
    if(file) {
        GerberListEntry file_row = GerberFile {
            file->get()->get_uri(),
            false //is_mirrored
        };
        state->add_to_list(file_row);
        std::cout << "combined: " << std::get<GerberFile>(file_row).file_uri << "\n";
    }
}