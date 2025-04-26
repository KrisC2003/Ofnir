#pragma once

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <qt_windows.h> 

struct Hotkey {
    quint32 keycode;
    quint32 modifier;

    bool operator==(const Hotkey& other) const {
        return keycode == other.keycode && modifier == other.modifier;
    }
};
class globalHotkeyFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit globalHotkeyFilter(QObject* parent = nullptr);
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;
    bool registerShortcut();
    bool unregisterShortcut();
protected:
    quint32 nativeKeycode(Qt::Key keycode);
    quint32 getSpecialVirtualKeyCode(Qt::Key keycode);
    quint32 nativeModifier(Qt::KeyboardModifiers modifier);

private:
    Hotkey m_savedHotkey;

signals:
    void hotkeyPressed();
};
