#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qmimedata.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

//protected:
    //void dragEnterEvent(QDragEnterEvent *event);
    //void dropEvent(QDropEvent *e);

private slots:
    void Get_source_explorer();
    void Get_dest_explorer();
    void Button_start_backup_pressed();
    void Button_view_backups_pressed();
    void Dest_path_selected();
    void Button_clear_pressed();
    void add_files_source(const QMimeData* mimeData);
    void add_files_dest(const QMimeData* mimeData);
};
#endif // MAINWINDOW_H
