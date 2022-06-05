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
#include <QMessageBox>

QString default_path = QDir::homePath()+ QDir::separator() + "auto-backup";
QString backups_path = default_path+QDir::separator()+"backups.json";
QString themes_path = "./themes";
QString icons_path = "./icons";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    if (!QDir(default_path).exists())
        QDir(default_path).mkpath(".");

    config_remove_buttom();
    config_icons();
    Button_view_backups_pressed();
    Dest_path_selected();

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

    connect(Button_remove_source, SIGNAL(released()), this, SLOT(remove_from_source()));
    connect(Button_remove_dest, SIGNAL(released()), this, SLOT(remove_from_dest()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::config_remove_buttom(){
    Button_remove_source = new QPushButton(this);
    //Button_remove_source = new QPushButton("Remove", this);
    Button_remove_dest = new QPushButton(this);
    Button_remove_source->setVisible(false);
    Button_remove_dest->setVisible(false);
    //Button_remove_dest = new QPushButton("Remove", this);
    horizontalLayout_r = new QHBoxLayout();
    horizontalLayout_r->setObjectName(QString::fromUtf8("horizontalLayout_r"));
    horizontalLayout_r->setSizeConstraint(QLayout::SetFixedSize);
    horizontalLayout_r->addWidget(Button_remove_source);
    horizontalLayout_r->addWidget(Button_remove_dest);
    ui->verticalLayout->insertLayout(3,horizontalLayout_r);
}

void MainWindow::config_icons(){
    QPixmap AddFile(icons_path + QDir::separator() + "add_file");
    QIcon IconAddFile(AddFile);

    QPixmap AddFolder(icons_path + QDir::separator() + "add_folder");
    QIcon IconAddFolder(AddFolder);

    QPixmap Remove(icons_path + QDir::separator() + "remove");
    QIcon IconRemove(Remove);

    ui->Button_source_folder->setIcon(IconAddFolder);
    ui->Button_source_files->setIcon(IconAddFile);
    ui->Button_dest->setIcon(IconAddFolder);

    ui->Button_source_folder->setIconSize(QSize(30, 30));
    ui->Button_source_files->setIconSize(QSize(30, 30));
    ui->Button_dest->setIconSize(QSize(30, 30));

    Button_remove_source->setIcon(Remove);
    Button_remove_source->setIconSize(QSize(20, 20));
    Button_remove_dest->setIcon(Remove);
    Button_remove_dest->setIconSize(QSize(20, 20));
}

void MainWindow::set_DarkMode(bool on){
    QFile styleSheetFile;
    if(on == true){
        styleSheetFile.setFileName(themes_path + QDir::separator() + "ThemeDarkMode.qss");
    }
    else {
        styleSheetFile.setFileName(themes_path + QDir::separator() + "ThemeClassic.qss");
    }
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    setStyleSheet(styleSheet);
}

void MainWindow::change_DarkMode(){
    if(ui->MenuDarkmode->isChecked()){
        //qDebug() << "MenuDarkmode is now checked";
        set_DarkMode(true);
    }
    else{
        //qDebug() << "MenuDarkmode is now unchecked";
        set_DarkMode(false);
    }
}

void MainWindow::add_files_source(const QMimeData* mimeData){
    if(!mimeData->urls().empty()){
        foreach (const QUrl &url, mimeData->urls()) {
            QString fileName = url.toLocalFile();
            ui->list_source->addItem(fileName);
        }
        showRemoveButtonSource();
    }
}

void MainWindow::add_files_dest(const QMimeData* mimeData){
    if(!mimeData->urls().empty()){
        foreach (const QUrl &url, mimeData->urls()) {
            QString fileName = url.toLocalFile();
            ui->list_dest->addItem(fileName);
        }
        showRemoveButtonDest();
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
        showRemoveButtonSource();
      }
    }
    else if(button->objectName() == "Button_source_files"){
      QStringList source_files = QFileDialog::getOpenFileNames(
          this, tr("Select files/folders to backup"), "/home", tr("Any(*.*)"));
      if(!source_files.isEmpty()){
          for (const auto &file : source_files) {
            ui->list_source->addItem(file);
            showRemoveButtonSource();
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
        showRemoveButtonDest();
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
    QFile file(backups_path);
    if(!file.exists()){
        file.open(QIODevice::WriteOnly);
        file.close();
    }
    QJsonDocument json_doc = read_json(backups_path);
    QJsonObject json_obj = json_doc.object();
    QByteArray bytes = json_doc.toJson(QJsonDocument::Indented);

    QList<QString> sources_added;
    QJsonArray new_source_array;
    bool remove_path_dest;

    int items_dest = ui->list_dest->count();
    if (items_dest) {
        if(items_dest == 1){ui->list_dest->setCurrentRow(0);} // select the unique destination path
        QListWidgetItem *path_dest = ui->list_dest->currentItem();
        QString path_dest_str = path_dest->text();
        QJsonArray source_array = json_obj.value(path_dest_str).toArray();
        for (int i = 0; i < ui->list_source->count(); ++i) {
          QString path_source = ui->list_source->item(i)->text();
          new_source_array.push_back(path_source);
          if(!search_string_in_json_array(source_array, path_source)){
            sources_added.append(path_source);
          }
        }
        if(new_source_array.isEmpty()){
            json_obj.remove(path_dest_str);
        } else{
            json_obj[path_dest_str] = new_source_array;
        }
        json_doc.setObject(json_obj);
        bytes = json_doc.toJson(QJsonDocument::Indented);

        bool confirmation = open_confirmation_box(path_dest_str, sources_added);
        if(confirmation){
            if (file.open(QIODevice::ReadWrite)) {
              file.resize(0);
              QTextStream stream(&file);
              stream << bytes << Qt::endl;
              file.close();
              if(!ui->list_source->count()){
                  ui->list_dest->takeItem(ui->list_dest->currentRow());
              }
              ui->footer->setText("Backup registered!");
              //ui->list_dest->clear();
              //ui->list_source->clear();
            } else {
              ui->footer->setText("File open failed: " + backups_path);
            }
        }

    } else {
        ui->footer->setText("Destination lists cannot be empty!");
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

void MainWindow::showRemoveButton(){
     showRemoveButtonSource();
     showRemoveButtonDest();
}

void MainWindow::showRemoveButtonSource(){
    horizontalLayout_r->insertWidget(1, Button_remove_source);
    Button_remove_source->setVisible(true);
}

void MainWindow::showRemoveButtonDest(){
    horizontalLayout_r->insertWidget(1, Button_remove_dest);
    Button_remove_dest->setVisible(true);
}

void MainWindow::Button_view_backups_pressed(){
    MainWindow::Button_clear_pressed();
    QFile file(backups_path);
    QJsonDocument json_doc = read_json(backups_path);
    QJsonObject json_obj = json_doc.object();
    if(!json_obj.isEmpty()){
        foreach(const QString& key, json_obj.keys()) {
            QJsonValue values = json_obj.value(key);
            qDebug() << "Destination: " << key << " => To backup: " << join_as_string(values.toArray());
            ui->list_dest->addItem(key);
            for (int i = 0; i < values.toArray().count(); ++i){
                ui->list_source->addItem(values[i].toString());
            }
        }
        ui->list_dest->setCurrentRow(0);
        //QDesktopServices::openUrl(QUrl::fromLocalFile(backups_path)); //open json file
        ui->footer->setText("Choose the destination to see the files/folders to backup");
        showRemoveButton();
    }
    else{
        file.resize(0); // clear file to avoid crash in case "{}"
        ui->footer->setText("No bakups registered!");
    }
}

void MainWindow::Dest_path_selected(){
    QFile file(backups_path);
    QJsonDocument json_doc = read_json(backups_path);
    QJsonObject json_obj = json_doc.object();
    if(!json_obj.isEmpty()){
        if(ui->list_dest->count() != 1){
            QString selected_item_dest = ui->list_dest->currentItem()->text();
            QJsonArray json_array = json_obj[selected_item_dest].toArray();
            ui->list_source->clear();
            for (const auto json_str : json_array){
                ui->list_source->addItem(json_str.toString());
            }
            Button_remove_source->setVisible(true);
            //showRemoveButtonSource();

        }
        //ui->footer->setText(selected_item_dest);
    }
}

bool MainWindow::open_confirmation_box(QString &path_dest, QList<QString> &new_sources_list){
  QMessageBox::StandardButton reply;
  QString message;
  if(new_sources_list.isEmpty()){
      message = "Apply changes?";
  } else{
      message = "Add to backup:\n" + new_sources_list.join("\n") + "\n\nDestination:\n" + path_dest + "?";
  }
  reply = QMessageBox::question(this, "Confirmation", message, QMessageBox::Yes|QMessageBox::No);
  if (reply == QMessageBox::Yes) {
    return true;
  } else {
    return false;
  }
}

void MainWindow::Button_clear_pressed(){
    ui->list_dest->clear();
    ui->list_source->clear();
    ui->footer->clear();
    Button_remove_source->setVisible(false);
    Button_remove_dest->setVisible(false);
}

void MainWindow::remove_from_source(){
    if (ui->list_source->currentItem() != 0 & ui->list_source->count() != 0){
        int current_row = ui->list_source->currentRow();
        QListWidgetItem *selected_item_source = ui->list_source->takeItem(current_row);
        ui->footer->setText(selected_item_source->text()+" removed");
        if(current_row != 0){
            ui->list_source->setCurrentRow(current_row-1);
        }
        if (ui->list_source->count() == 0){
            Button_remove_source->setVisible(false);
        }
    }
}

void MainWindow::remove_from_dest(){
    if(ui->list_dest->count() != 0){
        QListWidgetItem *selected_item_dest = ui->list_dest->takeItem(ui->list_dest->currentRow());
        ui->footer->setText(selected_item_dest->text()+" removed");
        ui->list_dest->setCurrentRow(0);
        if (ui->list_dest->count() == 0){
            Button_remove_dest->setVisible(false);
        }
    }
}
