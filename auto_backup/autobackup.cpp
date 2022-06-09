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

AutoBackup::AutoBackup(QString backups_json_path)
    : backups_json_path(backups_json_path) {};

void AutoBackup::copy_file(const QString &source_path, const QString &dest_dir) {
    QFileInfo fi(source_path);
    if(QFile::copy(source_path, dest_dir + QDir::separator() + fi.fileName())){
        qDebug() << source_path << " copied to " << dest_dir;
    } else{
        qDebug() << "ERROR while coping" << source_path << " to " << dest_dir;
    }
}

void AutoBackup::copy_dir(QString src, QString dst) {
    QDir dir(src);
    if (! dir.exists())
        return;
    foreach (QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString dst_path = dst + QDir::separator() + dir.dirName() + QDir::separator() + d;
        dir.mkpath(dst_path);
        copy_dir(src+ QDir::separator() + d, dst_path);
    }
    foreach (QString f, dir.entryList(QDir::Files)) {
        QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
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

void AutoBackup::start_backup() {
    QJsonDocument json_doc = read_json(backups_json_path);
    QJsonObject json_obj = json_doc.object();
    foreach(const auto& dest_dir, json_obj.keys()) {
        QJsonArray values = json_obj.value(dest_dir).toArray();
        foreach(const auto& value, values){
            QString source_path = value.toString();
            QFileInfo file_source(source_path);
            if(file_source.exists()){
                if(file_source.isFile()){
                    QFileInfo file_dest(dest_dir+QDir::separator()+file_source.fileName());
                    if (file_dest.exists())
                    {
                        if (file_dest.lastModified() < file_source.lastModified()){
                            qDebug() << "Updated: " << source_path << " -> " << dest_dir;
                            QFile::remove(dest_dir);
                            copy_file(source_path, dest_dir);
                        } else{
                            qDebug() << source_path << " = " << dest_dir;
                            continue;
                        }
                    } else{
                        if(QFile::exists(dest_dir)){
                            copy_file(source_path, dest_dir);
                        } else{
                            qDebug() << "Destination does not exists: " << dest_dir;
                        }
                    }
                } else{
                    qDebug() << "Created: " << source_path << " -> " << dest_dir;
                    copy_dir(source_path, dest_dir);
                    /*
                    //QDirIterator it(source_path, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);
                    QDirIterator it(source_path, QDir::Files, QDirIterator::Subdirectories);
                    while (it.hasNext())
                        qDebug() << it.next();*/
                }
            } else{
                qDebug() << "Source does not exists: " << source_path;
            }

        }
    }
}

/*
int main(int argc, char *argv[]){
    QString default_path = QDir::homePath()+ QDir::separator() + "auto-backup";
    QString backups_json_path = default_path+QDir::separator()+"backups.json";
    auto auto_bkp = new AutoBackup("");
    auto_bkp->start_backup();
    return 0;
}*/
