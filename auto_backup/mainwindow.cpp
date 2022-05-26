#include "mainwindow.h"
#include "dragdropfilter.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <filesystem>
#include <iostream>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSet>
#include <QDesktopServices>
#include <QMimeData>
#include <QDragEnterEvent>


QString default_path = QDir::homePath()+ QDir::separator() + "auto-backup";
QString backups_path = default_path+QDir::separator()+"backups.txt";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{   

    ui->setupUi(this);

    if (!QDir(default_path).exists())
        QDir(default_path).mkpath(".");

    auto dragDropSource = new DragDropFilter;
    auto dragDropDest = new DragDropFilter;
    ui->list_source->installEventFilter(dragDropSource);
    ui->list_dest->installEventFilter(dragDropDest);
    ui->list_source->setAcceptDrops(true);
    ui->list_dest->setAcceptDrops(true);

    connect(dragDropSource, &DragDropFilter::dragDropped, this, [this](const QMimeData* mimeData){add_files_source(mimeData);});
    connect(dragDropDest, &DragDropFilter::dragDropped, this, [this](const QMimeData* mimeData){add_files_dest(mimeData);});

    connect(ui->Button_source_files, SIGNAL(released()), this, SLOT(Get_source_explorer()));
    connect(ui->Button_source_folder, SIGNAL(released()), this, SLOT(Get_source_explorer()));
    connect(ui->Button_dest, SIGNAL(released()), this, SLOT(Get_dest_explorer()));
    connect(ui->Button_start, SIGNAL(released()), this, SLOT(Button_start_backup_pressed()));
    connect(ui->Button_view_active, SIGNAL(released()), this, SLOT(Button_view_backups_pressed()));
    connect(ui->Button_clear, SIGNAL(released()), this, SLOT(Button_clear_pressed()));
    connect(ui->list_dest, SIGNAL(itemSelectionChanged()), this, SLOT(Dest_path_selected()));
    connect(ui->MenuDarkmode, SIGNAL(triggered()), this, SLOT(change_DarkMode()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_DarkMode(bool on){
    QFile styleSheetFile;
    if(on == true){
        styleSheetFile.setFileName("ThemeDarkMode.qss");
    }
    else {
        styleSheetFile.setFileName("ThemeClassic.qss");
    }
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    setStyleSheet(styleSheet);
}

void MainWindow::change_DarkMode(){
    if(ui->MenuDarkmode->isChecked()){
        qDebug() << "MenuDarkmode is now checked";
        set_DarkMode(true);
    }
    else{
        qDebug() << "MenuDarkmode is now unchecked";
        set_DarkMode(false);
    }
}

void MainWindow::add_files_source(const QMimeData* mimeData){
    foreach (const QUrl &url, mimeData->urls()) {
        QString fileName = url.toLocalFile();
        ui->list_source->addItem(fileName);
    }
}

void MainWindow::add_files_dest(const QMimeData* mimeData){
    foreach (const QUrl &url, mimeData->urls()) {
        QString fileName = url.toLocalFile();
        ui->list_dest->addItem(fileName);
    }
}

/*
void MainWindow::dragEnterEvent(QDragEnterEvent *event){
    qDebug() << "drag" << event;
    if (event->mimeData()->hasText()) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}
*/

/*
void MainWindow::dropEvent(QDropEvent *e){
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        ui->listWidget->addItem(fileName);
    }
}
*/
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
      if (!bytes.isNull()){
          QJsonParseError jsonError;
          document = QJsonDocument::fromJson(bytes, &jsonError);
          if (jsonError.error != QJsonParseError::NoError) {
            qDebug() << "Failed while parsing json: " << jsonError.errorString();
            throw;
          }
      }
    }
    return document;
}

bool search_string_in_json_array(QJsonArray &array, QString str){
    for (const auto json_str : array){
      if(json_str == str){
          return true;
      }
    }
    return false;
}

void MainWindow::Button_start_backup_pressed(){
    QString backups_path = default_path+QDir::separator()+"backups.txt";
    QFile file(backups_path);
    QJsonDocument json_doc = read_json(backups_path);
    QJsonObject json_obj = json_doc.object();

    if (ui->list_dest->count() && ui->list_source->count()) {
      for (int i = 0; i < ui->list_dest->count(); ++i) {
        QString path_dest = ui->list_dest->item(i)->text();
        QJsonArray source_array = json_obj.value(path_dest).toArray();
        for (int i = 0; i < ui->list_source->count(); ++i) {
          QString path_source = ui->list_source->item(i)->text();
          if(!search_string_in_json_array(source_array, path_source)){
            source_array.push_back(path_source);
          }
        }
        json_obj[path_dest] = source_array;
      }
      json_doc.setObject(json_obj);
      QByteArray bytes = json_doc.toJson(QJsonDocument::Indented);
      qInfo() << json_doc;

      if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << bytes << Qt::endl;
        file.close();
        ui->footer->setText("Backup registered!");
        ui->list_dest->clear();
        ui->list_source->clear();
      } else {
        ui->footer->setText("File open failed: " + backups_path);
      }
    } else {
      ui->footer->setText("Source and destination lists cannot be empty!");
    }
}

template <typename Container>
QString join_as_string(Container const& array, const QString separator = ", "){
    QString str = "";
    foreach(const auto& e, array) {
        str = str + separator + e.toString();
    }
    str.erase(str.begin(),str.begin()+separator.size()); // remove the first "separator"
    return str;
}

void MainWindow::Button_view_backups_pressed(){
    MainWindow::Button_clear_pressed();
    QFile file(backups_path);
    QJsonDocument json_doc = read_json(backups_path);
    if(!json_doc.isEmpty()){
        QJsonObject json_obj = json_doc.object();
        foreach(const QString& key, json_obj.keys()) {
            QJsonValue value = json_obj.value(key);
            qDebug() << "Destination: " << key << " => To backup: " << join_as_string(value.toArray());
            ui->list_dest->addItem(key);
        }
        ui->list_dest->setCurrentRow(0);
        //QDesktopServices::openUrl(QUrl::fromLocalFile(backups_path)); //open json file
        ui->footer->setText("Choose the destination to see the backups");
    }
    else{
        ui->footer->setText("No bakups registered!");
    }
}

void MainWindow::Dest_path_selected(){
    QString selected_item_dest = ui->list_dest->currentItem()->text();
    QFile file(backups_path);
    QJsonDocument json_doc = read_json(backups_path);
    QJsonObject json_obj = json_doc.object();
    QJsonArray json_array = json_obj[selected_item_dest].toArray();
    ui->list_source->clear();
    for (const auto json_str : json_array){
        ui->list_source->addItem(json_str.toString());
    }
    //ui->footer->setText(selected_item_dest);
}

void MainWindow::Button_clear_pressed(){
    ui->list_dest->clear();
    ui->list_source->clear();
    ui->footer->clear();
}
