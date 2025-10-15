#include "keyboard.h"
#include <windows.h>
#include <vector>
#include <unordered_map>
#include "str.h"

using namespace std;

namespace keyboard {

    struct VKN {
        std::string name;
        unsigned long code;
    };

    vector<VKN> KeyNames{
        { "up", VK_UP },
        { "down", VK_DOWN },
        { "left", VK_LEFT },
        { "right", VK_RIGHT },
        { "enter", VK_RETURN },
        { "ctrl", VK_LCONTROL },
        { "shift", VK_SHIFT },
        { "alt", VK_LMENU },
        { "win", VK_LWIN },
    };

    const std::unordered_map<unsigned long, std::string> VkCodeString = {
            {0x01, "VK_LBUTTON"},
            {0x02, "VK_RBUTTON"},
            {0x03, "VK_CANCEL"},
            {0x04, "VK_MBUTTON"},
            {0x05, "VK_XBUTTON1"},
            {0x06, "VK_XBUTTON2"},
            {0x08, "VK_BACK"},
            {0x09, "VK_TAB"},
            {0x0C, "VK_CLEAR"},
            {0x0D, "VK_RETURN"},
            {0x10, "VK_SHIFT"},
            {0x11, "VK_CONTROL"},
            {0x12, "VK_MENU"},
            {0x13, "VK_PAUSE"},
            {0x14, "VK_CAPITAL"},
            {0x15, "VK_KANA"},
            {0x15, "VK_HANGUEL"},
            {0x15, "VK_HANGUL"},
            {0x17, "VK_JUNJA"},
            {0x18, "VK_FINAL"},
            {0x19, "VK_HANJA"},
            {0x19, "VK_KANJI"},
            {0x1B, "VK_ESCAPE"},
            {0x1C, "VK_CONVERT"},
            {0x1D, "VK_NONCONVERT"},
            {0x1E, "VK_ACCEPT"},
            {0x1F, "VK_MODECHANGE"},
            {0x20, "VK_SPACE"},
            {0x21, "VK_PRIOR"},
            {0x22, "VK_NEXT"},
            {0x23, "VK_END"},
            {0x24, "VK_HOME"},
            {0x25, "VK_LEFT"},
            {0x26, "VK_UP"},
            {0x27, "VK_RIGHT"},
            {0x28, "VK_DOWN"},
            {0x29, "VK_SELECT"},
            {0x2A, "VK_PRINT"},
            {0x2B, "VK_EXECUTE"},
            {0x2C, "VK_SNAPSHOT"},
            {0x2D, "VK_INSERT"},
            {0x2E, "VK_DELETE"},
            {0x2F, "VK_HELP"},
            {0x30, "VK_0"},
            {0x31, "VK_1"},
            {0x32, "VK_2"},
            {0x33, "VK_3"},
            {0x34, "VK_4"},
            {0x35, "VK_5"},
            {0x36, "VK_6"},
            {0x37, "VK_7"},
            {0x38, "VK_8"},
            {0x39, "VK_9"},
            {0x41, "VK_A"},
            {0x42, "VK_B"},
            {0x43, "VK_C"},
            {0x44, "VK_D"},
            {0x45, "VK_E"},
            {0x46, "VK_F"},
            {0x47, "VK_G"},
            {0x48, "VK_H"},
            {0x49, "VK_I"},
            {0x4A, "VK_J"},
            {0x4B, "VK_K"},
            {0x4C, "VK_L"},
            {0x4D, "VK_M"},
            {0x4E, "VK_N"},
            {0x4F, "VK_O"},
            {0x50, "VK_P"},
            {0x51, "VK_Q"},
            {0x52, "VK_R"},
            {0x53, "VK_S"},
            {0x54, "VK_T"},
            {0x55, "VK_U"},
            {0x56, "VK_V"},
            {0x57, "VK_W"},
            {0x58, "VK_X"},
            {0x59, "VK_Y"},
            {0x5A, "VK_Z"},
            {0x5B, "VK_LWIN"},
            {0x5C, "VK_RWIN"},
            {0x5D, "VK_APPS"},
            {0x5F, "VK_SLEEP"},
            {0x60, "VK_NUMPAD0"},
            {0x61, "VK_NUMPAD1"},
            {0x62, "VK_NUMPAD2"},
            {0x63, "VK_NUMPAD3"},
            {0x64, "VK_NUMPAD4"},
            {0x65, "VK_NUMPAD5"},
            {0x66, "VK_NUMPAD6"},
            {0x67, "VK_NUMPAD7"},
            {0x68, "VK_NUMPAD8"},
            {0x69, "VK_NUMPAD9"},
            {0x6A, "VK_MULTIPLY"},
            {0x6B, "VK_ADD"},
            {0x6C, "VK_SEPARATOR"},
            {0x6D, "VK_SUBTRACT"},
            {0x6E, "VK_DECIMAL"},
            {0x6F, "VK_DIVIDE"},
            {0x70, "VK_F1"},
            {0x71, "VK_F2"},
            {0x72, "VK_F3"},
            {0x73, "VK_F4"},
            {0x74, "VK_F5"},
            {0x75, "VK_F6"},
            {0x76, "VK_F7"},
            {0x77, "VK_F8"},
            {0x78, "VK_F9"},
            {0x79, "VK_F10"},
            {0x7A, "VK_F11"},
            {0x7B, "VK_F12"},
            {0x7C, "VK_F13"},
            {0x7D, "VK_F14"},
            {0x7E, "VK_F15"},
            {0x7F, "VK_F16"},
            {0x80, "VK_F17"},
            {0x81, "VK_F18"},
            {0x82, "VK_F19"},
            {0x83, "VK_F20"},
            {0x84, "VK_F21"},
            {0x85, "VK_F22"},
            {0x86, "VK_F23"},
            {0x87, "VK_F24"},
            {0x90, "VK_NUMLOCK"},
            {0x91, "VK_SCROLL"},
            {0xA0, "VK_LSHIFT"},
            {0xA1, "VK_RSHIFT"},
            {0xA2, "VK_LCONTROL"},
            {0xA3, "VK_RCONTROL"},
            {0xA4, "VK_LMENU"},
            {0xA5, "VK_RMENU"},
            {0xA6, "VK_BROWSER_BACK"},
            {0xA7, "VK_BROWSER_FORWARD"},
            {0xA8, "VK_BROWSER_REFRESH"},
            {0xA9, "VK_BROWSER_STOP"},
            {0xAA, "VK_BROWSER_SEARCH"},
            {0xAB, "VK_BROWSER_FAVORITES"},
            {0xAC, "VK_BROWSER_HOME"},
            {0xAD, "VK_VOLUME_MUTE"},
            {0xAE, "VK_VOLUME_DOWN"},
            {0xAF, "VK_VOLUME_UP"},
            {0xB0, "VK_MEDIA_NEXT_TRACK"},
            {0xB1, "VK_MEDIA_PREV_TRACK"},
            {0xB2, "VK_MEDIA_STOP"},
            {0xB3, "VK_MEDIA_PLAY_PAUSE"},
            {0xB4, "VK_LAUNCH_MAIL"},
            {0xB5, "VK_LAUNCH_MEDIA_SELECT"},
            {0xB6, "VK_LAUNCH_APP1"},
            {0xB7, "VK_LAUNCH_APP2"},
            {0xBA, "VK_OEM_1"},
            {0xBB, "VK_OEM_PLUS"},
            {0xBC, "VK_OEM_COMMA"},
            {0xBD, "VK_OEM_MINUS"},
            {0xBE, "VK_OEM_PERIOD"},
            {0xBF, "VK_OEM_2"},
            {0xC0, "VK_OEM_3"},
            {0xDB, "VK_OEM_4"},
            {0xDC, "VK_OEM_5"},
            {0xDD, "VK_OEM_6"},
            {0xDE, "VK_OEM_7"},
            {0xDF, "VK_OEM_8"},
            {0xE2, "VK_OEM_102"},
            {0xE5, "VK_PROCESSKEY"},
            {0xE7, "VK_PACKET"},
            {0xF6, "VK_ATTN"},
            {0xF7, "VK_CRSEL"},
            {0xF8, "VK_EXSEL"},
            {0xF9, "VK_EREOF"},
            {0xFA, "VK_PLAY"},
            {0xFB, "VK_ZOOM"},
            {0xFC, "VK_NONAME"},
            {0xFD, "VK_PA1"},
            {0xFE, "VK_OEM_CLEAR"}
    };

    void send_code(unsigned long code, key_direction dir, std::vector<INPUT>& dest) {

        // press the key if direction allows it
        if(dir == key_direction::press || dir == key_direction::down) {
            INPUT key_down = {0};
            key_down.type = INPUT_KEYBOARD;
            key_down.ki.wVk = static_cast<WORD>(code);
            dest.push_back(key_down);
        }

        // release the key if direction allows it
        if(dir == key_direction::press || dir == key_direction::up) {
            INPUT key_up = {0};
            key_up.type = INPUT_KEYBOARD;
            key_up.ki.wVk = static_cast<WORD>(code);
            key_up.ki.dwFlags = KEYEVENTF_KEYUP;
            dest.push_back(key_up);
        }
    }

    void send_char(wchar_t wc, vector<INPUT>& dest) {
        SHORT vk = VkKeyScanW(wc);
        if(vk == -1) {
            return; // Character cannot be typed
        }

        BYTE vk_code = LOBYTE(vk);
        BYTE shift_state = HIBYTE(vk);

        // Press Shift if needed
        if(shift_state & 1) {
            INPUT shift_down = {0};
            shift_down.type = INPUT_KEYBOARD;
            shift_down.ki.wVk = VK_SHIFT;
            dest.push_back(shift_down);
        }

        // press the key
        INPUT key_down = {0};
        key_down.type = INPUT_KEYBOARD;
        key_down.ki.wVk = vk_code;
        dest.push_back(key_down);

        // release the key
        INPUT key_up = {0};
        key_up.type = INPUT_KEYBOARD;
        key_up.ki.wVk = vk_code;
        key_up.ki.dwFlags = KEYEVENTF_KEYUP;
        dest.push_back(key_up);

        // Release Shift if it was pressed
        if(shift_state & 1) {
            INPUT shift_up = {0};
            shift_up.type = INPUT_KEYBOARD;
            shift_up.ki.wVk = VK_SHIFT;
            shift_up.ki.dwFlags = KEYEVENTF_KEYUP;
            dest.push_back(shift_up);
        }
    }
    
    bool is_key_name(const wstring& text, size_t pos,
        string& key_name,
        size_t& key_name_len,
        unsigned long& vk_code, key_direction& dir) {

        if(text[pos] != '{') return false;

        // find the closing bracket
        size_t end_pos = text.find('}', pos);
        if(end_pos == wstring::npos) return false;

        // get capture length
        key_name_len = end_pos - pos + 1;

        // extract the key name, which does NOT include brackets
        key_name = str::to_str(text.substr(pos + 1, end_pos - pos - 1));

        // check if there is "direction" indicator after brackets:
        // '+' - press the key
        // '-' - release the key
        char s_dir = key_name[0];
        if(s_dir == '+' || s_dir == '-') {
            dir = s_dir == '+' ? key_direction::down : key_direction::up;
            // remove the direction indicator
            key_name.erase(0, 1);
        } else {
            dir = key_direction::press;
        }

        // find the key code
        for(auto& kn : KeyNames) {
            if(kn.name == key_name) {
                vk_code = kn.code;
                return true;
            }
        }

        return false;
    }
    
    wchar_t vk_to_wchar(unsigned long vk_code) {

        // GetKeyboardState() do not properly report state for modifier keys if the key event in a window other that one
        // from the current process, so we need to manually fetch the valid states manually using GetKeyState()
        // We do not actually need the state of the other key, so we do not event bother calling GetKeyboardState()


        BYTE ks[256] = {0};
        BYTE keymods[] = {
            VK_SHIFT/*, VK_LSHIFT, VK_RSHIFT,
            VK_CONTROL, VK_LCONTROL, VK_RCONTROL,
            VK_MENU, VK_LMENU, VK_RMENU,
            VK_RWIN, VK_LWIN,
            VK_CAPITAL*/};
        for(auto k : keymods) {
            ks[k] = static_cast<BYTE>(GetKeyState(k));
        }

        WCHAR unicode_char[2];
        UINT flags = 1 << 2 | 1;
        auto scan_code = MapVirtualKey(vk_code, MAPVK_VK_TO_VSC);
        int result = ToUnicode(vk_code, scan_code, ks, unicode_char, 2, flags);
        if(result == 1) {
            return unicode_char[0];
        }

        return L'\0';
    }

    void type_code(unsigned long code, key_direction dir) {

        BYTE vk_code = static_cast<BYTE>(code);
        BYTE scan = MapVirtualKey(vk_code, MAPVK_VK_TO_VSC);

        // Press the key
        if(dir == key_direction::press || dir == key_direction::down)
            keybd_event(vk_code, scan, 0, 0);

        // Release the key
        if(dir == key_direction::press || dir == key_direction::up)
            keybd_event(vk_code, scan, KEYEVENTF_KEYUP, 0);
    }

    void type_char(wchar_t wc) {
        SHORT vk = VkKeyScanW(wc);
        if(vk == -1) {
            return; // Character cannot be typed
        }

        BYTE vk_code = LOBYTE(vk);
        BYTE shift_state = HIBYTE(vk);

        // Press Shift if needed
        if(shift_state & 1) {
            keybd_event(VK_SHIFT, 0, 0, 0);
        }

        type_code(vk_code);

        // Release Shift if it was pressed
        if(shift_state & 1) {
            keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
        }
    }

    void type_backspace(int times, int delay_ms) {

        for(int i = 0; i < times; ++i) {

            type_code(VK_BACK);

            ::Sleep(delay_ms);
        }
    }

    void send_backspace(int times) {
        vector<INPUT> inputs;
        for(int i = 0; i < times; ++i) {
            send_code(VK_BACK, key_direction::press, inputs);
        }
        // use SendInput to send the text
        ::SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }

    void type_text(const wstring& text, int delay_ms) {
        // use win32 api to type text as user
        // this will do: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput#example

        // type each character
        string key_name;
        size_t key_name_len;
        unsigned long vk_code{0};
        key_direction dir;

        for(int i = 0; i < text.size(); ++i) {

            if(is_key_name(text, i, key_name, key_name_len, vk_code, dir)) {
                // type the key
                type_code(vk_code, dir);
                // skip the key nam
                i += key_name_len - 1;
            } else {
                wchar_t c = text[i];
                type_char(c);
            }

            ::Sleep(delay_ms);
        }
    }

    void send_text(const std::wstring& text) {
        // use SendInput to send the text
        // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput#example
        // type each character
        string key_name;
        size_t key_name_len;
        unsigned long vk_code{0};
        key_direction dir;
        vector<INPUT> inputs;

        for(int i = 0; i < text.size(); ++i) {

            if(is_key_name(text, i, key_name, key_name_len, vk_code, dir)) {
                // type the key
                send_code(vk_code, dir, inputs);
                // skip the key name
                i += key_name_len - 1;
            } else {
                wchar_t c = text[i];
                send_char(c, inputs);
            }
        }

        // use SendInput to send the text
        ::SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }

    std::string to_string(unsigned long vk_code) {
        // use VkCodeString to get the string representation of the key code
        auto it = VkCodeString.find(vk_code);
        if(it != VkCodeString.end()) {
            return it->second;
        }
        return "";
    }
}