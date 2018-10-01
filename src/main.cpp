#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString defaultPath = "";
    QString locale = "";

    QStringList args = a.arguments();
    if( args.count() >= 3 ) {
        QString path = args[1];
        if(path.endsWith("fdsi")) {
            defaultPath = path;
        }

        locale = args[2];
    }

    //　翻訳ファイルの適用
    QTranslator translator;
    QString langFileName;
    QString langFileDefault;
    QString appPath = QApplication::applicationDirPath();
    langFileName  = appPath + "/translations/" + QString(APP_TARGET) + "_" + locale;
    langFileDefault = langFileName + "en";

    if(translator.load(langFileName)) {
        a.installTranslator(&translator);
    } else {
        if(translator.load(langFileDefault))
        {a.installTranslator(&translator);}
    }

    MainWindow w;
    w.setArguments(defaultPath, locale);
    w.show();

    return a.exec();
}
