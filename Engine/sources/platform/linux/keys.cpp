#include "platform/linux/keys.hpp"
#include <X11/keysym.h>

namespace Linux{

KeySym KeyCodeToXKeySym(Key code){
    switch (code){
        case Key::Unknown: return 0;
        case Key::Space: return XK_space;
        case Key::Quote: return XK_apostrophe;
        case Key::Comma: return XK_comma;
        case Key::Minus: return XK_minus; 
        case Key::Period: return XK_period; 
        case Key::Slash: return XK_slash; 
        case Key::Key_0: return XK_0; 
        case Key::Key_1: return XK_1; 
        case Key::Key_2: return XK_2; 
        case Key::Key_3: return XK_3; 
        case Key::Key_4: return XK_4; 
        case Key::Key_5: return XK_5; 
        case Key::Key_6: return XK_6; 
        case Key::Key_7: return XK_7; 
        case Key::Key_8: return XK_8; 
        case Key::Key_9: return XK_9; 
        case Key::Semicolon: return XK_semicolon; 
        case Key::Equal: return XK_equal; 
        case Key::A: return XK_A; 
        case Key::B: return XK_B; 
        case Key::C: return XK_C; 
        case Key::D: return XK_D; 
        case Key::E: return XK_E; 
        case Key::F: return XK_F; 
        case Key::G: return XK_G; 
        case Key::H: return XK_H; 
        case Key::I: return XK_I; 
        case Key::J: return XK_J; 
        case Key::K: return XK_K; 
        case Key::L: return XK_L; 
        case Key::M: return XK_M; 
        case Key::N: return XK_N; 
        case Key::O: return XK_O; 
        case Key::P: return XK_P; 
        case Key::Q: return XK_Q; 
        case Key::R: return XK_R; 
        case Key::S: return XK_S; 
        case Key::T: return XK_T; 
        case Key::U: return XK_U; 
        case Key::V: return XK_V; 
        case Key::W: return XK_W; 
        case Key::X: return XK_X; 
        case Key::Y: return XK_Y; 
        case Key::Z: return XK_Z; 
        case Key::LeftBracket: return XK_bracketleft; 
        case Key::Backslash: return XK_backslash;
        case Key::RightBracket: return XK_bracketright;
        case Key::GraveAccent: return XK_grave;
        case Key::Escape: return XK_Escape; 
        case Key::Enter: return XK_Return; 
        case Key::Tab: return XK_Tab; 
        case Key::Backspace: return XK_BackSpace; 
        case Key::Insert: return XK_Insert; 
        case Key::Delete: return XK_Delete; 
        case Key::Right: return XK_Right; 
        case Key::Left: return XK_Left; 
        case Key::Down: return XK_Down; 
        case Key::Up: return XK_Up; 
        case Key::PageUp: return XK_Page_Up; 
        case Key::PageDown: return XK_Page_Down; 
        case Key::Home: return XK_Home; 
        case Key::End: return XK_End; 
        case Key::CapsLock: return XK_Caps_Lock; 
        case Key::ScrollLock: return XK_Scroll_Lock; 
        case Key::NumLock: return XK_Num_Lock; 
        case Key::PrintScreen: return XK_Print; 
        case Key::Pause: return XK_Pause; 
        case Key::F1: return XK_F1; 
        case Key::F2: return XK_F2;
        case Key::F3: return XK_F3;
        case Key::F4: return XK_F4;
        case Key::F5: return XK_F5;
        case Key::F6: return XK_F6;
        case Key::F7: return XK_F7;
        case Key::F8: return XK_F8;
        case Key::F9: return XK_F9;
        case Key::F10: return XK_F10;
        case Key::F11: return XK_F11;
        case Key::F12: return XK_F12;
        case Key::F13: return XK_F13;
        case Key::F14: return XK_F14;
        case Key::F15: return XK_F15;
        case Key::F16: return XK_F16;
        case Key::F17: return XK_F17;
        case Key::F18: return XK_F18;
        case Key::F19: return XK_F19;
        case Key::F20: return XK_F20;
        case Key::F21: return XK_F21;
        case Key::F22: return XK_F22;
        case Key::F23: return XK_F23;
        case Key::F24: return XK_F24;
        case Key::Keypad_0: return XK_KP_0; 
        case Key::Keypad_1: return XK_KP_1;
        case Key::Keypad_2: return XK_KP_2; 
        case Key::Keypad_3: return XK_KP_3; 
        case Key::Keypad_4: return XK_KP_4; 
        case Key::Keypad_5: return XK_KP_5; 
        case Key::Keypad_6: return XK_KP_6; 
        case Key::Keypad_7: return XK_KP_7; 
        case Key::Keypad_8: return XK_KP_8; 
        case Key::Keypad_9: return XK_KP_9; 
        case Key::KeypadDecimal: return XK_KP_Decimal; 
        case Key::KeypadDivide: return XK_KP_Divide; 
        case Key::KeypadMultiply: return XK_KP_Multiply; 
        case Key::KeypadSubstract: return XK_KP_Subtract; 
        case Key::KeypadAdd: return XK_KP_Add; 
        case Key::KeypadEnter: return XK_KP_Enter; 
        case Key::KeypadEqual: return XK_KP_Equal; 
        case Key::LeftShift: return XK_Shift_L; 
        case Key::LeftControl: return XK_Control_L; 
        case Key::LeftAlt: return XK_Alt_L; 
        case Key::LeftSuper: return XK_Super_L; 
        case Key::RightShift: return XK_Shift_R; 
        case Key::RightControl: return XK_Control_R; 
        case Key::RightAlt: return XK_Alt_R; 
        case Key::RightSuper: return XK_Super_R; 
        default: return 0;
    }
}

Key XKeyCodeToKeyCode(unsigned int keyCode){
    switch (keyCode){
        case XK_space: return Key::Space;
        case XK_apostrophe: return Key::Quote;
        case XK_comma: return Key::Comma;
        case XK_minus: return Key::Minus;
        case XK_period: return Key::Period;
        case XK_slash: return Key::Slash;
        case XK_0: return Key::Key_0;
        case XK_1: return Key::Key_1;
        case XK_2: return Key::Key_2;
        case XK_3: return Key::Key_3;
        case XK_4: return Key::Key_4;
        case XK_5: return Key::Key_5;
        case XK_6: return Key::Key_6;
        case XK_7: return Key::Key_7;
        case XK_8: return Key::Key_8;
        case XK_9: return Key::Key_9;
        case XK_semicolon: return Key::Semicolon;
        case XK_equal: return Key::Equal;
        case XK_A: return Key::A;
        case XK_B: return Key::B;
        case XK_C: return Key::C;
        case XK_D: return Key::D;
        case XK_E: return Key::E;
        case XK_F: return Key::F;
        case XK_G: return Key::G;
        case XK_H: return Key::H;
        case XK_I: return Key::I;
        case XK_J: return Key::J;
        case XK_K: return Key::K;
        case XK_L: return Key::L;
        case XK_M: return Key::M;
        case XK_N: return Key::N;
        case XK_O: return Key::O;
        case XK_P: return Key::P;
        case XK_Q: return Key::Q;
        case XK_R: return Key::R;
        case XK_S: return Key::S;
        case XK_T: return Key::T;
        case XK_U: return Key::U;
        case XK_V: return Key::V;
        case XK_W: return Key::W;
        case XK_X: return Key::X;
        case XK_Y: return Key::Y;
        case XK_Z: return Key::Z;
        case XK_bracketleft: return Key::LeftBracket;
        case XK_backslash: return Key::Backslash;
        case XK_bracketright: return Key::RightBracket;
        case XK_grave: return Key::GraveAccent;
        case XK_Escape: return Key::Escape;
        case XK_Return: return Key::Enter;
        case XK_Tab: return Key::Tab;
        case XK_BackSpace: return Key::Backspace;
        case XK_Insert: return Key::Insert;
        case XK_Delete: return Key::Delete;
        case XK_Right: return Key::Right;
        case XK_Left: return Key::Left;
        case XK_Down: return Key::Down;
        case XK_Up: return Key::Up;
        case XK_Page_Up: return Key::PageUp;
        case XK_Page_Down: return Key::PageDown;
        case XK_Home: return Key::Home;
        case XK_End: return Key::End;
        case XK_Caps_Lock: return Key::CapsLock;
        case XK_Scroll_Lock: return Key::ScrollLock;
        case XK_Num_Lock: return Key::NumLock;
        case XK_Print: return Key::PrintScreen;
        case XK_Pause: return Key::Pause;
        case XK_F1: return Key::F1;
        case XK_F2: return Key::F2;
        case XK_F3: return Key::F3;
        case XK_F4: return Key::F4;
        case XK_F5: return Key::F5;
        case XK_F6: return Key::F6;
        case XK_F7: return Key::F7;
        case XK_F8: return Key::F8;
        case XK_F9: return Key::F9;
        case XK_F10: return Key::F10;
        case XK_F11: return Key::F11;
        case XK_F12: return Key::F12;
        case XK_F13: return Key::F13;
        case XK_F14: return Key::F14;
        case XK_F15: return Key::F15;
        case XK_F16: return Key::F16;
        case XK_F17: return Key::F17;
        case XK_F18: return Key::F18;
        case XK_F19: return Key::F19;
        case XK_F20: return Key::F20;
        case XK_F21: return Key::F21;
        case XK_F22: return Key::F22;
        case XK_F23: return Key::F23;
        case XK_F24: return Key::F24;
        case XK_KP_0: return Key::Keypad_0; 
        case XK_KP_1: return Key::Keypad_1;
        case XK_KP_2: return Key::Keypad_2; 
        case XK_KP_3: return Key::Keypad_3; 
        case XK_KP_4: return Key::Keypad_4; 
        case XK_KP_5: return Key::Keypad_5; 
        case XK_KP_6: return Key::Keypad_6; 
        case XK_KP_7: return Key::Keypad_7; 
        case XK_KP_8: return Key::Keypad_8; 
        case XK_KP_9: return Key::Keypad_9; 
        case XK_KP_Decimal: return Key::KeypadDecimal;
        case XK_KP_Divide: return Key::KeypadDivide; 
        case XK_KP_Multiply: return Key::KeypadMultiply;
        case XK_KP_Subtract: return Key::KeypadSubstract;
        case XK_KP_Add: return Key::KeypadAdd;
        case XK_KP_Enter: return Key::KeypadEnter;
        case XK_KP_Equal: return Key::KeypadEqual;
        case XK_Shift_L: return Key::LeftShift;
        case XK_Control_L: return Key::LeftControl;
        case XK_Alt_L: return Key::LeftAlt;
        case XK_Super_L: return Key::LeftSuper;
        case XK_Shift_R: return Key::RightShift;
        case XK_Control_R: return Key::RightControl;
        case XK_Alt_R: return Key::RightAlt;
        case XK_Super_R: return Key::RightSuper;
        case XK_Menu: return Key::Menu;
        default: return Key::Unknown;
    }
}


Mouse::Button XButtonToMouseButton(unsigned int button){
    switch(button)
    {
        case Button1: return Mouse::Left;
        case Button2: return Mouse::Middle;   
        case Button3: return Mouse::Right;    
        case 8:       return Mouse::XButton1; 
        case 9:       return Mouse::XButton2; 
        default: return Mouse::Unknown;
    }
}

}// namespace Linux::
