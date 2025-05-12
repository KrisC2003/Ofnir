#include "globalHotkeyFilter.h"


GlobalHotkeyFilter::GlobalHotkeyFilter(QObject* parent) : QObject(parent) {

}

// Update QLabel to display current hotkey
void GlobalHotkeyFilter::updateHotkeyDisplay() {
    QString hotkeyText = getHotkeyDisplayText(m_savedHotkey.keycode, m_savedHotkey.modifier);
    emit hotkeyUpdated(hotkeyText); // Signal to update the label
}

QString GlobalHotkeyFilter::getHotkeyDisplayText(quint32 keycode, quint32 modifiers) {
    QStringList modifierNames;

    if (modifiers & MOD_SHIFT) {
        modifierNames << "Shift";
    }
    if (modifiers & MOD_CONTROL) {
        modifierNames << "Ctrl";
    }
    if (modifiers & MOD_ALT) {
        modifierNames << "Alt";
    }
    if (modifiers & MOD_WIN) {
        modifierNames << "Win";
    }

    // Get the key sequence from the keycode
    QKeySequence keySeq(static_cast<int>(keycode));
    QString keyText = keySeq.toString(QKeySequence::NativeText);

    return modifierNames.join("+") + "+" + keyText;
}

bool GlobalHotkeyFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
    Q_UNUSED(eventType)
        Q_UNUSED(result)

        MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY) {
        const quint32 keycode = HIWORD(msg->lParam);
        const quint32 modifiers = LOWORD(msg->lParam);

        if (keycode == m_savedHotkey.keycode && modifiers == m_savedHotkey.modifier) {
            emit hotkeyPressed();
            return true;
        }
    }
    return false;
}

// converts qt key to virtual key for hotkey for when we need to save stuff
quint32 GlobalHotkeyFilter::nativeKeycode(Qt::Key keycode) {
    HKL hkl = GetKeyboardLayout(0); // gets current keyboard layout
    if (keycode <= 0xFFFF) {
        const SHORT vKey = VkKeyScanExW(static_cast<WCHAR>(keycode), hkl);
        if (vKey > -1) {
            return LOBYTE(vKey);
        }
    }
    return getSpecialVirtualKeyCode(keycode);
}

quint32 GlobalHotkeyFilter::nativeModifier(Qt::KeyboardModifiers modifier) {
    quint32 bitmask = 0;
    if (modifier & Qt::ShiftModifier)
        bitmask |= MOD_SHIFT;
    if (modifier & Qt::ControlModifier)
        bitmask |= MOD_CONTROL;
    if (modifier & Qt::AltModifier)
        bitmask |= MOD_ALT;
    if (modifier & Qt::MetaModifier)
        bitmask |= MOD_WIN;
    return bitmask;
}
// for special keys that arent just the basic keys
quint32 GlobalHotkeyFilter::getSpecialVirtualKeyCode(Qt::Key keycode) {
    static QMap<Qt::Key, quint32> keyMap = {
        {Qt::Key_Escape, VK_ESCAPE},
        {Qt::Key_Tab, VK_TAB},
        {Qt::Key_Backtab, VK_TAB},
        {Qt::Key_Backspace, VK_BACK},
        {Qt::Key_Return, VK_RETURN},
        {Qt::Key_Enter, VK_RETURN},
        {Qt::Key_Insert, VK_INSERT},
        {Qt::Key_Delete, VK_DELETE},
        {Qt::Key_Pause, VK_PAUSE},
        {Qt::Key_Print, VK_PRINT},
        {Qt::Key_Clear, VK_CLEAR},
        {Qt::Key_Home, VK_HOME},
        {Qt::Key_End, VK_END},
        {Qt::Key_Left, VK_LEFT},
        {Qt::Key_Up, VK_UP},
        {Qt::Key_Right, VK_RIGHT},
        {Qt::Key_Down, VK_DOWN},
        {Qt::Key_PageUp, VK_PRIOR},
        {Qt::Key_PageDown, VK_NEXT},
        {Qt::Key_CapsLock, VK_CAPITAL},
        {Qt::Key_NumLock, VK_NUMLOCK},
        {Qt::Key_ScrollLock, VK_SCROLL},
        {Qt::Key_F1, VK_F1},
        {Qt::Key_F2, VK_F2},
        {Qt::Key_F3, VK_F3},
        {Qt::Key_F4, VK_F4},
        {Qt::Key_F5, VK_F5},
        {Qt::Key_F6, VK_F6},
        {Qt::Key_F7, VK_F7},
        {Qt::Key_F8, VK_F8},
        {Qt::Key_F9, VK_F9},
        {Qt::Key_F10, VK_F10},
        {Qt::Key_F11, VK_F11},
        {Qt::Key_F12, VK_F12},
        {Qt::Key_F13, VK_F13},
        {Qt::Key_F14, VK_F14},
        {Qt::Key_F15, VK_F15},
        {Qt::Key_F16, VK_F16},
        {Qt::Key_F17, VK_F17},
        {Qt::Key_F18, VK_F18},
        {Qt::Key_F19, VK_F19},
        {Qt::Key_F20, VK_F20},
        {Qt::Key_F21, VK_F21},
        {Qt::Key_F22, VK_F22},
        {Qt::Key_F23, VK_F23},
        {Qt::Key_F24, VK_F24},
        {Qt::Key_Menu, VK_APPS},
        {Qt::Key_Help, VK_HELP},
        {Qt::Key_MediaNext, VK_MEDIA_NEXT_TRACK},
        {Qt::Key_MediaPrevious, VK_MEDIA_PREV_TRACK},
        {Qt::Key_MediaPlay, VK_MEDIA_PLAY_PAUSE},
        {Qt::Key_MediaStop, VK_MEDIA_STOP},
        {Qt::Key_VolumeDown, VK_VOLUME_DOWN},
        {Qt::Key_VolumeUp, VK_VOLUME_UP},
        {Qt::Key_VolumeMute, VK_VOLUME_MUTE},
        {Qt::Key_Mode_switch, VK_MODECHANGE},
        {Qt::Key_Select, VK_SELECT},
        {Qt::Key_Printer, VK_PRINT},
        {Qt::Key_Execute, VK_EXECUTE},
        {Qt::Key_Sleep, VK_SLEEP},
        {Qt::Key_Period, VK_DECIMAL},
        {Qt::Key_Play, VK_PLAY},
        {Qt::Key_Cancel, VK_CANCEL},
        {Qt::Key_Forward, VK_BROWSER_FORWARD},
        {Qt::Key_Refresh, VK_BROWSER_REFRESH},
        {Qt::Key_Stop, VK_BROWSER_STOP},
        {Qt::Key_Search, VK_BROWSER_SEARCH},
        {Qt::Key_Favorites, VK_BROWSER_FAVORITES},
        {Qt::Key_HomePage, VK_BROWSER_HOME},
        {Qt::Key_LaunchMail, VK_LAUNCH_MAIL},
        {Qt::Key_LaunchMedia, VK_LAUNCH_MEDIA_SELECT},
        {Qt::Key_Launch0, VK_LAUNCH_APP1},
        {Qt::Key_Launch1, VK_LAUNCH_APP2},
        {Qt::Key_Massyo, VK_OEM_FJ_MASSHOU},
        {Qt::Key_Touroku, VK_OEM_FJ_TOUROKU}
    };
    quint32 vkCode = keyMap.value(keycode, 0);
    if (vkCode == 0) {
        // Log or handle the unknown key case if theres a special keycode im missing
        qWarning() << "Unknown key code: " << keycode;
    }
    return vkCode;
}

bool GlobalHotkeyFilter::registerShortcut() {
    m_savedHotkey = { nativeKeycode(Qt::Key_X), nativeModifier(Qt::AltModifier) }; // test temp
    RegisterHotKey(NULL, 1, m_savedHotkey.modifier | MOD_NOREPEAT, m_savedHotkey.keycode);

    updateHotkeyDisplay(); // Update the display after registering the hotkey
    return true;
}
// we only really need 1 hotkey atm so yea
bool GlobalHotkeyFilter::unregisterShortcut() {
    UnregisterHotKey(NULL, 1);
    return true;
}
