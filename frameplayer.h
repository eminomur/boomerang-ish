#ifndef FRAMEPLAYER_H
#define FRAMEPLAYER_H

#include <QObject>

class FramePlayer : public QObject
{
    Q_OBJECT
public:
    explicit FramePlayer(QObject *parent = nullptr);

signals:

};

#endif // FRAMEPLAYER_H
