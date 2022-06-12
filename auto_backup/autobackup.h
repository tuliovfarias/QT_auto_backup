#ifndef AUTOBACKUP_H
#define AUTOBACKUP_H

#include <QString>
#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QRunnable>


class AutoBackup : public QObject, public QRunnable
{
    Q_OBJECT
private:
    QString backups_json_path;
protected:
    void run() override;
public:    
    AutoBackup(QString backups_json_path);
    ~AutoBackup();
    //void start_backup();
    static void copy_file(const QString &source_path, const QString &dest_dir);
    static void copy_dir(QString src, QString dst);
    static QJsonDocument read_json(QFile file);
    static void chk_mtime_and_copy_file(QString source_path, QString dest_dir);
};

#endif // AUTOBACKUP_H
