#ifndef RECT3DNODE_H
#define RECT3DNODE_H

#include <QObject>
#include <QColor>
#include <QVector3D>
#include <QGLPainter>
#include "line3dnode.h"

class Rect3DNode : public QObject
{
    Q_OBJECT
public:
    explicit Rect3DNode(QObject *parent = 0);
    void createRect(GeoCoords pos1, GeoCoords pos2);
    void draw(QGLPainter *painter);

private:
    QColor _color;
    GeoCoords _pos1;
    GeoCoords _pos2;
    Line3DNode* line1;
    Line3DNode* line2;
    Line3DNode* line3;
    Line3DNode* line4;
signals:
    
public slots:
    
};

#endif // RECT3DNODE_H
