#include "mainwindow.h"
#include "autobackup.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("./icons/add_folder.png"));

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "test_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }


    MainWindow w;

    w.show();
    w.set_DarkMode(true);
    return a.exec();
}
