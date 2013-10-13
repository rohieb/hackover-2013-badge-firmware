#ifndef INCLUDED_BADGE2013_MOCKUP_LCD_WINDOW_HH
#define INCLUDED_BADGE2013_MOCKUP_LCD_WINDOW_HH

#include "badge_worker.hh"
#include <ui/display.h>

#include <glibmm/dispatcher.h>
#include <glibmm/thread.h>

#include <gtkmm/builder.h>
#include <gtkmm/dialog.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/window.h>

#include <cstdint>
#include <memory>

namespace badge2013 {
  class lcd_drawingarea : public Gtk::DrawingArea {
  public:
    lcd_drawingarea(BaseObjectType *cobject,
		    Glib::RefPtr<Gtk::Builder> const &glade);
    virtual ~lcd_drawingarea();

    void push_framebuffer(badge_framebuffer const &fb);

  protected:
    virtual bool on_draw(Cairo::RefPtr<Cairo::Context> const &cr);

  private:
    void force_redraw();

    Glib::Dispatcher  sig_redraw_;
    badge_framebuffer framebuffer_;  
  };


  class lcd_window : public Gtk::Window {
  public:
    lcd_window(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &glade);
    virtual ~lcd_window();

    void push_framebuffer(badge_framebuffer const &fb);
    void request_close();

  protected:
    virtual void on_show();
    virtual bool on_key_press_event  (GdkEventKey* event);
    virtual bool on_key_release_event(GdkEventKey* event);

  private:
    bool on_game_tick();

    bool             running_ = false;
    Glib::Dispatcher sig_close_;
    badge_worker     worker_;
    lcd_drawingarea *canvas_    = 0;
    std::uint8_t     key_state_ = 0;
    std::uint8_t     ticks_ = 0;
  };
}

#endif
