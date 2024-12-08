#pragma once

#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#pragma comment(lib, "user32.lib")

namespace yw {

using string = std::u32string;
using string_view = std::u32string_view;
using namespace std::string_literals;

constexpr std::wstring to_wstring(const string& s) {
  std::wstring r(s.size() * 2, 0);
  auto p = r.begin();
  for (const auto c : s) {
    if (c >= 0x10000) {
      *p++ = static_cast<wchar_t>(0xD800 + ((c - 0x10000) >> 10));
      *p++ = static_cast<wchar_t>(0xDC00 + ((c - 0x10000) & 0x3FF));
    } else *p++ = static_cast<wchar_t>(c);
  }
  r.resize(p - r.begin());
  return r;
}

constexpr std::string to_string(const string& s) {
  std::string r(s.size() * 4, 0);
  auto p = r.begin();
  for (const auto c : s) {
    if (c >= 0x10000) {
      *p++ = static_cast<char>(0xF0 | ((c >> 18) & 0x07));
      *p++ = static_cast<char>(0x80 | ((c >> 12) & 0x3F));
      *p++ = static_cast<char>(0x80 | ((c >> 6) & 0x3F));
      *p++ = static_cast<char>(0x80 | (c & 0x3F));
    } else if (c >= 0x800) {
      *p++ = static_cast<char>(0xE0 | ((c >> 12) & 0x0F));
      *p++ = static_cast<char>(0x80 | ((c >> 6) & 0x3F));
      *p++ = static_cast<char>(0x80 | (c & 0x3F));
    } else if (c >= 0x80) {
      *p++ = static_cast<char>(0xC0 | ((c >> 6) & 0x1F));
      *p++ = static_cast<char>(0x80 | (c & 0x3F));
    } else *p++ = static_cast<char>(c);
  }
  r.resize(p - r.begin());
  return r;
}



struct null {
  constexpr null() noexcept = default;
  constexpr null(auto&&...) noexcept {}
  constexpr null& operator=(auto&&) noexcept { return *this; }
  explicit constexpr operator bool() const noexcept { return false; }
  friend constexpr bool operator==(null, null) noexcept { return false; }
  friend constexpr auto operator<=>(null, null) noexcept { return std::partial_ordering::unordered; }
  friend constexpr null operator+(null) noexcept { return {}; }
  friend constexpr null operator-(null) noexcept { return {}; }
  friend constexpr null operator+(null, null) noexcept { return {}; }
  friend constexpr null operator-(null, null) noexcept { return {}; }
  friend constexpr null operator*(null, null) noexcept { return {}; }
  friend constexpr null operator/(null, null) noexcept { return {}; }
  friend constexpr null& operator+=(null& n, null) noexcept { return n; }
  friend constexpr null& operator-=(null& n, null) noexcept { return n; }
  friend constexpr null& operator*=(null& n, null) noexcept { return n; }
  friend constexpr null& operator/=(null& n, null) noexcept { return n; }
};

inline null lvnull{};

template<typename T> class nullable {
public:
  bool has_value;
  T value{};

  constexpr nullable() : has_value(false) {}
  constexpr nullable(const T& value) : has_value(true), value(value) {}
  constexpr nullable(T&& value) : has_value(true), value(static_cast<T&&>(value)) {}
  
  constexpr operator bool() const { 
    if constexpr (requires { static_cast<bool>(value); }) return has_value && bool(value);
    else return has_value;
  }
  constexpr bool is_null() const { return !has_value; }
  constexpr T& get() { return value; }
  constexpr const T& get() const { return value; }
};

struct vector2 {
  double x, y;
};

inline const HINSTANCE hinstance = GetModuleHandleW(nullptr);

inline bool ok(const string& text, const string& caption = U"OK?"s) {
  auto text_w = to_wstring(text), caption_w = to_wstring(caption);
  return MessageBoxW(nullptr, text_w.c_str(), caption_w.c_str(), MB_OK) == IDOK;
}

inline bool yesno(const string& text, const string& caption = U"YES?"s) {
  auto text_w = to_wstring(text), caption_w = to_wstring(caption);
  return MessageBoxW(nullptr, text_w.c_str(), caption_w.c_str(), MB_YESNO) == IDYES;
}

class window;

////////////////////////////////////////////////////////////////////////////////
//  WINDOW MESSAGE

enum class wmessage : unsigned {
  wm_null = WM_NULL, wm_create = WM_CREATE, wm_destroy = WM_DESTROY, wm_move = WM_MOVE, wm_size = WM_SIZE, wm_activate = WM_ACTIVATE, 
  wm_setfocus = WM_SETFOCUS, wm_killfocus = WM_KILLFOCUS, wm_enable = WM_ENABLE, wm_setredraw = WM_SETREDRAW, wm_settext = WM_SETTEXT, wm_gettext = WM_GETTEXT, 
  wm_gettextlength = WM_GETTEXTLENGTH, wm_paint = WM_PAINT, wm_close = WM_CLOSE, wm_queryendsession = WM_QUERYENDSESSION, wm_queryopen = WM_QUERYOPEN,
  wm_endsession = WM_ENDSESSION, wm_quit = WM_QUIT, wm_erasebkgnd = WM_ERASEBKGND, wm_syscolorchange = WM_SYSCOLORCHANGE, wm_showwindow = WM_SHOWWINDOW, 
  wm_wininichange = WM_WININICHANGE, wm_settingchange = WM_SETTINGCHANGE, wm_devmodechange = WM_DEVMODECHANGE, wm_activateapp = WM_ACTIVATEAPP, 
  wm_fontchange = WM_FONTCHANGE, wm_timechange = WM_TIMECHANGE, wm_cancelsmode = WM_CANCELMODE, wm_setcursor = WM_SETCURSOR,  
  wm_mouseactivate = WM_MOUSEACTIVATE, wm_childactivate = WM_CHILDACTIVATE, wm_queuesync = WM_QUEUESYNC, wm_getminmaxinfo = WM_GETMINMAXINFO, 
  wm_painticon = WM_PAINTICON, wm_iconerasebkgnd = WM_ICONERASEBKGND, wm_nextdlgctl = WM_NEXTDLGCTL, wm_spoolerstatus = WM_SPOOLERSTATUS, 
  wm_drawitem = WM_DRAWITEM,  wm_measureitem = WM_MEASUREITEM, wm_deleteitem = WM_DELETEITEM, wm_vkeytoitem = WM_VKEYTOITEM, wm_chartoitem = WM_CHARTOITEM, 
  wm_setfont = WM_SETFONT, wm_getfont = WM_GETFONT, wm_sethotkey = WM_SETHOTKEY, wm_gethotkey = WM_GETHOTKEY, wm_querydragicon = WM_QUERYDRAGICON, 
  wm_compareitem = WM_COMPAREITEM, wm_getobject = WM_GETOBJECT, wm_compacting = WM_COMPACTING, wm_commnotify = WM_COMMNOTIFY, 
  wm_windowposchanging = WM_WINDOWPOSCHANGING, wm_windowposchanged = WM_WINDOWPOSCHANGED, wm_power = WM_POWER, wm_copydata = WM_COPYDATA, 
  wm_canceljournal = WM_CANCELJOURNAL, wm_notify = WM_NOTIFY, wm_inputlangchangerequest = WM_INPUTLANGCHANGEREQUEST, wm_inputlangchange = WM_INPUTLANGCHANGE, 
  wm_tcard = WM_TCARD, wm_help = WM_HELP, wm_userchanged = WM_USERCHANGED, wm_notifyformat = WM_NOTIFYFORMAT, wm_contextmenu = WM_CONTEXTMENU, 
  wm_stylechanging = WM_STYLECHANGING, wm_stylechanged = WM_STYLECHANGED, wm_displaychange = WM_DISPLAYCHANGE, wm_geticon = WM_GETICON, 
  wm_seticon = WM_SETICON, wm_nccreate = WM_NCCREATE, wm_ncdestroy = WM_NCDESTROY, wm_nccalcsize = WM_NCCALCSIZE, wm_nchittest = WM_NCHITTEST, 
  wm_ncpaint = WM_NCPAINT, wm_ncactivate = WM_NCACTIVATE, wm_getdlgcode = WM_GETDLGCODE, wm_syncpaint = WM_SYNCPAINT, wm_ncmousemove = WM_NCMOUSEMOVE, 
  wm_nclbuttondown = WM_NCLBUTTONDOWN, wm_nclbuttonup = WM_NCLBUTTONUP, wm_nclbuttondblclk = WM_NCLBUTTONDBLCLK, wm_ncrbuttondown = WM_NCRBUTTONDOWN, 
  wm_ncrbuttonup = WM_NCRBUTTONUP, wm_ncrbuttondblclk = WM_NCRBUTTONDBLCLK, wm_ncmbuttondown = WM_NCMBUTTONDOWN, wm_ncmbuttonup = WM_NCMBUTTONUP, 
  wm_ncmbuttondblclk = WM_NCMBUTTONDBLCLK, wm_ncxbuttondown = WM_NCXBUTTONDOWN, wm_ncxbuttonup = WM_NCXBUTTONUP, wm_ncxbuttondblclk = WM_NCXBUTTONDBLCLK,
  wm_input_device_change = WM_INPUT_DEVICE_CHANGE, wm_input = WM_INPUT, wm_keyfirst = WM_KEYFIRST, wm_keydown = WM_KEYDOWN, wm_keyup = WM_KEYUP, 
  wm_char = WM_CHAR, wm_deadchar = WM_DEADCHAR, wm_syskeydown = WM_SYSKEYDOWN, wm_syskeyup = WM_SYSKEYUP, wm_syschar = WM_SYSCHAR, 
  wm_sysdeadchar = WM_SYSDEADCHAR, wm_unichar = WM_UNICHAR, wm_keylast = WM_KEYLAST, wm_ime_startcomposition = WM_IME_STARTCOMPOSITION, 
  wm_ime_endcomposition = WM_IME_ENDCOMPOSITION, wm_ime_composition = WM_IME_COMPOSITION, wm_ime_keylast = WM_IME_KEYLAST, wm_initdialog = WM_INITDIALOG, 
  wm_command = WM_COMMAND, wm_syscommand = WM_SYSCOMMAND, wm_timer = WM_TIMER, wm_hscroll = WM_HSCROLL, wm_vscroll = WM_VSCROLL, wm_initmenu = WM_INITMENU, 
  wm_initmenupopup = WM_INITMENUPOPUP, wm_gesture = WM_GESTURE, wm_gesturenotify = WM_GESTURENOTIFY, wm_menuselect = WM_MENUSELECT, wm_menuchar = WM_MENUCHAR, 
  wm_enteridle = WM_ENTERIDLE, wm_menurbuttonup = WM_MENURBUTTONUP, wm_menudrag = WM_MENUDRAG, wm_menugetobject = WM_MENUGETOBJECT, 
  wm_uninitmenupopup = WM_UNINITMENUPOPUP, wm_menucommand = WM_MENUCOMMAND, wm_changeuistate = WM_CHANGEUISTATE, wm_updateuistate = WM_UPDATEUISTATE, 
  wm_queryuistate = WM_QUERYUISTATE, wm_ctlcolormsgbox = WM_CTLCOLORMSGBOX, wm_ctlcoloredit = WM_CTLCOLOREDIT, wm_ctlcolorlistbox = WM_CTLCOLORLISTBOX, 
  wm_ctlcolorbtn = WM_CTLCOLORBTN, wm_ctlcolordlg = WM_CTLCOLORDLG, wm_ctlcolorscrollbar = WM_CTLCOLORSCROLLBAR, wm_ctlcolorstatic = WM_CTLCOLORSTATIC, 
  mn_gethmenu = MN_GETHMENU, wm_mousefirst = WM_MOUSEFIRST, wm_mousemove = WM_MOUSEMOVE, wm_lbuttondown = WM_LBUTTONDOWN, wm_lbuttonup = WM_LBUTTONUP, 
  wm_lbuttondblclk = WM_LBUTTONDBLCLK, wm_rbuttondown = WM_RBUTTONDOWN, wm_rbuttonup = WM_RBUTTONUP, wm_rbuttondblclk = WM_RBUTTONDBLCLK, 
  wm_mbuttondown = WM_MBUTTONDOWN, wm_mbuttonup = WM_MBUTTONUP, wm_mbuttondblclk = WM_MBUTTONDBLCLK, wm_mousewheel = WM_MOUSEWHEEL, 
  wm_xbuttondown = WM_XBUTTONDOWN, wm_xbuttonup = WM_XBUTTONUP, wm_xbuttondblclk = WM_XBUTTONDBLCLK, wm_mousehwheel = WM_MOUSEHWHEEL, 
  wm_mouselast = WM_MOUSELAST, wm_parentnotify = WM_PARENTNOTIFY, wm_entermenuloop = WM_ENTERMENULOOP, wm_exitmenuloop = WM_EXITMENULOOP, 
  wm_nextmenu = WM_NEXTMENU, wm_sizing = WM_SIZING, wm_capturechanged = WM_CAPTURECHANGED, wm_moving = WM_MOVING, wm_powerbroadcast = WM_POWERBROADCAST, 
  wm_devicechange = WM_DEVICECHANGE, wm_mdicreate = WM_MDICREATE, wm_mdidestroy = WM_MDIDESTROY, wm_mdiactivate = WM_MDIACTIVATE, 
  wm_mdirestore = WM_MDIRESTORE, wm_mdinext = WM_MDINEXT, wm_mdimaximize = WM_MDIMAXIMIZE, wm_mditile = WM_MDITILE, wm_mdicascade = WM_MDICASCADE, 
  wm_mdiiconarrange = WM_MDIICONARRANGE, wm_mdigetactive = WM_MDIGETACTIVE, wm_mdisetmenu = WM_MDISETMENU, wm_entersizemove = WM_ENTERSIZEMOVE, 
  wm_exitsizemove = WM_EXITSIZEMOVE, wm_dropfiles = WM_DROPFILES, wm_mdirefreshmenu = WM_MDIREFRESHMENU, wm_pointerdevicechange = WM_POINTERDEVICECHANGE, 
  wm_pointerdeviceinrange = WM_POINTERDEVICEINRANGE, wm_pointerdeviceoutofrange = WM_POINTERDEVICEOUTOFRANGE, wm_touch = WM_TOUCH, 
  wm_ncpointerupdate = WM_NCPOINTERUPDATE, wm_ncpointerdown = WM_NCPOINTERDOWN, wm_ncpointerup = WM_NCPOINTERUP, wm_pointerupdate = WM_POINTERUPDATE, 
  wm_pointerdown = WM_POINTERDOWN, wm_pointerup = WM_POINTERUP, wm_pointerenter = WM_POINTERENTER, wm_pointerleave = WM_POINTERLEAVE, 
  wm_pointeractivate = WM_POINTERACTIVATE, wm_pointercapturechanged = WM_POINTERCAPTURECHANGED, wm_touchhittesting = WM_TOUCHHITTESTING, 
  wm_pointerwheel = WM_POINTERWHEEL, wm_pointerhwheel = WM_POINTERHWHEEL, dm_pointerhittest = DM_POINTERHITTEST, wm_pointerroutedto = WM_POINTERROUTEDTO, 
  wm_pointerroutedaway = WM_POINTERROUTEDAWAY, wm_pointerroutedreleased = WM_POINTERROUTEDRELEASED, wm_ime_setcontext = WM_IME_SETCONTEXT, 
  wm_ime_notify = WM_IME_NOTIFY, wm_ime_control = WM_IME_CONTROL, wm_ime_compositionfull = WM_IME_COMPOSITIONFULL, wm_ime_select = WM_IME_SELECT, 
  wm_ime_char = WM_IME_CHAR, wm_ime_request = WM_IME_REQUEST, wm_ime_keydown = WM_IME_KEYDOWN, wm_ime_keyup = WM_IME_KEYUP, wm_mousehover = WM_MOUSEHOVER, 
  wm_mouseleave = WM_MOUSELEAVE, wm_ncmousehover = WM_NCMOUSEHOVER, wm_ncmouseleave = WM_NCMOUSELEAVE, wm_wtssession_change = WM_WTSSESSION_CHANGE, 
  wm_tablet_first = WM_TABLET_FIRST, wm_tablet_last = WM_TABLET_LAST, wm_dpichanged = WM_DPICHANGED, wm_dpichanged_beforeparent = WM_DPICHANGED_BEFOREPARENT, 
  wm_dpichanged_afterparent = WM_DPICHANGED_AFTERPARENT, wm_getdpiscaledsize = WM_GETDPISCALEDSIZE, wm_cut = WM_CUT, wm_copy = WM_COPY, wm_paste = WM_PASTE, 
  wm_clear = WM_CLEAR, wm_undo = WM_UNDO, wm_renderformat = WM_RENDERFORMAT, wm_renderallformats = WM_RENDERALLFORMATS, 
  wm_destroyclipboard = WM_DESTROYCLIPBOARD, wm_drawclipboard = WM_DRAWCLIPBOARD, wm_paintclipboard = WM_PAINTCLIPBOARD, 
  wm_vscrollclipboard = WM_VSCROLLCLIPBOARD, wm_sizeclipboard = WM_SIZECLIPBOARD, wm_askcbformatname = WM_ASKCBFORMATNAME, wm_changecbchain = WM_CHANGECBCHAIN, 
  wm_hscrollclipboard = WM_HSCROLLCLIPBOARD, wm_querynewpalette = WM_QUERYNEWPALETTE, wm_paletteischanging = WM_PALETTEISCHANGING, 
  wm_palettechanged = WM_PALETTECHANGED, wm_hotkey = WM_HOTKEY, wm_print = WM_PRINT, wm_printclient = WM_PRINTCLIENT, wm_appcommand = WM_APPCOMMAND, 
  wm_themechanged = WM_THEMECHANGED, wm_clipboardupdate = WM_CLIPBOARDUPDATE, wm_dwmcompositionchanged = WM_DWMCOMPOSITIONCHANGED, 
  wm_dwmncrenderingchanged = WM_DWMNCRENDERINGCHANGED, wm_dwmcolorizationcolorchanged = WM_DWMCOLORIZATIONCOLORCHANGED, 
  wm_dwmwindowmaximizedchange = WM_DWMWINDOWMAXIMIZEDCHANGE, wm_dwmsendiconicthumbnail = WM_DWMSENDICONICTHUMBNAIL, 
  wm_dwmsendiconiclivepreviewbitmap = WM_DWMSENDICONICLIVEPREVIEWBITMAP, wm_gettitlebarinfoex = WM_GETTITLEBARINFOEX, wm_handheldfirst = WM_HANDHELDFIRST, 
  wm_handheldlast = WM_HANDHELDLAST, wm_affirst = WM_AFXFIRST, wm_aflast = WM_AFXLAST, wm_penwinfirst = WM_PENWINFIRST, wm_penwinlast = WM_PENWINLAST, 
  wm_app = WM_APP, wm_user = WM_USER,
};
using enum wmessage;
constexpr auto operator|(wmessage a, wmessage b) { return wmessage(unsigned(a) | unsigned(b)); }
constexpr auto operator&(wmessage a, wmessage b) { return wmessage(unsigned(a) & unsigned(b)); }
constexpr auto operator^(wmessage a, wmessage b) { return wmessage(unsigned(a) ^ unsigned(b)); }
constexpr auto operator~(wmessage a) { return wmessage(~unsigned(a)); }

////////////////////////////////////////////////////////////////////////////////
//  DEFAULT WINDOW USER DATA

struct default_window_userdata {
  HWND hwnd;
  int pad_x, pad_y;
  bool (*userproc)(window&, wmessage, unsigned long long wp, long long lp);
  MSG msg;
};

////////////////////////////////////////////////////////////////////////////////
//  DEFAULT WINDOW PROCEDURE

LRESULT CALLBACK default_window_procedure(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
  auto userdata = (default_window_userdata*)::GetWindowLongPtrW(hw, GWLP_USERDATA);
  if (userdata && userdata->userproc) 
    if (userdata->userproc(*reinterpret_cast<window*>(userdata), wmessage(msg), wp, lp)) return 0;
  return DefWindowProcW(hw, msg, wp, lp);
}

////////////////////////////////////////////////////////////////////////////////
//  DEFAULT WINDOW CLASS

inline const wchar_t* const default_window_class = [] {
  WNDCLASSEXW wc = {sizeof(WNDCLASSEXW)};
  
}();

class window {
  HWND handle{};
  int pad_x{}, pad_y{};
  bool (*userproc)(window&, wmessage, unsigned long long wp, long long lp){};
  MSG msg{};
public:

  window() : handle(nullptr) {}
  window(HWND handle) : handle(handle) {}
  window(const window&) = delete;
  window(window&& other) : handle(std::exchange(other.handle, nullptr)) {}
  ~window() { if (handle) DestroyWindow(handle); }

  window& operator=(const window&) = delete;
  window& operator=(window&& other) {
    if (handle) DestroyWindow(handle);
    handle = std::exchange(other.handle, nullptr);
    return *this;
  }
};

inline window create_window( 
  const string& title, 
  nullable<vector2> position = {}, 
  nullable<vector2> size = {}, 
  nullable<long long> style = {}, 
  nullable<bool> visible = true, 
  nullable<bool> frame = true) {
  auto title_w = to_wstring(title);
  unsigned long s = visible ? WS_VISIBLE : 0;
  if (frame) s |= WS_CAPTION | WS_SYSMENU;
  int x = position ? static_cast<int>(position.value.x) : CW_USEDEFAULT;
  int y = position ? static_cast<int>(position.value.y) : CW_USEDEFAULT;
  int w = size ? static_cast<int>(size.value.x) : CW_USEDEFAULT;
  int h = size ? static_cast<int>(size.value.y) : CW_USEDEFAULT;
  auto hw = CreateWindowExW( 
    WS_EX_ACCEPTFILES, L"STATIC", title_w.c_str(), s, 
    x, y, w, h, nullptr, nullptr, hinstance, nullptr);
  return window(hw);
}

} 