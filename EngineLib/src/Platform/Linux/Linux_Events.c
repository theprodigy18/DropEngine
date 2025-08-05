#include "pch.h"
#include "Platform/Linux/Linux_Events.h"

#pragma region INTERNAL
static DROP_KEYCODE s_keycodesLookup[256] = {DROP_KEYCODE_UNKNOWN};

DROP_KEYCODE KeySymToKeyCode(KeySym keySym)
{
    // Letters.
    if (keySym >= XK_a && keySym <= XK_z)
        return DROP_KEYCODE_A + (keySym - XK_a);

    if (keySym >= XK_A && keySym <= XK_Z)
        return DROP_KEYCODE_A + (keySym - XK_A);

    // Numbers.
    if (keySym >= XK_0 && keySym <= XK_9)
        return DROP_KEYCODE_0 + (keySym - XK_0);

    // Function keys.
    if (keySym >= XK_F1 && keySym <= XK_F12)
        return DROP_KEYCODE_F1 + (keySym - XK_F1);

    // Numpad numbers.
    if (keySym >= XK_KP_0 && keySym <= XK_KP_9)
        return DROP_KEYCODE_NUMPAD_0 + (keySym - XK_KP_0);

    // Individual mappings.
    switch (keySym)
    {
    // Arrow keys
    case XK_Up:
        return DROP_KEYCODE_ARROW_UP;
    case XK_Down:
        return DROP_KEYCODE_ARROW_DOWN;
    case XK_Left:
        return DROP_KEYCODE_ARROW_LEFT;
    case XK_Right:
        return DROP_KEYCODE_ARROW_RIGHT;

    // Numpad operators
    case XK_KP_Decimal:
        return DROP_KEYCODE_NUMPAD_DECIMAL;
    case XK_KP_Divide:
        return DROP_KEYCODE_NUMPAD_DIVIDE;
    case XK_KP_Multiply:
        return DROP_KEYCODE_NUMPAD_MULTIPLY;
    case XK_KP_Subtract:
        return DROP_KEYCODE_NUMPAD_SUBTRACT;
    case XK_KP_Add:
        return DROP_KEYCODE_NUMPAD_ADD;
    case XK_KP_Enter:
        return DROP_KEYCODE_NUMPAD_ENTER;

    // Modifier keys
    case XK_Shift_L:
        return DROP_KEYCODE_SHIFT_LEFT;
    case XK_Shift_R:
        return DROP_KEYCODE_SHIFT_RIGHT;
    case XK_Control_L:
        return DROP_KEYCODE_CTRL_LEFT;
    case XK_Control_R:
        return DROP_KEYCODE_CTRL_RIGHT;
    case XK_Alt_L:
        return DROP_KEYCODE_ALT_LEFT;
    case XK_Alt_R:
        return DROP_KEYCODE_ALT_RIGHT;
    case XK_Super_L:
        return DROP_KEYCODE_SUPER_LEFT;
    case XK_Super_R:
        return DROP_KEYCODE_SUPER_RIGHT;
    case XK_Meta_L:
        return DROP_KEYCODE_SUPER_LEFT; // Fallback
    case XK_Meta_R:
        return DROP_KEYCODE_SUPER_RIGHT; // Fallback

    // Symbol keys (US QWERTY layout)
    case XK_grave:
        return DROP_KEYCODE_GRAVE; // `
    case XK_asciitilde:
        return DROP_KEYCODE_GRAVE; // ~ (shift+`)
    case XK_minus:
        return DROP_KEYCODE_MINUS; // -
    case XK_underscore:
        return DROP_KEYCODE_MINUS; // _ (shift+-)
    case XK_equal:
        return DROP_KEYCODE_EQUALS; // =
    case XK_plus:
        return DROP_KEYCODE_EQUALS; // + (shift+=)
    case XK_bracketleft:
        return DROP_KEYCODE_BRACKET_LEFT; // [
    case XK_braceleft:
        return DROP_KEYCODE_BRACKET_LEFT; // { (shift+[)
    case XK_bracketright:
        return DROP_KEYCODE_BRACKET_RIGHT; // ]
    case XK_braceright:
        return DROP_KEYCODE_BRACKET_RIGHT; // } (shift+])
    case XK_backslash:
        return DROP_KEYCODE_BACKSLASH; // '\'
    case XK_bar:
        return DROP_KEYCODE_BACKSLASH; // | (shift+\)
    case XK_semicolon:
        return DROP_KEYCODE_SEMICOLON; // ;
    case XK_colon:
        return DROP_KEYCODE_SEMICOLON; // : (shift+;)
    case XK_apostrophe:
        return DROP_KEYCODE_APOSTROPHE; // '
    case XK_quotedbl:
        return DROP_KEYCODE_APOSTROPHE; // " (shift+')
    case XK_comma:
        return DROP_KEYCODE_COMMA; // ,
    case XK_less:
        return DROP_KEYCODE_COMMA; // < (shift+,)
    case XK_period:
        return DROP_KEYCODE_PERIOD; // .
    case XK_greater:
        return DROP_KEYCODE_PERIOD; // > (shift+.)
    case XK_slash:
        return DROP_KEYCODE_SLASH; // /
    case XK_question:
        return DROP_KEYCODE_SLASH; // ? (shift+/)

    // Special keys
    case XK_Escape:
        return DROP_KEYCODE_ESCAPE;
    case XK_Tab:
        return DROP_KEYCODE_TAB;
    case XK_Caps_Lock:
        return DROP_KEYCODE_CAPSLOCK;
    case XK_space:
        return DROP_KEYCODE_SPACE;
    case XK_Return:
        return DROP_KEYCODE_ENTER;
    case XK_BackSpace:
        return DROP_KEYCODE_BACKSPACE;
    case XK_Delete:
        return DROP_KEYCODE_DELETE;
    case XK_Insert:
        return DROP_KEYCODE_INSERT;
    case XK_Home:
        return DROP_KEYCODE_HOME;
    case XK_End:
        return DROP_KEYCODE_END;
    case XK_Page_Up:
        return DROP_KEYCODE_PAGE_UP;
    case XK_Page_Down:
        return DROP_KEYCODE_PAGE_DOWN;
    case XK_Print:
        return DROP_KEYCODE_PRINT_SCREEN;
    case XK_Scroll_Lock:
        return DROP_KEYCODE_SCROLL_LOCK;
    case XK_Pause:
        return DROP_KEYCODE_PAUSE;
    case XK_Menu:
        return DROP_KEYCODE_MENU;
    case XK_Num_Lock:
        return DROP_KEYCODE_NUM_LOCK;

    default:
        return DROP_KEYCODE_UNKNOWN;
    }
}

#pragma endregion

void FillKeyCodesLookupTable(Display* pDisplay)
{
    // Fill lookup table for each possible keycode.
    for (int keycode = 8; keycode < 256; ++keycode)
    {
        KeySym keySym = XkbKeycodeToKeysym(pDisplay, keycode, 0, 0);
        if (keySym != NoSymbol)
        {
            s_keycodesLookup[keycode] = KeySymToKeyCode(keySym);
        }
    }
}

DROP_KEYCODE GetKeycode(i32 keycode)
{
    if (keycode < 0 || keycode >= 256)
    {
        ASSERT_MSG(false, "Invalid keycode.");
        return DROP_KEYCODE_UNKNOWN;
    }
    return s_keycodesLookup[keycode];
}

const char* KeyCodeToString(DROP_KEYCODE keycode)
{
    switch (keycode)
    {
    case DROP_KEYCODE_A:
        return "A";
    case DROP_KEYCODE_B:
        return "B";
    case DROP_KEYCODE_C:
        return "C";
    case DROP_KEYCODE_D:
        return "D";
    case DROP_KEYCODE_E:
        return "E";
    case DROP_KEYCODE_F:
        return "F";
    case DROP_KEYCODE_G:
        return "G";
    case DROP_KEYCODE_H:
        return "H";
    case DROP_KEYCODE_I:
        return "I";
    case DROP_KEYCODE_J:
        return "J";
    case DROP_KEYCODE_K:
        return "K";
    case DROP_KEYCODE_L:
        return "L";
    case DROP_KEYCODE_M:
        return "M";
    case DROP_KEYCODE_N:
        return "N";
    case DROP_KEYCODE_O:
        return "O";
    case DROP_KEYCODE_P:
        return "P";
    case DROP_KEYCODE_Q:
        return "Q";
    case DROP_KEYCODE_R:
        return "R";
    case DROP_KEYCODE_S:
        return "S";
    case DROP_KEYCODE_T:
        return "T";
    case DROP_KEYCODE_U:
        return "U";
    case DROP_KEYCODE_V:
        return "V";
    case DROP_KEYCODE_W:
        return "W";
    case DROP_KEYCODE_X:
        return "X";
    case DROP_KEYCODE_Y:
        return "Y";
    case DROP_KEYCODE_Z:
        return "Z";
    case DROP_KEYCODE_0:
        return "0";
    case DROP_KEYCODE_1:
        return "1";
    case DROP_KEYCODE_2:
        return "2";
    case DROP_KEYCODE_3:
        return "3";
    case DROP_KEYCODE_4:
        return "4";
    case DROP_KEYCODE_5:
        return "5";
    case DROP_KEYCODE_6:
        return "6";
    case DROP_KEYCODE_7:
        return "7";
    case DROP_KEYCODE_8:
        return "8";
    case DROP_KEYCODE_9:
        return "9";
    case DROP_KEYCODE_SPACE:
        return "Space";
    case DROP_KEYCODE_ENTER:
        return "Enter";
    case DROP_KEYCODE_ESCAPE:
        return "Escape";
    case DROP_KEYCODE_BACKSPACE:
        return "Backspace";
    case DROP_KEYCODE_TAB:
        return "Tab";
    case DROP_KEYCODE_SHIFT_LEFT:
        return "Shift_L";
    case DROP_KEYCODE_SHIFT_RIGHT:
        return "Shift_R";
    case DROP_KEYCODE_CTRL_LEFT:
        return "Ctrl_L";
    case DROP_KEYCODE_CTRL_RIGHT:
        return "Ctrl_R";
    case DROP_KEYCODE_ALT_LEFT:
        return "Alt_L";
    case DROP_KEYCODE_ALT_RIGHT:
        return "Alt_R";
    case DROP_KEYCODE_ARROW_UP:
        return "Up";
    case DROP_KEYCODE_ARROW_DOWN:
        return "Down";
    case DROP_KEYCODE_ARROW_LEFT:
        return "Left";
    case DROP_KEYCODE_ARROW_RIGHT:
        return "Right";
    case DROP_KEYCODE_F1:
        return "F1";
    case DROP_KEYCODE_F2:
        return "F2";
    case DROP_KEYCODE_F3:
        return "F3";
    case DROP_KEYCODE_F4:
        return "F4";
    case DROP_KEYCODE_F5:
        return "F5";
    case DROP_KEYCODE_F6:
        return "F6";
    case DROP_KEYCODE_F7:
        return "F7";
    case DROP_KEYCODE_F8:
        return "F8";
    case DROP_KEYCODE_F9:
        return "F9";
    case DROP_KEYCODE_F10:
        return "F10";
    case DROP_KEYCODE_F11:
        return "F11";
    case DROP_KEYCODE_F12:
        return "F12";
    default:
        return "Unknown";
    }
}
