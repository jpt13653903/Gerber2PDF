#if !defined(__LOGIC_HPP__)
#define __LOGIC_HPP__

#include <string>
#include <sstream>
#include <vector>
#include <boost/variant.hpp>

struct GerberFile {
    std::string file_name;
    std::string file_uri;
    bool is_mirrored;
    float color_rgba[4] = {0, 0, 0, 1};
};

struct PageBreak {};

typedef boost::variant<GerberFile, PageBreak> GerberListEntry;

enum PAGE_SIZE {
    PAGE_SIZE_A4 = 0,
    PAGE_SIZE_A3 = 1,
    PAGE_SIZE_EXTENTS = 2,
    PAGE_SIZE_LETTER = 3
};

struct MainState {
    std::vector<GerberListEntry> gerber_list;
    std::string output_file;
    float bg_color_rgba[4] = {255,255,255, 1};
    int page_size = 0;
    bool is_stroke2fills = false;
};

std::string generate_batch_script(const MainState &state);

#endif // __LOGIC_HPP__