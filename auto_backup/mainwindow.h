#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qboxlayout.h"
#include "qmimedata.h"
#include "qpushbutton.h"
#include <QMainWindow>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void set_DarkMode(bool on);

private:
    Ui::MainWindow *ui;
    QPushButton *Button_remove_source;
    QPushButton *Button_remove_dest;
    QHBoxLayout *horizontalLayout_r;
    void config_remove_buttom();
    void config_icons();
    void showRemoveButton();
    void showRemoveButtonSource();
    void showRemoveButtonDest();
    bool open_confirmation_box(QString &path_dest, QList<QString> &sources_added, QList<QString> &sources_removed);
    void create_tray_icon();
    void changeEvent(QEvent* e);

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
    void change_DarkMode();
    void remove_from_source();
    void remove_from_dest();
    void on_show_hide(QSystemTrayIcon::ActivationReason reason);
};
#endif // MAINWINDOW_H
