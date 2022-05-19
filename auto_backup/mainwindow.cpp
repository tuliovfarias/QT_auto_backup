#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <filesystem>
#include <iostream>
#include <map>
#include <QDebug>

QString default_path = QDir::homePath()+"/auto-backup";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{   

    ui->setupUi(this);


    connect(ui->Button_source_files, SIGNAL(released()), this, SLOT(Get_source_explorer()));
    connect(ui->Button_source_folder, SIGNAL(released()), this, SLOT(Get_source_explorer()));
    connect(ui->Button_dest, SIGNAL(released()), this, SLOT(Get_dest_explorer()));
    connect(ui->Button_start, SIGNAL(released()), this, SLOT(Button_start_backup_pressed()));
    connect(ui->Button_view_active, SIGNAL(released()), this, SLOT(Button_view_backups_pressed()));        
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Get_source_explorer(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString buttonText = button->text();
    if(button->objectName() == "Button_source_folder"){
      QString source_folder = QFileDialog::getExistingDirectory(
          this, tr("Open Directory"), "/home",
          QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
      if(!source_folder.isEmpty()){
        ui->list_source->addItem(source_folder);
      }
    }
    else if(button->objectName() == "Button_source_files"){
      QStringList source_files = QFileDialog::getOpenFileNames(
          this, tr("Select files/folders to backup"), "/home", tr("Any(*.*)"));
      if(!source_files.isEmpty()){
          for (const auto &file : source_files) {
            ui->list_source->addItem(file);
          }
      }
    }
    else{
        ui->list_source->addItem(buttonText);
    }
}

void MainWindow::Get_dest_explorer(){
    QString dest_folder = QFileDialog::getExistingDirectory(
        this, tr("Open Directory"), "/home",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(!dest_folder.isEmpty()){
        ui->list_dest->addItem(dest_folder);
    }
}

void MainWindow::Button_start_backup_pressed(){
    if(ui->list_dest->count() && ui->list_source->count() ){
        QMap<QString, QList<QString>> map;
        for(int i = 0; i < ui->list_dest->count(); ++i){
            QString path_dest = ui->list_dest->item(i)->text();
            for(int i = 0; i < ui->list_source->count(); ++i){
                QString path_source = ui->list_source->item(i)->text();
                map[path_dest].push_back(path_source);
            }
        }
        qInfo() << map;
        /*foreach (auto value, map) {
          qInfo() << value;
        }*/
        for (auto iter = map.constBegin(); iter != map.constEnd(); ++iter) {
            QString values = iter.value().join(",");
            qInfo() << iter.key() << ":" << iter.value().join(",");
        }
        ui->footer->setText("Backup registered!");
    }
    else{
        ui->footer->setText("Source and destination lists cannot be empty!");
    }

}

void MainWindow::Button_view_backups_pressed(){

}
