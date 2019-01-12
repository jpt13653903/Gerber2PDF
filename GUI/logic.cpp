#include "logic.hpp"
#include "Engine.h"
#include <boost/variant.hpp>
#include <cstring>
#include <sstream>

static void pretty_line(std::ostream &ostream, const char * line,
                        int indent_level, int line_max_len, int newline=true) {
    for(int i = 0; i<indent_level; i++) {
        ostream << "  ";
    }
    ostream << line;
    int line_len = std::strlen(line);
    int pad_len = line_max_len - line_len - indent_level*2;
    for(int i = 0; i<pad_len; i++)
        ostream << " ";
    if(newline)
        ostream << " ^\n";
}

// TODO: Provide an implementation for linux shell scripts.
std::string generate_batch_script(const MainState &state) {
    static const int LINE_LEN = 80;
    std::stringstream out_string;
    pretty_line(out_string, "Gerber2pdf", 0, LINE_LEN);
    pretty_line(out_string, 
        (std::string("-output=\"") + state.output_file + std::string("\"")).c_str(),
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
            line << "-colour=" << (int) (file_entry->color_rgba[0]*255) <<","
                              << (int) (file_entry->color_rgba[1]*255) <<","
                              << (int) (file_entry->color_rgba[2]*255) <<","
                              << (int) (file_entry->color_rgba[3]*255) <<" ";

            line << '"' << file_entry->file_uri << '"';
            pretty_line(
                out_string,         // ostream
                line.str().c_str(), // string to print
                2,                  // indent level
                LINE_LEN,           // length of a line to pad
                // whether to append new line
                &entry != &state.gerber_list.back()
            );
        } else {
            // skip the last page break
            if(&entry != &state.gerber_list.back())
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

void execute_gerber2pdf(const MainState &state) {
    auto engine = ENGINE();
    engine.Light.R = state.bg_color_rgba[0];
    engine.Light.G = state.bg_color_rgba[1];
    engine.Light.B = state.bg_color_rgba[2];
    engine.Light.A = state.bg_color_rgba[3];
    switch(static_cast<PageSize>(state.page_size)) {
        case PageSize::A3:
            engine.PageSize = engine.PS_A3;
            break;
        case PageSize::A4:
            engine.PageSize = engine.PS_A4;
            break;
        case PageSize::LETTER:
            engine.PageSize = engine.PS_Letter;
            break;
        case PageSize::EXTENTS:
            engine.PageSize = engine.PS_Extents;
            break;
        case PageSize::TIGHT:
            engine.PageSize = engine.PS_Tight;
            break;
    }

    bool page_break_encountered = false;
    for(auto i = 0ul; i<state.gerber_list.size(); i++) {
        auto entry = state.gerber_list[i];
        if(!boost::get<GerberFile>(&entry)) {
            page_break_encountered = true;
            continue;
        }
        auto page = boost::get<GerberFile>(&entry);
        if(page_break_encountered) {
            engine.Combine = false;
            engine.NewPage = true;
            page_break_encountered = false;
        } else {
            engine.Combine = true;
            engine.NewPage = false;
        }
        engine.Dark.R = page->color_rgba[0];
        engine.Dark.G = page->color_rgba[1];
        engine.Dark.B = page->color_rgba[2];
        engine.Dark.A = page->color_rgba[3];
        engine.Mirror = page->is_mirrored;
        engine.Run(page->file_uri.c_str(), page->file_name.c_str());
    }
    engine.Finish(state.output_file.c_str());
}
