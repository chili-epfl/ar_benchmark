#ifndef TESTER_H
#define TESTER_H



#include <QObject>

class Tester : public QObject
{
    Q_OBJECT

public:
    explicit Tester(QObject *parent = 0);
    Q_INVOKABLE void run(QString img_path, QString type=QString("rotate"), qreal param=0.0);
signals:
    void finished();
public slots:

private:


};

#endif // TESTER_H
