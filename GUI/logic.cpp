#include "logic.hpp"

std::string generate_batch_script(const MainState &state) {
   std::stringstream out_string;
   out_string << "Gerber2pdf -silentexit ";
   out_string << "-combine\n";
   for(auto &entry: state.gerber_list) {
       if(std::holds_alternative<GerberFile>(entry)) {
           GerberFile file_entry = std::get<GerberFile>(entry);
           if(file_entry.is_mirrored) {
               out_string << "-mirror ";
           } else {
               out_string << "-nomirror ";
           }
           out_string << "-color=" << (int) (file_entry.color_rgba[0]*255) <<","
                                   << (int) (file_entry.color_rgba[1]*255) <<","
                                   << (int) (file_entry.color_rgba[2]*255) <<","
                                   << (int) (file_entry.color_rgba[3]*255) <<" ";

           out_string << file_entry.file_uri;
           out_string << "\n";
       } else {
           out_string << "-combine\n";
       }
   }
   return out_string.str();
}