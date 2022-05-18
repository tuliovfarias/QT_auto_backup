#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->Button_source_files, SIGNAL(released()), this, SLOT(Get_source_explorer()));
    connect(ui->Button_source_folder, SIGNAL(released()), this, SLOT(Get_source_explorer()));
    connect(ui->Button_dest, SIGNAL(released()), this, SLOT(Get_dest_explorer()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::Get_source_explorer(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QString buttonText = button->text();
    if(button->objectName() == "Button_source_folder"){
        QString source_folder = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
        ui->list_source->addItem(source_folder);
    }
    else if(button->objectName() == "Button_source_files"){
        QStringList source_files = QFileDialog::getOpenFileNames(
                                this,
                                tr("Select files/folders to backup"),
                                "/home",
                                tr("Any(*.*)"));
        for (const auto& file : source_files){
            ui->list_source->addItem(file);
        }
    }
    else{
        ui->list_source->addItem(buttonText);
    }
}

void MainWindow::Get_dest_explorer(){
    //QString dest_folder = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), QDir::currentPath());
    QString dest_folder = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    ui->list_dest->addItem(dest_folder);
}

