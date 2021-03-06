#include "rect3dnode.h"

Rect3DNode::Rect3DNode(QObject *parent) :
    QObject(parent)
{
}

Rect3DNode::~Rect3DNode()
{
    delete line1;
    delete line2;
    delete line3;
    delete line4;
}

void Rect3DNode::createRect(GeoCoords pos1, GeoCoords pos2)
{
    _pos1 = pos1;
    _pos2 = pos2;
    GeoCoords _point1 = _pos1;
    GeoCoords _point2 = {_pos2.lat, _pos1.lon};
    GeoCoords _point3 = _pos2;
    GeoCoords _point4 = {_pos1.lat, _pos2.lon};

    line1 = new Line3DNode(this);
    line1->createLine(_point1, _point2);
    line2 = new Line3DNode(this);
    line2->createLine(_point2, _point3);
    line3 = new Line3DNode(this);
    line3->createLine(_point3, _point4);
    line4 = new Line3DNode(this);
    line4->createLine(_point4, _point1);
}

void Rect3DNode::draw(QGLPainter *painter)
{
    line1->draw(painter);
    line2->draw(painter);
    line3->draw(painter);
    line4->draw(painter);
}

void Rect3DNode::changedPoint(GeoCoords pos2)
{
    _pos2 = pos2;
    GeoCoords _point1 = _pos1;
    GeoCoords _point2 = {_pos2.lat, _pos1.lon};
    GeoCoords _point3 = _pos2;
    GeoCoords _point4 = {_pos1.lat, _pos2.lon};

    line1->createLine(_point1, _point2);
    line2->createLine(_point2, _point3);
    line3->createLine(_point3, _point4);
    line4->createLine(_point4, _point1);
}
