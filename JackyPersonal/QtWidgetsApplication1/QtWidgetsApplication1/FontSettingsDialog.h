#pragma once

#include <QDialog>
#include <QFontComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QKeySequenceEdit>
#include <QDialogButtonBox>
#include <QSettings>
#include <QComboBox>  

class FontSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FontSettingsDialog(QWidget* parent = nullptr);

    QFont selectedFont() const;
    QColor textColor() const;
    QColor outlineColor() const;
    QKeySequence selectedHotkey() const;
    QString selectedLanguage() const; 

private slots:
    void chooseTextColor();
    void chooseOutlineColor();

private:
    QFontComboBox* m_fontBox;
    QSpinBox* m_sizeBox;
    QPushButton* m_textColorBtn;
    QPushButton* m_outlineColorBtn;
    QKeySequenceEdit* m_hotkeyEdit;
    QComboBox* m_languageBox; 

    QColor m_textColor;
    QColor m_outlineColor;
    QKeySequence m_originalHotkey;
    bool m_isInitializing = false;
};

// Utility
bool saveSettingsToJson(const QString& filePath, const QFont& font, const QColor& textColor, const QColor& outlineColor, const QKeySequence& hotkey, const QString& language);
bool loadSettingsFromJson(const QString& filePath, QFont& font, QColor& textColor, QColor& outlineColor, QKeySequence& hotkey, QString& language);
