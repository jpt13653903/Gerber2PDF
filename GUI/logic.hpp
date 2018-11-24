#if !defined(__LOGIC_HPP__)
#define __LOGIC_HPP__

#include <string>
#include <sstream>
#include <variant>
#include <tuple>
#include <vector>

struct GerberFile {
    std::string file_name;
    std::string file_uri;
    bool is_mirrored;
    float color_rgba[4] = {0, 0, 0, 1};
};

struct PageBreak {};

typedef std::variant<GerberFile, PageBreak> GerberListEntry;

struct MainState {
    std::vector<GerberListEntry> gerber_list;
};

std::string generate_batch_script(const MainState &state);

#endif // __LOGIC_HPP__