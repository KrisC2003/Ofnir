#include "FontSettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

FontSettingsDialog::FontSettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");

    m_fontBox = new QFontComboBox(this);
    m_sizeBox = new QSpinBox(this);
    m_sizeBox->setRange(6, 30);
    m_sizeBox->setValue(24);

    m_textColorBtn = new QPushButton("Select Text Color", this);
    m_outlineColorBtn = new QPushButton("Select Outline Color", this);
    m_hotkeyEdit = new QKeySequenceEdit(this);
    m_languageBox = new QComboBox(this);

    
    m_languageBox->addItem(QString::fromUtf8(u8"English"), "en");
    m_languageBox->addItem(QString::fromUtf8(u8"Chinese"), "zh-Hant");
    m_languageBox->addItem(QString::fromUtf8(u8"Spanish"), "es");
    m_languageBox->addItem(QString::fromUtf8(u8"French"), "fr");



    connect(m_textColorBtn, &QPushButton::clicked, this, &FontSettingsDialog::chooseTextColor);
    connect(m_outlineColorBtn, &QPushButton::clicked, this, &FontSettingsDialog::chooseOutlineColor);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* fontLayout = new QHBoxLayout();
    fontLayout->addWidget(new QLabel("Font:"));
    fontLayout->addWidget(m_fontBox);
    fontLayout->addWidget(new QLabel("Size:"));
    fontLayout->addWidget(m_sizeBox);
    mainLayout->addLayout(fontLayout);

    mainLayout->addWidget(m_textColorBtn);
    mainLayout->addWidget(m_outlineColorBtn);

    QHBoxLayout* hotkeyLayout = new QHBoxLayout();
    hotkeyLayout->addWidget(new QLabel("Hotkey:"));
    hotkeyLayout->addWidget(m_hotkeyEdit);
    mainLayout->addLayout(hotkeyLayout);

    QHBoxLayout* languageLayout = new QHBoxLayout();
    languageLayout->addWidget(new QLabel("Output Language:"));
    languageLayout->addWidget(m_languageBox);
    mainLayout->addLayout(languageLayout);

    QLabel* hintLabel = new QLabel("Click the box above and press a new shortcut", this);
    hintLabel->setStyleSheet("color: gray; font-size: 10px;");
    mainLayout->addWidget(hintLabel);

    QLabel* hotkeySavedLabel = new QLabel("Hotkey recorded", this);
    hotkeySavedLabel->setStyleSheet("color: green; font-size: 10px;");
    hotkeySavedLabel->setAlignment(Qt::AlignCenter);
    hotkeySavedLabel->setVisible(false);
    mainLayout->addWidget(hotkeySavedLabel);

    connect(m_hotkeyEdit, &QKeySequenceEdit::editingFinished, this, [=]() {
        hotkeySavedLabel->setVisible(true);

        auto* effect = new QGraphicsOpacityEffect(hotkeySavedLabel);
        hotkeySavedLabel->setGraphicsEffect(effect);

        QPropertyAnimation* fade = new QPropertyAnimation(effect, "opacity");
        fade->setDuration(5000);
        fade->setStartValue(1.0);
        fade->setEndValue(0.0);
        fade->start(QAbstractAnimation::DeleteWhenStopped);

        connect(fade, &QPropertyAnimation::finished, hotkeySavedLabel, [=]() {
            hotkeySavedLabel->setVisible(false);
            hotkeySavedLabel->setGraphicsEffect(nullptr);
            });
        });

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [=]() {
        QString filePath = "user_settings.json";
        saveSettingsToJson(filePath, selectedFont(), textColor(), outlineColor(), selectedHotkey(), selectedLanguage());
        accept();
        });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    // Load settings
    QFont font;
    QColor textColor;
    QColor outlineColor;
    QKeySequence hotkey;
    QString language;
    if (loadSettingsFromJson("user_settings.json", font, textColor, outlineColor, hotkey, language)) {
        m_fontBox->setCurrentFont(font);
        m_sizeBox->setValue(font.pointSize());
        m_textColor = textColor;
        m_outlineColor = outlineColor;
        m_hotkeyEdit->setKeySequence(hotkey);

        
        int index = m_languageBox->findData(language);
        if (index >= 0) m_languageBox->setCurrentIndex(index);
    }
}

void FontSettingsDialog::chooseTextColor()
{
    QColor color = QColorDialog::getColor(m_textColor, this, "Choose Text Color");
    if (color.isValid()) {
        m_textColor = color;
    }
}

void FontSettingsDialog::chooseOutlineColor()
{
    QColor color = QColorDialog::getColor(m_outlineColor, this, "Choose Outline Color");
    if (color.isValid()) {
        m_outlineColor = color;
    }
}

QFont FontSettingsDialog::selectedFont() const
{
    QFont font = m_fontBox->currentFont();
    font.setPointSize(m_sizeBox->value());
    return font;
}

QColor FontSettingsDialog::textColor() const
{
    return m_textColor;
}

QColor FontSettingsDialog::outlineColor() const
{
    return m_outlineColor;
}

QKeySequence FontSettingsDialog::selectedHotkey() const
{
    return m_hotkeyEdit->keySequence();
}


QString FontSettingsDialog::selectedLanguage() const
{
    return m_languageBox->currentData().toString();
}

bool loadSettingsFromJson(const QString& filePath, QFont& font, QColor& textColor, QColor& outlineColor, QKeySequence& hotkey, QString& language) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return false;

    QJsonObject json = doc.object();
    font.setFamily(json["FontFamily"].toString("Arial"));

    //max 30 even user change json
    int loadedSize = json["FontSize"].toInt(24);
    if (loadedSize > 30) loadedSize = 30;
    font.setPointSize(loadedSize);

    textColor = QColor(json["TextColor"].toString("#ffffff"));
    outlineColor = QColor(json["OutlineColor"].toString("#000000"));
    hotkey = QKeySequence(json["Hotkey"].toString("Alt+X"));
    language = json["OutputLanguage"].toString("en");

    return true;
}


bool saveSettingsToJson(const QString& filePath, const QFont& font, const QColor& textColor, const QColor& outlineColor, const QKeySequence& hotkey, const QString& language) {
    QJsonObject json;
    json["FontFamily"] = font.family();
    json["FontSize"] = font.pointSize();
    json["TextColor"] = textColor.name();
    json["OutlineColor"] = outlineColor.name();
    json["Hotkey"] = hotkey.toString();
    json["OutputLanguage"] = language;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
        file.close();
        return true;
    }
    return false;
}
