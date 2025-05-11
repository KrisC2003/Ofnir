#pragma once
#include <QJsonObject>
#include <QString>

class SettingsManager : public QObject {
    Q_OBJECT
public:
    explicit SettingsManager(const QString& path, QObject* parent = nullptr);
    void save();
    void load();

signals:
    void settingsLoaded(const QJsonObject& settingsMap);

public slots:
    void onSettingChanged(const QString& key, const QVariant& value);
    void onSettingsChanged(const QVariantMap& settingsMap);

private:
    QString m_path;
    QJsonObject m_settings;
};