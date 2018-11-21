#include "view.hpp"

MainWindow new_main_window() {
    MainWindow main_window;
    auto ref_builder = Gtk::Builder::create_from_file("glade/ui.glade");
    main_window.__builder = ref_builder;
    ref_builder->get_widget("main_window", main_window.main_window);
    ref_builder->get_widget("add_seperate_btn", main_window.add_seperate_btn);
    ref_builder->get_widget("add_combined_btn", main_window.add_combined_btn);
    ref_builder->get_widget("gerber_list", main_window.gerber_list);
    ref_builder->get_widget("execute_btn", main_window.execute_btn);
    ref_builder->get_widget("save_to_batch_btn", main_window.save_to_batch_btn);
    return main_window;
}

GerberFileRow new_gerber_file_row() {
    auto ref_builder = Gtk::Builder::create_from_file("glade/gerber_row.glade");
    GerberFileRow file_row;
    file_row.__builder = ref_builder;
    ref_builder->get_widget("gerber_row", file_row.row);
    ref_builder->get_widget("color_btn", file_row.color_btn);
    ref_builder->get_widget("mirror_checkbox", file_row.mirror_checkbox);
    ref_builder->get_widget("delete_btn", file_row.delete_btn);
    return file_row;
}

PageBreakRow new_page_break_row() {
    auto ref_builder = Gtk::Builder::create_from_file("glade/page_break.glade");
    PageBreakRow pb_row;
    pb_row.__builder = ref_builder;
    ref_builder->get_widget("page_break_row", pb_row.row);
    return pb_row;
}

std::optional<Glib::RefPtr<Gio::File>> open_gerber_file(Gtk::Window &parent_window) {
    auto file_chooser_dialog = Gtk::FileChooserDialog(parent_window, 
                                                      Glib::ustring("Open gerber file"),
                                                      Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN);
    file_chooser_dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    file_chooser_dialog.add_button("_Open", Gtk::RESPONSE_ACCEPT);
    auto response = file_chooser_dialog.run();
    if(response == Gtk::RESPONSE_ACCEPT) {
        return file_chooser_dialog.get_file();
    }
    return {};
}