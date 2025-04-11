#pragma once

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <Windows.h>

class HotkeyFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit HotkeyFilter(QObject* parent = nullptr);
    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

signals:
    void hotkeyPressed();
};
