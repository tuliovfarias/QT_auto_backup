#include "autobackup.h"
#include <filesystem>
#include <iostream>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "mainwindow.h"
#include <QFile>

#include <QThreadPool>

//QThreadPool *auto_bkp_pool = new QThreadPool();

AutoBackup::AutoBackup(QString backups_json_path)
    : backups_json_path(backups_json_path)
{
    //auto_bkp_pool->setMaxThreadCount(1);
};

AutoBackup::~AutoBackup(){
    qDebug() << "AutoBackup destructor";
}

void AutoBackup::run() {
    QJsonDocument json_doc = read_json(backups_json_path);
    QJsonObject json_obj = json_doc.object();
    foreach(const auto& dest_dir, json_obj.keys()) {
        QJsonArray values = json_obj.value(dest_dir).toArray();
        foreach(const auto& value, values){
            QString source_path = value.toString();
            QFileInfo file_source(source_path);
            if(file_source.exists()){
                if(file_source.isFile()){
                    chk_mtime_and_copy_file(source_path, dest_dir);
                } else{
                    QDir folder(source_path);
                    copy_dir(source_path, dest_dir + QDir::separator() + folder.dirName());
                }
            } else{
                qDebug() << "Source does not exists: " << source_path;
            }
        }
    }
}

void AutoBackup::copy_file(const QString &source_path, const QString &dest_dir) {
    QFileInfo fi(source_path);
    if(QFile::copy(source_path, dest_dir + QDir::separator() + fi.fileName())){
        qDebug() << source_path << " copied to " << dest_dir;
    } else{
        qDebug() << "ERROR while coping" << source_path << " to " << dest_dir;
    }
}

void AutoBackup::copy_dir(QString src, QString dst){
    QDir dir(src);
    QString dst_path;
    if (! dir.exists())
        return;
    foreach (QString folder, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        dst_path = dst + QDir::separator() + folder;
        if (!QDir(dst_path).exists()) {
          dir.mkpath(dst_path);
          qDebug() << "Dir created: " << dst_path;
        }
        /*auto lambda = [src, folder, dst_path](){
            copy_dir(src+ QDir::separator() + folder, dst_path);
        };
        qDebug() << folder << "auto_bkp_pool:" << auto_bkp_pool->activeThreadCount() << auto_bkp_pool->maxThreadCount();
        auto_bkp_pool->start(lambda);*/
        copy_dir(src+ QDir::separator() + folder, dst_path);
    }
    foreach (QString file, dir.entryList(QDir::Files)) {
         AutoBackup::chk_mtime_and_copy_file(src + QDir::separator() + file, dst);
    }
}

QJsonDocument AutoBackup::read_json(QFile file){
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

void AutoBackup::chk_mtime_and_copy_file(QString source_path, QString dest_dir){
    QFileInfo file_source(source_path);
    QFileInfo file_dest(dest_dir+QDir::separator()+file_source.fileName());
    if (file_dest.exists())
    {
        if (file_dest.lastModified() < file_source.lastModified()){
            qDebug() << "Updated: " << source_path << " -> " << dest_dir;
            QFile::remove(dest_dir);
            copy_file(source_path, dest_dir);
        } else{
            //qDebug() << source_path << " = " << dest_dir;
            return;
        }
    } else{
        if(QDir(dest_dir).exists()){
            copy_file(source_path, dest_dir);
        } else{
            qDebug() << "Destination dir does not exists: " << dest_dir;
        }
    }
}
