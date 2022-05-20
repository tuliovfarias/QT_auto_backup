#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <filesystem>
#include <iostream>
#include <map>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>


QString default_path = QDir::homePath()+ QDir::separator() + "auto-backup";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{   

    ui->setupUi(this);
    if (!QDir(default_path).exists())
        QDir(default_path).mkpath(".");

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

QJsonDocument read_json(QFile file){
    QJsonDocument document;
    if (file.open(QIODevice::ReadOnly)) {
      QByteArray bytes = file.readAll();
      file.close();

      QJsonParseError jsonError;
      document = QJsonDocument::fromJson(bytes, &jsonError);
      if (jsonError.error != QJsonParseError::NoError) {
        // qInfo() << "fromJson failed: " <<
        // jsonError.errorString().toStdString() << Qt::endl;
        throw;
      }
    }
    return document;
}

void MainWindow::Button_start_backup_pressed(){
    QString backups_path = default_path+QDir::separator()+"backups.txt";
    QFile file(backups_path);
    QJsonDocument json_doc = read_json(backups_path);
    QJsonObject json_obj = json_doc.object();

    foreach(const QString& key, json_obj.keys()) {
        //qDebug() << json_obj.value(key);
        QJsonValue value = json_obj.value(key);
        for (const auto& valuew : value.toArray()){
            //qDebug() << valuew;
        }
        //qDebug() << "Key = " << key << ", Value = " << value.toString();
        qDebug() << "Key = " << key << ", Value = " << value.toArray();
    }
    //qInfo() << json_obj;

    if (ui->list_dest->count() && ui->list_source->count()) {
      QMap<QString, QList<QString>> map;
      for (int i = 0; i < ui->list_dest->count(); ++i) {
        QString path_dest = ui->list_dest->item(i)->text();
        for (int i = 0; i < ui->list_source->count(); ++i) {
          QString path_source = ui->list_source->item(i)->text();
          map[path_dest].push_back(path_source);
        }
      }
      // qInfo() << map;
      QVariantMap vmap;
      QMapIterator<QString, QList<QString>> i(map);
      while (i.hasNext()) {
        i.next();
        vmap.insert(i.key(), i.value());
      }
      // QVariantMap C = vmap.unite(vmap);
      QJsonDocument new_backup = QJsonDocument::fromVariant(vmap);
      QByteArray bytes = new_backup.toJson(QJsonDocument::Indented);
      qInfo() << new_backup;

      if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << bytes << Qt::endl;
        file.close();
        ui->footer->setText("Backup registered!");
      } else {
        ui->footer->setText("File open failed: " + backups_path);
      }

      /*foreach (auto value, map) {
        qInfo() << value;
      }*/
      /*
      for (auto iter = map.constBegin(); iter != map.constEnd(); ++iter) {
        QString values = iter.value().join(",");
        qInfo() << iter.key() << ":" << iter.value().join(",");
      }*/

    } else {
      ui->footer->setText("Source and destination lists cannot be empty!");
    }
}

void MainWindow::Button_view_backups_pressed(){
    QString backups_path = default_path+QDir::separator()+"backups.txt";
    QFile file(backups_path);
    QJsonDocument document = read_json(backups_path);
    qInfo() << document;
}
