#include "logic.hpp"
#include <cstring>
#include <sstream>

// TODO: Provide an implementation for linux shell scripts.

static void pretty_line(std::ostream &ostream, const char * line,
                        int indent_level, int line_max_len) {
    for(int i = 0; i<=indent_level; i++) {
        ostream << "  ";
    }
    ostream << line;
    int line_len = std::strlen(line);
    int pad_len = line_max_len - line_len;
    for(int i = 0; i<pad_len; i++)
        ostream << " ";
    ostream << "\n^";
}

std::string generate_batch_script(const MainState &state) {
    static const int LINE_LEN = 80;
    std::stringstream out_string;
    pretty_line(out_string, "Gerber2pdf", 0, LINE_LEN);
    pretty_line(out_string, 
        (std::string("output='") + state.output_file + std::string("'")).c_str(),
        1, LINE_LEN);

    switch(static_cast<PageSize>(state.page_size)) {
        case PageSize::TIGHT:
            break;
        case PageSize::A4:
            pretty_line(out_string, "-page_size=A4", 1, LINE_LEN);
            break;
        case PageSize::A3:
            pretty_line(out_string, "-page_size=A3", 1, LINE_LEN);
            break;
        case PageSize::EXTENTS:
            pretty_line(out_string, "-page_size=extents", 1, LINE_LEN);
            break;
        case PageSize::LETTER:
            pretty_line(out_string, "-page_size=letter", 1, LINE_LEN);
            break;
    }

    pretty_line(out_string, "-combine", 1, LINE_LEN);
    for(auto &entry: state.gerber_list) {
        if(const GerberFile *file_entry = boost::get<GerberFile>(&entry)) {
            std::ostringstream line;
            if(file_entry->is_mirrored) {
                line << "-mirror ";
            } else {
                line << "-nomirror ";
            }
            line << "-color=" << (int) (file_entry->color_rgba[0]*255) <<","
                              << (int) (file_entry->color_rgba[1]*255) <<","
                              << (int) (file_entry->color_rgba[2]*255) <<","
                              << (int) (file_entry->color_rgba[3]*255) <<" ";

            line << file_entry->file_uri;
            pretty_line(out_string, line.str().c_str(), 2, LINE_LEN);
        } else {
            pretty_line(out_string, "-combine", 1, LINE_LEN);
        }
    }
    return out_string.str();
}


const char * page_size_to_string(PageSize page_size) {
    switch(page_size) {
        case PageSize::TIGHT:
            return "Tight";
        case PageSize::A3:
            return "A3";
        case PageSize::A4:
            return "A4";
        case PageSize::LETTER:
            return "Letter";
        case PageSize::EXTENTS:
            return "Extents";
    }
    return "Invalid Page Size";
}