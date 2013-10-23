#include <SFML/Graphics/Image.hpp>

#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>

#include <badge/ui/display.h>

double square(double x) { return x * x; }

int main(int argc, char *argv[]) {
  if(argc != 2) {
    std::cerr << "Usage: " << argv[0] << " [ FILE ]\n";
    return -1;
  }

  sf::Image img;
  if(!img.loadFromFile(argv[1])) {
    std::cerr << "Could not open file.\n";
    return -2;
  }

  if(img.getSize().x != BADGE_DISPLAY_WIDTH ||
     img.getSize().y != BADGE_DISPLAY_HEIGHT) {
    std::cerr << "Image must be " << BADGE_DISPLAY_WIDTH << "x" << BADGE_DISPLAY_HEIGHT << " pixels.\n";
    return -3;
  }

  badge_framebuffer fb = { { { 0 } } };

  for(unsigned x = 0; x < BADGE_DISPLAY_WIDTH; ++x) {
    for(unsigned y = 0; y < BADGE_DISPLAY_HEIGHT; ++y) {
      sf::Color c = img.getPixel(x, y);

      if(std::sqrt(0.241 * square(c.r / 255.) + 0.691 * square(c.g / 255.) + 0.068 * square(c.b / 255.)) < 0.5) {
        badge_framebuffer_pixel_on(&fb, x, y);
      }
    }
  }

  std::ofstream out("vanity.dat", std::ios::out | std::ios::binary);

  if(out) {
    out.write(static_cast<char const *>(static_cast<void const *>(&fb)), sizeof(fb));
  }

  return 0;
}
