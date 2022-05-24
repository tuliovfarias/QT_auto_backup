#include <QObject>
#include <QMimeData>

class DragDropFilter : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
signals:
    void dragEntered(const QMimeData* mimeData);
    void dragDropped(const QMimeData* mimeData);
    void dragDeparted();
};
