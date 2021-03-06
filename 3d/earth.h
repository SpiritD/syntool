#ifndef EARTH_H
#define EARTH_H

#include "qglbuilder.h"
#include "qgraphicsrotation3d.h"
#include <QGLSphere>
#include <QGLPainter>

#include "qgltexture2d.h"
#include <QImage>
#include <QTimer>
#include <QObject>
#include <QDir>
#include <typeinfo>
#include <QSettings>

#include "tiledownloader.h"
#include "more/structure.h"
#include "more/geofunctions.h"
#include "glclasses/glnodecache.h"
#include "tilecachenumbersclass.h"
#include "glscenenodewrapper.h"

QT_BEGIN_NAMESPACE
class QGLTexture2D;
QT_END_NAMESPACE

class Earth : public QGLSceneNode
{
    Q_OBJECT
public:
    Earth(QObject *parent, QSharedPointer<QGLMaterialCollection> materials, ConfigData *configData);
    ~Earth();

//    void drawImage(QGLPainter *painter);
    QGLSceneNode *sphere;

    void tileDownload(qint32 cur_zoom, qint32 separation, qint32 lonTileNum, qint32 latTileNum);

private:
    QCache<QString, TileDownloader>* downloadQueue;
    QCache<TileCacheNumber, GLSceneNodeWrapper>* tileNodeCache;

    int zoom;
    int zoom_old;

    qreal     curScale;
    qreal     curZoom;
    GeoCoords curGeoCoords;
    bool      newZoomFlag;

//    QHash<TileCacheNumber, QGLTexture2D*> m_LoadedTextures;
    QGLTexture2D*        m_texture;
    QString              cacheDir;
    ConfigData*          m_configData;

    void          buildEarthNode(qreal radius = 1.0, int divisions = 5, int separation = 1);
    QGLSceneNode* BuildSpherePart(int separation, qreal minSphereLat, qreal maxSphereLat,
                                  qreal minSphereLon, qreal maxSphereLon);
    bool          addTextureToTile(QGLSceneNode *tempNode, int separation, int lonTileNum, int latTileNum, int cur_zoom);
    bool          checkTextureFile(int separation, int lonTileNum, int latTileNum, int cur_zoom);

    bool          checkNodeInCache(int zoom, int x, int y);
    void          cleanupResources();
    void          clearTileCache();

    // <mapThemeName, tile url>
    QMap<QString, QString> mapThemeList;
    QString                currentMapTheme;
    QString                currentMapThemeUrl;
    QString                tileExtension;

signals:
    void textureDownloadedSignal(qint32 cur_zoom, qint32 lonTileNum, qint32 latTileNum);
    void updatedTilesRangeSignal(qint32 curZoom, TileRange tileRange1, TileRange tileRange2);
    void updatedAllTilesSignal(qint32 curZoom);

public slots:
    void updateTilesSlot(qreal scale, GeoCoords geoCoords);
    void addTileNode(int cur_zoom, qint32 lonTileNum, qint32 latTileNum);
    void textureDownloaded(qint32 cur_zoom, qint32 lonTileNum, qint32 latTileNum);
    void setMapTheme(QString mapThemeName);

};

#endif // EARTH_H
