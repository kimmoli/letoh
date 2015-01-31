#ifndef LETOH_H
#define LETOH_H

#include <QObject>

class Letoh : public QObject
{
    Q_OBJECT
public:
    explicit Letoh(QObject *parent = 0);

signals:

public slots:
    void handleNotify(QString notification);

};

#endif // LETOH_H
