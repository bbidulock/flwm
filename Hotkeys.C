// Hotkeys.C

// If you want to change what the hotkeys are, see the table at the bottom!

#include "config.h"
#include "Frame.H"
#include "Desktop.H"
#include <stdio.h>

extern void ShowMenu();
extern void ShowTabMenu(int tab);

#if STANDARD_HOTKEYS

static void NextWindow() { // Alt+Tab
  ShowTabMenu(1);
}

static void PreviousWindow() { // Alt+Shift+Tab
  ShowTabMenu(-1);
}

#endif

#if DESKTOPS

static void NextDesk() {
  if (Desktop::current()) {
    Desktop::current(Desktop::current()->next?
		     Desktop::current()->next:Desktop::first);
  } else {
    Desktop::current(Desktop::first);
  }
}

static void PreviousDesk() {
  Desktop* search=Desktop::first;
  while (search->next && search->next!=Desktop::current()){
    search=search->next;
  }
  Desktop::current(search);
}

// warning: this assummes it is bound to Fn key:
static void DeskNumber() {
  Desktop::current(Desktop::number(Fl::event_key()-FL_F, 1));
}
#endif

#if WMX_HOTKEYS || CDE_HOTKEYS

static void Raise() { // Alt+Up
  Frame* f = Frame::activeFrame();
  if (f) f->raise();
}

static void Lower() { // Alt+Down
  Frame* f = Frame::activeFrame();
  if (f) f->lower();
}

static void Iconize() { // Alt+Enter
  Frame* f = Frame::activeFrame();
  if (f) f->iconize();
  else ShowMenu(); // so they can deiconize stuff
}

static void Close() { // Alt+Delete
  Frame* f = Frame::activeFrame();
  if (f) f->close();
}

#endif

////////////////////////////////////////////////////////////////

static struct {int key; void (*func)();} keybindings[] = {
#if STANDARD_HOTKEYS || MINIMUM_HOTKEYS
  // these are very common and tend not to conflict, due to Windoze:
  {FL_ALT+FL_Escape,	ShowMenu},
  {FL_ALT+FL_Menu,	ShowMenu},
#endif
#if STANDARD_HOTKEYS
  {FL_ALT+FL_Tab,	NextWindow},
  {FL_ALT+FL_SHIFT+FL_Tab,PreviousWindow},
  {FL_ALT+FL_SHIFT+0xfe20,PreviousWindow}, // XK_ISO_Left_Tab
#endif
#if KWM_HOTKEYS && DESKTOPS // KWM uses these to switch desktops
//   {FL_CTRL+FL_Tab,	NextDesk},
//   {FL_CTRL+FL_SHIFT+FL_Tab,PreviousDesk},
//   {FL_CTRL+FL_SHIFT+0xfe20,PreviousDesk}, // XK_ISO_Left_Tab
  {FL_CTRL+FL_F+1,	DeskNumber},
  {FL_CTRL+FL_F+2,	DeskNumber},
  {FL_CTRL+FL_F+3,	DeskNumber},
  {FL_CTRL+FL_F+4,	DeskNumber},
  {FL_CTRL+FL_F+5,	DeskNumber},
  {FL_CTRL+FL_F+6,	DeskNumber},
  {FL_CTRL+FL_F+7,	DeskNumber},
  {FL_CTRL+FL_F+8,	DeskNumber},
  {FL_CTRL+FL_F+9,	DeskNumber},
  {FL_CTRL+FL_F+10,	DeskNumber},
  {FL_CTRL+FL_F+11,	DeskNumber},
  {FL_CTRL+FL_F+12,	DeskNumber},
#endif
#if WMX_HOTKEYS
  // wmx also sets all these, they seem pretty useful:
  {FL_ALT+FL_Up,	Raise},
  {FL_ALT+FL_Down,	Lower},
  {FL_ALT+FL_Enter,	Iconize},
  {FL_ALT+FL_Delete,	Close},
  //{FL_ALT+FL_Page_Up,	ToggleMaxH},
  //{FL_ALT+FL_Page_Down,ToggleMaxW},
#endif
#if WMX_DESK_HOTKEYS && DESKTOPS
  // these wmx keys are not set by default as they break NetScape:
  {FL_ALT+FL_Left,	PreviousDesk},
  {FL_ALT+FL_Right,	NextDesk},
#endif
#if CDE_HOTKEYS
  // CDE hotkeys (or at least what SGI's 4DWM uses):
  {FL_ALT+FL_F+1,	Raise},
//{FL_ALT+FL_F+2,	unknown}, // KWM uses this to run a typed-in command
  {FL_ALT+FL_F+3,	Lower},
  {FL_ALT+FL_F+4,	Close}, // this matches KWM
//{FL_ALT+FL_F+5,	Restore}, // useless because no icons visible
//{FL_ALT+FL_F+6,	unknown}, // ?
//{FL_ALT+FL_F+7,	Move}, // grabs the window for movement
//{FL_ALT+FL_F+8,	Resize}, // grabs the window for resizing
  {FL_ALT+FL_F+9,	Iconize},
//{FL_ALT+FL_F+10,	Maximize},
//{FL_ALT+FL_F+11,	unknown}, // ?
  {FL_ALT+FL_F+12,	Close}, // actually does "quit"
#else
#if DESKTOPS && DESKTOP_HOTKEYS
  // seem to be common to Linux window managers
  {FL_ALT+FL_F+1,	DeskNumber},
  {FL_ALT+FL_F+2,	DeskNumber},
  {FL_ALT+FL_F+3,	DeskNumber},
  {FL_ALT+FL_F+4,	DeskNumber},
  {FL_ALT+FL_F+5,	DeskNumber},
  {FL_ALT+FL_F+6,	DeskNumber},
  {FL_ALT+FL_F+7,	DeskNumber},
  {FL_ALT+FL_F+8,	DeskNumber},
  {FL_ALT+FL_F+9,	DeskNumber},
  {FL_ALT+FL_F+10,	DeskNumber},
  {FL_ALT+FL_F+11,	DeskNumber},
  {FL_ALT+FL_F+12,	DeskNumber},
#endif
#endif
  {0}};

#if FL_MAJOR_VERSION > 1
// Define missing function, this should get put in fltk2.0:
namespace fltk {
int test_shortcut(int shortcut) {
  if (!shortcut) return 0;

  int shift = Fl::event_state();
  // see if any required shift flags are off:
  if ((shortcut&shift) != (shortcut&0x7fff0000)) return 0;
  // record shift flags that are wrong:
  int mismatch = (shortcut^shift)&0x7fff0000;
  // these three must always be correct:
  if (mismatch&(FL_META|FL_ALT|FL_CTRL)) return 0;

  int key = shortcut & 0xffff;

  // if shift is also correct, check for exactly equal keysyms:
  if (!(mismatch&(FL_SHIFT)) && unsigned(key) == Fl::event_key()) return 1;

  // try matching ascii, ignore shift:
  if (key == event_text()[0]) return 1;

  // kludge so that Ctrl+'_' works (as opposed to Ctrl+'^_'):
  if ((shift&FL_CTRL) && key >= 0x3f && key <= 0x5F
      && event_text()[0]==(key^0x40)) return 1;
  return 0;
}
}
#endif

int Handle_Hotkey() {
  for (int i = 0; keybindings[i].key; i++) {
    if (Fl::test_shortcut(keybindings[i].key) ||
	(keybindings[i].key & 0xFFFF) == FL_Delete
	&& Fl::event_key() == FL_BackSpace// fltk bug?
	) {
      keybindings[i].func();
      return 1;
    }
  }
  return 0;
}

extern Fl_Window* Root;

void Grab_Hotkeys() {
  XWindow root = fl_xid(Root);
  for (int i = 0; keybindings[i].key; i++) {
    int k = keybindings[i].key;
    int keycode = XKeysymToKeycode(fl_display, k & 0xFFFF);
    if (!keycode) continue;
    // Silly X!  we need to ignore caps lock & numlock keys by grabbing
    // all the combinations:
    XGrabKey(fl_display, keycode, k>>16,     root, 0, 1, 1);
    XGrabKey(fl_display, keycode, (k>>16)|2, root, 0, 1, 1); // CapsLock
    XGrabKey(fl_display, keycode, (k>>16)|16, root, 0, 1, 1); // NumLock
    XGrabKey(fl_display, keycode, (k>>16)|18, root, 0, 1, 1); // both
  }
}

