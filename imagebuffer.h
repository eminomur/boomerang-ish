#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QObject>

class ImageBuffer : public QObject
{
    Q_OBJECT
public:
    explicit ImageBuffer(QObject *parent = nullptr);

signals:

};

#endif // IMAGEBUFFER_H
