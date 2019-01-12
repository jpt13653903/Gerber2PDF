#if !defined(__LOGIC_HPP__)
#define __LOGIC_HPP__

#include <string>
#include <vector>
#include <boost/variant.hpp>

struct GerberFile {
    std::string file_name;
    std::string file_uri;
    bool is_mirrored;
    bool strokes_to_fills;
    float color_rgba[4] = {0, 0, 0, 1};
};

struct PageBreak {};

typedef boost::variant<GerberFile, PageBreak> GerberListEntry;

enum class PageSize {
    TIGHT = 0,
    A4 = 1,
    A3 = 2,
    LETTER = 3,
    EXTENTS = 4
};

struct MainState {
    std::vector<GerberListEntry> gerber_list;
    std::string output_file;
    float bg_color_rgba[4] = {255,255,255, 0};
    int page_size = static_cast<int>(PageSize::TIGHT);
};

std::string generate_batch_script(const MainState &state);
const char * page_size_to_string(PageSize page_size);
void execute_gerber2pdf(const MainState &state);

#endif // __LOGIC_HPP__