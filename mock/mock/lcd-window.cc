#include "lcd-window.hh"
#include "badge_worker.hh"
#include "mock/event.h"

#include <gtkmm/main.h>

namespace badge2013 {
  lcd_drawingarea::lcd_drawingarea(BaseObjectType *cobject,
				   Glib::RefPtr<Gtk::Builder> const &)
    : Gtk::DrawingArea(cobject)
  {
    sig_redraw_.connect(sigc::mem_fun(*this, &lcd_drawingarea::force_redraw));
    badge_framebuffer_clear(&framebuffer_);
  }

  lcd_drawingarea::~lcd_drawingarea() { }

  void lcd_drawingarea::push_framebuffer(badge_framebuffer const &fb) {
    framebuffer_ = fb;
    sig_redraw_();
  }

  void lcd_drawingarea::force_redraw() {
    Glib::RefPtr<Gdk::Window> win = get_window();
    if(win) {
      win->invalidate(true);
    }
  }

  bool lcd_drawingarea::on_draw(Cairo::RefPtr<Cairo::Context> const &cr) {
    Gtk::Allocation alloc = get_allocation();
    double w_width  = alloc.get_width ();
    double w_height = alloc.get_height();

    double c_width  = w_width  / BADGE_DISPLAY_WIDTH;
    double c_height = w_height / BADGE_DISPLAY_HEIGHT;

    cr->save();
  
    for(int i = 0; i < BADGE_DISPLAY_WIDTH; ++i) {
      for(int j = 0; j < BADGE_DISPLAY_HEIGHT; ++j) {
	if(badge_framebuffer_pixel(&framebuffer_, i, j)) {
	  cr->rectangle(c_width * i, c_height * j,
			c_width    , c_height);
	}
      }
    }

    cr->fill();
    cr->restore();

    return true;
  }

  ///////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////

  lcd_window::lcd_window(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &glade)
    : Gtk::Window(cobject),
      worker_(this)
  {
    glade->get_widget_derived("canvas", canvas_);

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &lcd_window::on_game_tick ), 11);
    sig_close_.connect(sigc::mem_fun(*this, &lcd_window::hide));
  }

  lcd_window::~lcd_window() { }

  void lcd_window::push_framebuffer(badge_framebuffer const &fb) {
    canvas_->push_framebuffer(fb);
  }

  void lcd_window::request_close() {
    sig_close_();
  }

  bool lcd_window::on_key_press_event  (GdkEventKey* event) {
    uint8_t new_state = key_state_;

    switch(event->keyval) {
    case GDK_KEY_e: new_state |= BADGE_EVENT_KEY_UP;     break;
    case GDK_KEY_s: new_state |= BADGE_EVENT_KEY_LEFT;   break;
    case GDK_KEY_d: new_state |= BADGE_EVENT_KEY_DOWN;   break;
    case GDK_KEY_f: new_state |= BADGE_EVENT_KEY_RIGHT;  break;
    case GDK_KEY_a: new_state |= BADGE_EVENT_KEY_CENTER; break;
    case GDK_KEY_j: new_state |= BADGE_EVENT_KEY_BTN_A;  break;
    case GDK_KEY_k: new_state |= BADGE_EVENT_KEY_BTN_B;  break;
    }

    badge_event_push(badge_event_new(BADGE_EVENT_USER_INPUT, key_state_, new_state));
    key_state_ = new_state;

    return true;
  }

  bool lcd_window::on_key_release_event(GdkEventKey* event) {
    uint8_t new_state = key_state_;

    switch(event->keyval) {
    case GDK_KEY_e: new_state &= ~BADGE_EVENT_KEY_UP;     break;
    case GDK_KEY_s: new_state &= ~BADGE_EVENT_KEY_LEFT;   break;
    case GDK_KEY_d: new_state &= ~BADGE_EVENT_KEY_DOWN;   break;
    case GDK_KEY_f: new_state &= ~BADGE_EVENT_KEY_RIGHT;  break;
    case GDK_KEY_a: new_state &= ~BADGE_EVENT_KEY_CENTER; break;
    case GDK_KEY_j: new_state &= ~BADGE_EVENT_KEY_BTN_A;  break;
    case GDK_KEY_k: new_state &= ~BADGE_EVENT_KEY_BTN_B;  break;
    }

    badge_event_push(badge_event_new(BADGE_EVENT_USER_INPUT, key_state_, new_state));
    key_state_ = new_state;

    return true;  
  }

  bool lcd_window::on_game_tick() {
    badge_event_push(badge_event_new(BADGE_EVENT_GAME_TICK, key_state_, key_state_));

    return true;
  }

  void lcd_window::on_show() {
    Gtk::Window::on_show();

    if(!running_) {
      running_ = true;
      Glib::Thread::create(sigc::mem_fun(worker_, &badge_worker::run));
    }
  }
}
