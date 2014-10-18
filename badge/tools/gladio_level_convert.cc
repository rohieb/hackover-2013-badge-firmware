extern "C" {
#include "gladio/enemy_types.h"
#include "gladio/level.h"
}

#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <fstream>
#include <sstream>
#include <string>

#define PATH_PREFIX "../badge/gladio/levels/"

struct enemy_type_parser {
  enemy_type_parser() {
    enemy_types.add
      ("rocket"             , GLADIO_ENEMY_ROCKET               )
      ("fighter"            , GLADIO_ENEMY_FIGHTER              )
      ("cannon_up"          , GLADIO_ENEMY_CANNON_UP            )
      ("cannon_down"        , GLADIO_ENEMY_CANNON_DOWN          )
      ("tumbler"            , GLADIO_ENEMY_TUMBLER              )
      ("wfighter_down"      , GLADIO_ENEMY_WFIGHTER_DOWN        )
      ("wfighter_up"        , GLADIO_ENEMY_WFIGHTER_UP          )
      ("finalboss_topgun"   , GLADIO_ENEMY_FINAL_BOSS_TOP_GUN   )
      ("finalboss_body"     , GLADIO_ENEMY_FINAL_BOSS_BODY      )
      ("finalboss_bottomgun", GLADIO_ENEMY_FINAL_BOSS_BOTTOM_GUN)
      ;
  }

  uint8_t operator()(std::string type) const {
    boost::trim(type);
    uint8_t const *p = enemy_types.find(type);
    if(!p) throw std::invalid_argument("Unbekannter Gegnertyp: " + type);
    return *p;
  }

  boost::spirit::qi::symbols<char, uint8_t> enemy_types;
} const enemy_type;

void level_dump(std::string const &filename) {
  std::ifstream in(filename.c_str());
  std::vector<gladio_level_spawn_spec> specs;

  if(!in) {
    throw std::invalid_argument("Could not open input file: " + filename);
  }

  unsigned linenum = 0;
  uint16_t time = 0;
  std::string spec_line;
  while(std::getline(in, spec_line)) {
    ++linenum;

    boost::trim(spec_line);
    if(spec_line == "") {
      continue;
    }

    std::istringstream parser(spec_line);

    unsigned pause;
    unsigned pos_y;
    std::string type;

    if((parser >> pause >> pos_y >> type)
       && pause < 65536
       && pos_y < 256)
    {
      time += pause;

      gladio_level_spawn_spec spec;
      spec.time = time;
      spec.pos_y = pos_y;
      spec.type = enemy_type(type);

      specs.push_back(spec);
    } else {
      std::ostringstream fmt;
      fmt << filename << ":" << linenum << ": Parsefehler";
      throw std::logic_error(fmt.str());
    }
  }

  if(linenum != 0) {
    std::ofstream out((filename + "l").c_str());
    if(!out) {
      throw std::invalid_argument("Could not open output file: " + filename + "l");
    }
    uint16_t len = specs.size();

    out.write(static_cast<char const*>(static_cast<void*>(&len)), sizeof(len));
    out.write(static_cast<char const*>(static_cast<void*>(&specs[0])), len * sizeof(specs[0]));
  }
}

int main(int argc, char *argv[]) {
  for(int i = 1; i < argc; ++i) {
    level_dump(argv[i]);
  }
}
