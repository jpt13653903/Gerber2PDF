#include <gtkmm.h>
#include <optional>

template<typename T>
class ListStoreWrapper : public Gio::ListStore<T> {
public:
    void update_entry(int index, std::optional<T> entry) {
        if(index==-1) { // Append
            this->append(entry);
        } else if(entry) { // Update
            this->get(index) = entry;
        } else { // Delete
            this->get(index).reset();
        }
    }
};