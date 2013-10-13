#include "mock/lcd-window.hh"

#include <glibmm/thread.h>
#include <gtkmm/main.h>

#include <memory>

int main(int argc, char *argv[]) {
  Glib::thread_init();

  Gtk::Main kit(argc, argv);
  Glib::RefPtr<Gtk::Builder> glade = Gtk::Builder::create();
  badge2013::lcd_window *win_naked = nullptr;

  glade->add_from_file("mock/lcd-display.glade");
  glade->get_widget_derived("mainwindow", win_naked);

  std::unique_ptr<badge2013::lcd_window> win(win_naked);

  if(win) {
    kit.run(*win);
  }
}
