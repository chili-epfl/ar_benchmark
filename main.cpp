#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "tester.h"
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    qmlRegisterType<Tester>("Tester", 1, 0, "Tester");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
