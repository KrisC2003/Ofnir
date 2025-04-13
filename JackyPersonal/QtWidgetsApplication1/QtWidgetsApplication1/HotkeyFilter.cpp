#include "HotkeyFilter.h"

HotkeyFilter::HotkeyFilter(QObject* parent) : QObject(parent) {}

bool HotkeyFilter::nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result)
{
    Q_UNUSED(eventType)
        Q_UNUSED(result)

        MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY && msg->wParam == 1001) {
        emit hotkeyPressed();
        return true;
    }
    return false;
}
