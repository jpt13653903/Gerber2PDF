#if !defined(__LOGIC_HPP__)
#define __LOGIC_HPP__

#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <functional>

struct GerberFile {
    std::string file_uri;
    bool is_mirrored;
};

struct PageBreak {};

typedef std::variant<GerberFile, PageBreak> GerberListEntry;

class MainState {
    std::vector<GerberListEntry> gerber_list;
    /* 
     * When adding new entry, index = -1.
     * When deleting, entry will be null.
     * When updating, both index and entry will contain valid values.
     */
    using CBListUpdated = std::function<void(int index, std::optional<GerberListEntry> entry)>;
    std::optional<CBListUpdated> cb_list_updated = {};
public:
    void add_to_list(GerberListEntry list_entry);
    void remove_from_list(int index);
    void update_list_at(int index, GerberListEntry list_entry);
    void set_cb_list_updated(std::optional<CBListUpdated> cb_list_updated);
    const std::vector<GerberListEntry> get_gerber_list();
};


#endif // __LOGIC_HPP__