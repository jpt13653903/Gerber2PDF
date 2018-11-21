#if !defined(__VIEW_HPP__)
#define __VIEW_HPP__

#include <gtkmm.h>

struct GerberFileRow {
    Glib::RefPtr<Gtk::Builder> __builder;
    Gtk::Container* row;
    Gtk::ColorButton* color_btn;
    Gtk::Label* file_label;
    Gtk::CheckButton* mirror_checkbox;
    Gtk::Button* delete_btn;
};

struct PageBreakRow {
    Glib::RefPtr<Gtk::Builder> __builder;
    Gtk::Container* row;
};

struct MainWindow {
    Glib::RefPtr<Gtk::Builder> __builder;
    Gtk::Window* main_window;
    Gtk::Button* add_seperate_btn;
    Gtk::Button* add_combined_btn;
    Gtk::ListBox* gerber_list;
    Gtk::Button* execute_btn;
    Gtk::Button* save_to_batch_btn;
};


MainWindow new_main_window();
GerberFileRow new_gerber_file_row();
PageBreakRow new_page_break_row();
std::optional<Glib::RefPtr<Gio::File>> open_gerber_file(Gtk::Window &parent_window);

#endif // __VIEW_HPP__