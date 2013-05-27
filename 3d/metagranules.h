#ifndef METAGRANULES_H
#define METAGRANULES_H

#include <QObject>
#include <QGLSceneNode>
#include <QMap>

#include "more/structure.h"
#include "tilecacheclass.h"
#include "simplegranulesnode.h"

class EarthView;

class MetaGranules : public QGLSceneNode
{
    Q_OBJECT
public:
    MetaGranules(EarthView *parentView, QSharedPointer<QGLMaterialCollection> palette, ConfigData configData);
    void drawSimpleGranules(QGLPainter *painter);
    void drawTiledGranules(QGLPainter *painter);
    void setParent(EarthView *parentView){_parentView = parentView;}

    void addGranuleNode(qint32 granuleId, qint32 productId);
private:
    // int idGranule
    QCache<qint32, SimpleGranulesNode>* simpleGranuleCache;
    QCache<qint32, SimpleGranuleCacheNumber> simpleGranuleNumberCache;
    // <height, granuleId> - QMap items are always sorted by key.
    QMap<qint32, qint32> heightGranuleMap;

    QCache<TileCacheNumber, QGLSceneNode> tiledGranuleCache;
//    SimpleGranulesNode *granulesNode;

    EarthView *_parentView;
    QSharedPointer<QGLMaterialCollection> m_palette;
    ConfigData m_configData;
    qint32 currentHeight;

signals:
    void displayed();
    
public slots:
    void addSimpleGranuleNode(qint32 granuleId, qint32 productId);
    void removeSimpleGranuleNode(qint32 granuleId, qint32 productId);
    
};

#endif // METAGRANULES_H