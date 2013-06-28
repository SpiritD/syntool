#ifndef POINT3DNODE_H
#define POINT3DNODE_H

#include <QObject>
#include <QColor>
#include <QGLSceneNode>
#include <QGraphicsRotation3D>
#include <QVector3D>
#include <QGLPainter>

#include "more/structure.h"
#include "more/geofunctions.h"

class Point3DNode : public QObject
{
    Q_OBJECT
public:
    explicit Point3DNode(QObject *parent = 0);
    void createPoint(GeoCoords pos);
    void draw(QGLPainter *painter);

private:
    QColor _color;
    GeoCoords _pos;
    QVector3DArray verts;
signals:
    
public slots:
    
};

#endif // POINT3DNODE_H
