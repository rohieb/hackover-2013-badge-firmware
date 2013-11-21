extern "C" {
#include <jumpnrun/levels.h>
#include <jumpnrun/items.h>
#include <jumpnrun/tiles.h>
#include <jumpnrun/enemies.h>
}

#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

enum {
  LEVEL_LINE_COUNT = 14
};

#define PATH_PREFIX "../badge/jumpnrun/levels/"

namespace jnrcpp {
  struct descriptors {
    descriptors() {
      tiles.add
        ("tube_top_left" , JUMPNRUN_TILE_TYPE_TUBE_TOP_LEFT )
        ("tube_top_right", JUMPNRUN_TILE_TYPE_TUBE_TOP_RIGHT)
        ("tube_left"     , JUMPNRUN_TILE_TYPE_TUBE_LEFT     )
        ("tube_right"    , JUMPNRUN_TILE_TYPE_TUBE_RIGHT    )
        ("brick"         , JUMPNRUN_TILE_TYPE_BRICK         )
        ("square"        , JUMPNRUN_TILE_TYPE_SQUARE        )
        ("spike_up"      , JUMPNRUN_TILE_TYPE_SPIKE_UP      )
        ("spike_right"   , JUMPNRUN_TILE_TYPE_SPIKE_RIGHT   )
        ("spike_down"    , JUMPNRUN_TILE_TYPE_SPIKE_DOWN    )
        ("spike_left"    , JUMPNRUN_TILE_TYPE_SPIKE_LEFT    )
        ;

      items.add
        ("doc"          , JUMPNRUN_ITEM_TYPE_DOCUMENT          )
        ("checkpoint"   , JUMPNRUN_ITEM_TYPE_CHECKPOINT        )
        ("key"          , JUMPNRUN_ITEM_TYPE_KEY               )
        ("doc_encrypted", JUMPNRUN_ITEM_TYPE_ENCRYPTED_DOCUMENT)
        ;

      enemies.add
        ("cat"          , JUMPNRUN_ENEMY_TYPE_CAT          )
        ("mushroom"     , JUMPNRUN_ENEMY_TYPE_MUSHROOM     )
        ("bunny"        , JUMPNRUN_ENEMY_TYPE_BUNNY        )
        ("kaninchen"    , JUMPNRUN_ENEMY_TYPE_BUNNY        ) // legacy
        ("snake"        , JUMPNRUN_ENEMY_TYPE_SNAKE        )
        ("spiral"       , JUMPNRUN_ENEMY_TYPE_SPIRAL       )
        ("rotor"        , JUMPNRUN_ENEMY_TYPE_ROTOR        )
        ("dog"          , JUMPNRUN_ENEMY_TYPE_DOG          )
        ("giraffe"      , JUMPNRUN_ENEMY_TYPE_GIRAFFE      )
        ("bird"         , JUMPNRUN_ENEMY_TYPE_BIRD         )
        ("bird_straight", JUMPNRUN_ENEMY_TYPE_BIRD_STRAIGHT)
        ("bird_dip"     , JUMPNRUN_ENEMY_TYPE_BIRD_DIP     )
        ;
    }

    boost::spirit::qi::symbols<char, unsigned> tiles;
    boost::spirit::qi::symbols<char, unsigned> items;
    boost::spirit::qi::symbols<char, unsigned> enemies;
  } const desc;

  struct level_name_map {
    level_name_map() {
      std::ifstream in("levels.txt");

      if(!in) {
        throw std::logic_error("levels.txt konnte nicht geöffnet werden.");
      }

      std::string name;
      while(std::getline(in, name)) {
        if(boost::trim_copy(name) != "") {
          names.push_back(name);
        }
      }
    }

    std::vector<std::string> names;
  } const level_names;

  class level {
  public:
    level(std::string const &filename)
      : level_lines(LEVEL_LINE_COUNT)
    {
      std::ifstream in(filename.c_str());

      if(!in) {
        throw std::invalid_argument("Could not open file: " + filename);
      }

      for(std::size_t i = 0; i < level_lines.size(); ++i) {
        std::getline(in, level_lines[i]);
      }

      std::string line;
      std::string type_prefix;
      std::map<char, std::string> *objmap = 0;

      while(std::getline(in, line)) {
        if(line[0] == '[') {
          if(line == "[tiles]") {
            objmap = &tile_types;
          } else if(line == "[items]") {
            objmap = &item_types;
          } else if(line == "[enemies]") {
            objmap = &enemy_types;
          } else if(line == "[parameters]") {
            objmap = &level_params;
          } else {
            throw std::invalid_argument("Unkown type: " + line);
          }
        } else if(boost::trim_right_copy(line) != "") {
          char c;
          std::string tok;
          std::istringstream parser(line);

          if(parser >> c >> tok) {
            if(objmap) {
              (*objmap)[c] = tok;
            }
          } else {
            throw std::invalid_argument("Line not parseable: " + line);
          }
        }
      }
    }

    std::size_t length() const {
      std::size_t level_length = 0;

      for(std::size_t i = 0; i < level_lines.size(); ++i) {
        level_length = std::max(level_length, level_lines[i].size());
      }

      return level_length;
    }

    unsigned find_type(boost::spirit::qi::symbols<char, unsigned> const &types, std::string const &type, std::string const &error) const {
      unsigned const *p = types.find(type);
      if(!p) throw std::invalid_argument(error + type);
      return *p;
    }

    std::size_t count_things(std::map<char, std::string> const &objmap) const {
      std::size_t len = length();
      std::size_t i = 0;

      for(std::size_t x = 0; x < len; ++x) {
        for(std::size_t y = 0; y < level_lines.size(); ++y) {
          if(x < level_lines[y].size()) {
            auto iter = objmap.find(level_lines[y][x]);

            if(iter != objmap.end()) {
              ++i;
            }
          }
        }
      }

      return i;
    }

    std::size_t count_tiles  () const { return count_things(tile_types ); }
    std::size_t count_items  () const { return count_things(item_types ); }
    std::size_t count_enemies() const { return count_things(enemy_types); }

    void dump_things(std::ostream &dest, std::map<char, std::string> const &objmap, boost::spirit::qi::symbols<char, unsigned> const &types, std::string const &error) const {
      std::size_t len = length();

      for(std::size_t x = 0; x < len; ++x) {
        for(std::size_t y = 0; y < level_lines.size(); ++y) {
          if(level_lines[y].size() < x) continue;

          auto iter = objmap.find(level_lines[y][x]);

          if(iter != objmap.end()) {
            unsigned char buf[3];
            buf[0] = static_cast<uint8_t>(y) << 4 | static_cast<uint8_t>(x >> 8);
            buf[1] = static_cast<uint8_t>(x);
            buf[2] = static_cast<uint8_t>(find_type(types, iter->second, error));

            dest.write(static_cast<char const*>(static_cast<void*>(&buf[0])), sizeof(buf));
          }
        }
      }
    }

    void dump_tiles  (std::ostream &dest) const { dump_things(dest,  tile_types, desc.tiles  , "Unbekannter Tile-Typ: "); }
    void dump_items  (std::ostream &dest) const { dump_things(dest,  item_types, desc.items  , "Unbekannter Item-Typ: "); }
    void dump_enemies(std::ostream &dest) const { dump_things(dest, enemy_types, desc.enemies, "Unbekannter Enemy-Typ: "); }

    std::pair<std::size_t, std::size_t> starting_position() const {
      std::pair<std::size_t, std::size_t> player_pos(0, 0);

      for(std::size_t y = 0; y < level_lines.size(); ++y) {
        std::size_t x = level_lines[y].find('P');

        if(x != std::string::npos) {
          player_pos.first = x;
          player_pos.second = y;
          break;
        }
      }

      return player_pos;
    }

    void generate_header(jumpnrun_level *dest) const {
      dest->header.tile_count  = count_tiles  ();
      dest->header.item_count  = count_items  ();
      dest->header.enemy_count = count_enemies();
    }

    void dump(std::ostream &dest) const {
      jumpnrun_level dummy;

      generate_header(&dummy);
      uint16_t head[3] = {
        static_cast<uint16_t>(dummy.header. tile_count),
        static_cast<uint16_t>(dummy.header. item_count),
        static_cast<uint16_t>(dummy.header.enemy_count)
      };

      std::pair<std::size_t, std::size_t> player_pos = starting_position();

      uint16_t spos[2] = {
        static_cast<uint16_t>(player_pos.first),
        static_cast<uint16_t>(player_pos.second)
      };

      uint8_t lives = 3;
      {
        std::map<char, std::string>::const_iterator iter = level_params.find('L');
        if(iter != level_params.end()) {
          unsigned x;
          std::istringstream parser(iter->second);
          if(parser >> x) {
            lives = static_cast<uint8_t>(x);
          }
        }
      }

      dest.write(static_cast<char const *>(static_cast<void const *>(head  )), sizeof(head ));
      dest.write(static_cast<char const *>(static_cast<void const *>(spos  )), sizeof(spos ));
      dest.write(static_cast<char const *>(static_cast<void const *>(&lives)), sizeof(lives));

      dump_tiles(dest);
      dump_items(dest);
      dump_enemies(dest);
    }

  private:
    std::vector<std::string> level_lines;
    std::map<char, std::string> tile_types;
    std::map<char, std::string> item_types;
    std::map<char, std::string> enemy_types;
    std::map<char, std::string> level_params;
  };
}

void jumpnrun_level_dump(std::string const &filename) {
  try {
    std::string outfilename = filename + 'l';
    jnrcpp::level lv(filename);
    std::ofstream out(outfilename.c_str());
    lv.dump(out);
  } catch(std::exception const &e) {
    std::cerr << e.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown error." << std::endl;
  }
}

int main(int argc, char *argv[]) {
  for(int i = 1; i < argc; ++i) {
    jumpnrun_level_dump(argv[i]);
  }
}
