#if !defined(__LOGIC_HPP__)
#define __LOGIC_HPP__

#include <string>
#include <variant>
#include <tuple>
#include <vector>

struct GerberFile {
    std::string file_uri;
    bool is_mirrored;
    float color_rgba[4];
};

struct PageBreak {};

typedef std::variant<GerberFile, PageBreak> GerberListEntry;

struct MainState {
    std::vector<GerberListEntry> gerber_list;
};


#endif // __LOGIC_HPP__