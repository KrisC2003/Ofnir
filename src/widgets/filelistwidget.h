#pragma once

#include <QListWidget>
#include <QDragEnterEvent>
#include <QDropEvent>

class FileListWidget : public QListWidget {
    Q_OBJECT

public:
    explicit FileListWidget(QWidget* parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};
