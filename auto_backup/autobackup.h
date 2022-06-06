#ifndef AUTOBACKUP_H
#define AUTOBACKUP_H

#include <QString>
#include <QObject>
#include <QFile>
#include <QJsonDocument>


class AutoBackup : public QObject
{
    Q_OBJECT
private:
    QString backups_json_path;
public:    
    AutoBackup(QString backups_json_path);
    void start_backup();
    static void copy_file(const QString &source_path, const QString &dest_dir);
    static void copy_dir(QString src, QString dst);
    static QJsonDocument read_json(QFile file);
};

#endif // AUTOBACKUP_H
