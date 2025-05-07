#include "filelistwidget.h"
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>

FileListWidget::FileListWidget(QWidget* parent) : QListWidget(parent) {
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::DropOnly);
}

void FileListWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void FileListWidget::dropEvent(QDropEvent* event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    for (const QUrl& url : urls) {
        QString localPath = url.toLocalFile();
        QFileInfo info(localPath);
        if (info.exists() && info.isFile()) {
            addItem(info.fileName()); 
        }
    }
    event->acceptProposedAction();
}
