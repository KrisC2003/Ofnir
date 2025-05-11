#include "settingsmanager.h"
#include <QFile>
#include <QJsonDocument>

SettingsManager::SettingsManager(const QString& path, QObject* parent) 
    : QObject(parent)
    , m_path(path + "/settings.json")
{
    load();
}

void SettingsManager::load() {
    QFile file(m_path);
    if (!file.exists()) {
        qWarning() << "Settings file not found, using defaults.";
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open settings file for reading:" << m_path;
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse settings file:" << parseError.errorString();
        return;
    }

    if (!doc.isObject()) {
        qWarning() << "Settings file does not contain a JSON object.";
        return;
    }
    m_settings = doc.object();
    emit settingsLoaded(m_settings);
}

void SettingsManager::save() {
    QFile file(m_path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open settings file for writing:" << m_path;
        return;
    }
    QJsonDocument doc(m_settings);
    file.write(doc.toJson());
    file.close();
}

void SettingsManager::onSettingChanged(const QString& key, const QVariant& value) {
    qDebug() << "Setting changed1:" << key << "=>" << value;

    // if its something like a fontsetting, passing a variant map will properly parse it
    if (value.type() == QVariant::Map) {
        QJsonObject obj = QJsonObject::fromVariantMap(value.toMap());
        m_settings.insert(key, obj);
    }
    else {
        m_settings.insert(key, QJsonValue::fromVariant(value));
    }
    save();
}

void SettingsManager::onSettingsChanged(const QVariantMap& settingsMap) {
    qDebug() << "Multiple settings updated:" << settingsMap;
    for (auto it = settingsMap.constBegin(); it != settingsMap.constEnd(); ++it) {
        m_settings.insert(it.key(), QJsonValue::fromVariant(it.value()));
    }
    save();
}