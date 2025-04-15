#include "FontSettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
FontSettingsDialog::FontSettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");

    m_fontBox = new QFontComboBox(this);
    m_sizeBox = new QSpinBox(this);
    m_sizeBox->setRange(6, 72);
    m_sizeBox->setValue(24);

    m_textColorBtn = new QPushButton("Select Text Color", this);
    m_outlineColorBtn = new QPushButton("Select Outline Color", this);
    m_hotkeyEdit = new QKeySequenceEdit(this);

    connect(m_textColorBtn, &QPushButton::clicked, this, &FontSettingsDialog::chooseTextColor);
    connect(m_outlineColorBtn, &QPushButton::clicked, this, &FontSettingsDialog::chooseOutlineColor);

    // Layout setup
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

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [=]() {
        QString filePath = "user_settings.json"; 
        saveSettingsToJson(filePath, selectedFont(), textColor(), outlineColor(), selectedHotkey());
        accept();
        });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    QFont font;
    QColor textColor;
    QColor outlineColor;
    QKeySequence hotkey;

    if (loadSettingsFromJson("user_settings.json", font, textColor, outlineColor, hotkey)) {
        m_fontBox->setCurrentFont(font);
        m_sizeBox->setValue(font.pointSize());
        m_textColor = textColor;
        m_outlineColor = outlineColor;
        m_hotkeyEdit->setKeySequence(hotkey);
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

bool loadSettingsFromJson(const QString& filePath, QFont& font, QColor& textColor, QColor& outlineColor, QKeySequence& hotkey) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return false;

    QJsonObject json = doc.object();
    font.setFamily(json["FontFamily"].toString("Arial"));
    font.setPointSize(json["FontSize"].toInt(24));
    textColor = QColor(json["TextColor"].toString("#ffffff"));
    outlineColor = QColor(json["OutlineColor"].toString("#000000"));
    hotkey = QKeySequence(json["Hotkey"].toString("Alt+X"));

    return true;
}


bool saveSettingsToJson(const QString& filePath, const QFont& font, const QColor& textColor, const QColor& outlineColor, const QKeySequence& hotkey) {
    QJsonObject json;
    json["FontFamily"] = font.family();
    json["FontSize"] = font.pointSize();
    json["TextColor"] = textColor.name();
    json["OutlineColor"] = outlineColor.name();
    json["Hotkey"] = hotkey.toString();

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(json).toJson(QJsonDocument::Indented));
        file.close();
        return true;
    }
    return false;
}
