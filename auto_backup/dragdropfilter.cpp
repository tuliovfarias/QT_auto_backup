#include <QDragEnterEvent>
#include "dragdropfilter.h"
#include "qwidget.h"

bool DragDropFilter::eventFilter(QObject* obj, QEvent* event)
{
    auto w = qobject_cast<QWidget*>(obj);
    if(w == nullptr) {
        return true;
    }

    QString highlightColor = w->palette().color(QPalette::Highlight).name(QColor::HexRgb);
    QString selectedSS = QString{"background-color:%1;"}.arg(highlightColor);

    switch (event->type()) {
        case QEvent::DragEnter:
        {
            w->setStyleSheet(selectedSS);
            auto dragEnterEvent = static_cast<QDragEnterEvent*>(event);
            dragEnterEvent->acceptProposedAction();
            emit dragEntered(dragEnterEvent->mimeData());
            break;
        }
        case QEvent::DragMove:
        {
            auto dragMoveEvent = static_cast<QDragMoveEvent*>(event);
            dragMoveEvent->acceptProposedAction();
            break;
        }
        case QEvent::DragLeave:
        {
            w->setStyleSheet("");
            auto dragLeaveEvent = static_cast<QDragLeaveEvent*>(event);
            dragLeaveEvent->accept();
            emit dragDeparted();
            break;
        }
        case QEvent::Drop:
        {
            w->setStyleSheet("");
            auto dropEvent = static_cast<QDropEvent*>(event);
            dropEvent->acceptProposedAction();
            emit dragDropped(dropEvent->mimeData());
            break;
        }
        default:
            return QObject::eventFilter(obj, event);
    }

    return true;
}
