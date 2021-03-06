#include "earth.h"

const double a = 6378137.0;

Earth::Earth(QObject *parent, QSharedPointer<QGLMaterialCollection> materials, ConfigData *configData)
    : QGLSceneNode(parent)
{
    setPalette(materials);
    m_configData = configData;
    cacheDir = configData->cacheDir;

    // create Theme variables
    mapThemeList.insert("OSM", "http://tile.openstreetmap.org/%1/%2/%3.png");
    mapThemeList.insert("transportOSM", "http://tile2.opencyclemap.org/transport/%1/%2/%3.png");
    mapThemeList.insert("yandexSatellite", "http://sat.maps.yandex.net/tiles?l=sat&v=3.102.0&z=%1&x=%2&y=%3&lang=ru_RU");
    mapThemeList.insert("yandexMaps", "http://vec.maps.yandex.net/tiles?l=map&v=2.45.0&z=%1&x=%2&y=%3&lang=ru_RU");
    mapThemeList.insert("googleMaps", "http://mts.google.com/vt/lyrs=m&hl=en&z=%1&x=%2&y=%3&s=Galile");
    mapThemeList.insert("googleSatellite", "http://khms.google.ru/kh/v=135&src=app&z=%1&x=%2&y=%3&s=Gal");

    currentMapTheme = configData->mapThemeName;
    currentMapThemeUrl = mapThemeList.value(currentMapTheme);
    tileExtension = (currentMapTheme.indexOf("Satellite") >= 0) ? "jpg": "png";

    // set the maximum number of threads to download images tiles
    downloadQueue = new QCache<QString, TileDownloader>;
    downloadQueue->setMaxCost(25);

    tileNodeCache = new QCache<TileCacheNumber, GLSceneNodeWrapper>;
    // set maximum cost for cache
    tileNodeCache->setMaxCost(configData->numberCachedTiles);
//    m_LoadedTextures.setMaxCost(50);

    buildEarthNode(a, 10, 0);

    QGraphicsRotation3D *rotateX = new QGraphicsRotation3D(this);
    rotateX->setAngle(180.0f);
    rotateX->setAxis(QVector3D(1,0,0));

    QGraphicsRotation3D *rotateY = new QGraphicsRotation3D(this);
    rotateY->setAngle(90.0f);
    rotateY->setAxis(QVector3D(0,1,0));

    addTransform(rotateX);
    addTransform(rotateY);

//    addNode(sphere);

    zoom = -1;
    zoom_old = 0;

    connect(this, &Earth::textureDownloadedSignal, this, &Earth::textureDownloaded);
}

/*!
 * Check availability QGLSceneNode* in tileNodeCache
 * and the addition of node if contains
 */
bool Earth::checkNodeInCache(int zoom, int x, int y)
{
    TileCacheNumber currentCacheNumber = TileCacheNumber(zoom, x, y);
    if (tileNodeCache->contains(currentCacheNumber))
    {
        QGLSceneNode* sceneNode = tileNodeCache->object(currentCacheNumber)->glSceneNodeObject();
        if (sceneNode->options().testFlag(QGLSceneNode::HideNode))
        {
            sceneNode->setOptions(QGLSceneNode::CullBoundingBox);
            addNode(sceneNode);
        }

        emit displayed();
        return true;
    }
    return false;
}

/*!
    Add all tiles node for current zoom (considering bbox).
*/
void Earth::buildEarthNode(qreal radius, int divisions, int cur_zoom)
{
    Q_UNUSED(radius);
    Q_UNUSED(divisions);
    qreal separation = qPow(2, cur_zoom);

    if (cur_zoom > 2){
        int numberTiles = qCeil(getNumberTiles(cur_zoom, 2*40000/curScale));

        TileNumber tileNumber = deg2TileNum(curGeoCoords, cur_zoom);

        TileRange* tileRange = getTileRange(tileNumber, numberTiles, cur_zoom);
        emit updatedTilesRangeSignal(cur_zoom, tileRange[0], tileRange[1]);

//        qCritical() << "=========>";
//        qCritical() << tileRange[0].startX << tileRange[0].endX << tileRange[0].startY << tileRange[0].endY << tileRange[0].end;
//        qCritical() << tileRange[1].startX << tileRange[1].endX << tileRange[1].startY << tileRange[1].endY;

        for (int tileRangeNumber = 0; tileRangeNumber <= 1; tileRangeNumber++)
        {
            for (int lonTileNum = tileRange[tileRangeNumber].startX; lonTileNum <= tileRange[tileRangeNumber].endX; lonTileNum++)
            {
                for (int latTileNum = tileRange[tileRangeNumber].startY; latTileNum <= tileRange[tileRangeNumber].endY; latTileNum++)
                {
                    if (!checkNodeInCache(cur_zoom, lonTileNum, latTileNum))
                    {
                        addTileNode(cur_zoom, lonTileNum, separation-1-latTileNum);
                    }
                }
            }
            if (tileRange[tileRangeNumber].end)
                break;
        }
        delete tileRange;
    }
    else
    {
        emit updatedAllTilesSignal(cur_zoom);
        for (int lonTileNum = 0; lonTileNum < separation; lonTileNum++)
        {
            for (int latTileNum = 0; latTileNum < separation; latTileNum++)
            {
                if (!checkNodeInCache(cur_zoom, lonTileNum, separation-1-latTileNum))
                {
                    addTileNode(cur_zoom, lonTileNum, latTileNum);
                }
            }
        }
    }
}

/*!
    Wrapper to add a one tile to the sphere.
    If texture file not exist then run new thread (from method tileDownload).
*/
void Earth::addTileNode(int cur_zoom, qint32 lonTileNum, qint32 latTileNum)
{
    int separation = qPow(2, cur_zoom);

    if (!checkTextureFile(separation, lonTileNum, latTileNum, cur_zoom))
    {
        tileDownload(cur_zoom, separation, lonTileNum, latTileNum);
        return;
    }
//    emit textureDownloadedSignal(cur_zoom, lonTileNum, latTileNum);
    textureDownloaded(cur_zoom, lonTileNum, latTileNum);
}

/*!
    starts then texture exists or after download texture.
    This method calling all methods for create tile, overlay texture and
         add QGLSceneNode to scene (BuildSpherePart, addTextureToTile, addNode)
*/
void Earth::textureDownloaded(qint32 cur_zoom, qint32 lonTileNum, qint32 latTileNum)
{
    if(cur_zoom != zoom)
        return;

    int separation = qPow(2, cur_zoom);

    qreal NTLon = 2*M_PI / separation;

    qreal minSphereLat = -tiley2lat(latTileNum, separation)/180.0*M_PI;
    qreal maxSphereLat = -tiley2lat(latTileNum+1, separation)/180.0*M_PI;

    qreal minSphereLon = ((lonTileNum) * NTLon - M_PI);
    qreal maxSphereLon = ((lonTileNum+1) * NTLon - M_PI);

    QGLSceneNode* sceneNode = BuildSpherePart(separation, minSphereLat, maxSphereLat,
                                             minSphereLon, maxSphereLon);

    QString nodeObjectName = QString("tile-%1-%2-%3").arg(cur_zoom).arg(lonTileNum)
                                                     .arg(separation-1-latTileNum);

    if (addTextureToTile(sceneNode, separation, lonTileNum, latTileNum, cur_zoom))
    {
        if(cur_zoom != zoom)
        {
            delete sceneNode;
            return;
        }
        sceneNode->setOptions(QGLSceneNode::CullBoundingBox);

        // add SceneNode to cache
        TileCacheNumber tileNumber = TileCacheNumber(cur_zoom, lonTileNum,separation-1-latTileNum);

        GLSceneNodeWrapper* sceneNodeWrapper = new GLSceneNodeWrapper;
        sceneNodeWrapper->setGLSceneNodeObject(sceneNode, this);

        tileNodeCache->insert(tileNumber, sceneNodeWrapper);
        sceneNode->setObjectName(nodeObjectName);

        addNode(sceneNode);
        qDebug() << tr("Add GLSceneNone object") << nodeObjectName;
    }
    else
    {
        delete sceneNode;
        qWarning() << tr("Texture not added");
    }
    emit displayed();
}

/*!
    Create QGLSceneNode for one tile.
    stacks - number pieces of tatitude for one tile,
    slices - number pieces of longitude for one tile,
*/
QGLSceneNode* Earth::BuildSpherePart(int separation, qreal minSphereLat, qreal maxSphereLat,
                                     qreal minSphereLon, qreal maxSphereLon)
{
    // calculate spherical min and max lon and lat
    qreal minMerLat = Lat2MercatorLatitude(minSphereLat);
    qreal maxMerLat = Lat2MercatorLatitude(maxSphereLat);

    // all stacks and slices
    int stacks = 32;
    int slices = 32;
    if (curZoom > 5)
    {
        stacks = qPow(2, qFloor(curZoom));
        slices = qPow(2, qFloor(curZoom));
    }

    // stacks and slices for one tile
//    qreal stacksForOne = stacks/(float)separation;
//    qreal slicesForOne = slices/(float)separation;

    qreal oneSphereStackDegrees = (maxSphereLat - minSphereLat) / qreal(stacks/(qreal)separation);
    qreal oneSphereSliceDegrees = (maxSphereLon - minSphereLon) / qreal(slices/(qreal)separation);

    QVector3D curDecart;
    QVector3D curDecartNext;

    qreal curMerLat;
    qreal curMerLatNext;
    qreal curSphereLatNext;

    QGLBuilder tempBuilder;
    QGeometryData prim;

    double yTexCoord;
    double yTexCoordNext;
    double xTexCoord;
    double xTexCoordNext;

    // maxSphereLon with with an error of calculations (approximately)
    bool firstFlag = true;
    bool lastFlag = false;
    qreal curSphereLon, curSphereLat;

    for (int lat_iter = 0; lat_iter <= stacks/(qreal)separation; lat_iter++)
    {
        curSphereLat = minSphereLat + lat_iter * oneSphereStackDegrees;
        // calculate next point latitude
        curSphereLatNext = curSphereLat + oneSphereStackDegrees;

        // if next latitude is max latitude
        if (qAbs(curSphereLatNext - maxSphereLat) < 0.0001)
        {
            curSphereLatNext = maxSphereLat;
            curSphereLat = maxSphereLat - oneSphereStackDegrees;
            lastFlag = true;
        }

        // calculate mercator latitude
        curMerLat = Lat2MercatorLatitude(curSphereLat);
        curMerLatNext = Lat2MercatorLatitude(curSphereLatNext);

        // calculate xTexCoord for this lat and next lat
        yTexCoord = (curMerLat - minMerLat) / qreal(maxMerLat - minMerLat);
        yTexCoordNext = (curMerLatNext - minMerLat) / qreal(maxMerLat - minMerLat);

        prim.clear();

        for (int lon_iter = 0; lon_iter <= slices/(qreal)separation; lon_iter++)
        {
            curSphereLon = minSphereLon + lon_iter * oneSphereSliceDegrees;
            // calculate decart coordinates (x,y,z) for Vertex and Normal
            curDecart = llh2xyz((curSphereLat), curSphereLon);

            // calculate texture coordinates
            xTexCoord = ((curSphereLon) - (minSphereLon))/qreal((maxSphereLon) - (minSphereLon));
            if (firstFlag)
                yTexCoord = 0;

            prim.appendVertex(curDecart);
            prim.appendNormal(curDecart);
            prim.appendTexCoord(QVector2D(xTexCoord, (yTexCoord)));

            // all for next point
            curDecartNext = llh2xyz((curSphereLatNext), curSphereLon);
            xTexCoordNext = ((curSphereLon) - (minSphereLon)) / qreal((maxSphereLon) - (minSphereLon));
            if (lastFlag)
                yTexCoordNext = 1;

            prim.appendVertex(curDecartNext);
            prim.appendNormal(curDecartNext);
            prim.appendTexCoord(QVector2D(xTexCoordNext, (yTexCoordNext)));
        }

        // add QGeometryData to our builder
        tempBuilder.addQuadStrip(prim);
        firstFlag = false;
        if (lastFlag)
            break;
    }

    return tempBuilder.finalizedSceneNode();
}

/*!
    verification of the existence texture file in cache directory.
    return true if file exist.
*/
bool Earth::checkTextureFile(int separation, int lonTileNum, int latTileNum, int cur_zoom)
{
    QString texFilePath = QString(cacheDir+"/%1-%2-%3.%4").arg(cur_zoom).
                          arg(lonTileNum).arg(separation-1-latTileNum).arg(tileExtension);
    return QFile::exists(texFilePath);
}

/*!
    add texture to current QGLSceneNode object.
*/
bool Earth::addTextureToTile(QGLSceneNode* tempNode, int separation, int lonTileNum,
                             int latTileNum, int cur_zoom)
{
//    if (separation > 1){
    QGLTexture2D* tex;
    tex = new QGLTexture2D();
//    tex->setSize(QSize(512, 256));

    QString t_filepath = cacheDir+"/%1-%2-%3.%4";
    QString filepath(t_filepath.arg(cur_zoom).arg(lonTileNum).arg(separation-1-latTileNum).arg(tileExtension));

    if (!QFile::exists(filepath))
    {
        return false;
    }

    QUrl url;
    url.setPath(filepath);
    url.setScheme(QLatin1String("file"));
    tex->setUrl(url);

    QGLMaterial *mat1 = new QGLMaterial;
    mat1->setTexture(tex, 0);

//    m_LoadedTextures.insert(filepath, tex);

//    TileCacheNumber tileNumber = TileCacheNumber(cur_zoom, lonTileNum,separation-1-latTileNum);
//    m_LoadedTextures.insert(tileNumber, tex);
    int earthMat = tempNode->palette()->addMaterial(mat1);

    tempNode->setMaterialIndex(earthMat);
    tempNode->setEffect(QGL::LitModulateTexture2D);
    return true;
}

/*!
    starts a thread for downloading tile texture.
*/
void Earth::tileDownload(qint32 cur_zoom, qint32 separation, qint32 lonTileNum, qint32 latTileNum)
{
    QString m_filepath = cacheDir+"/%1-%2-%3.%4";
    QString textureStorePath = (m_filepath.arg(cur_zoom).arg(lonTileNum).arg(separation-1-latTileNum).arg(tileExtension));

    TileDownloader *tileDownloader = new TileDownloader(separation, lonTileNum, latTileNum,
                                                        cur_zoom, textureStorePath, currentMapThemeUrl);

    downloadQueue->insert(textureStorePath, tileDownloader);

    QObject::connect(tileDownloader, &TileDownloader::resultReady,
                     this, &Earth::textureDownloaded);
//    QObject::connect(tileDownloader, &TileDownloader::resultReady,
//                     tileDownloader, &TileDownloader::deleteLater);

    // Starts an event loop, and emits workerThread->started()
//    tileDownloader->downloadedData();
    if (zoom == 0)
    {
        if (!QFile::exists(textureStorePath))
            Sleeper::msleep(50);
    }
}

/*!
    removal of old tiles and create new when changed zoom
*/
void Earth::updateTilesSlot(qreal scale, GeoCoords geoCoords)
{
    curZoom = log10(scale)/log10(2.0);
    curScale = scale;
    // save current coordinates
    curGeoCoords = geoCoords;

//    curZoom+=1;
    newZoomFlag = false;

    if (zoom != qFloor(curZoom))
    {
        newZoomFlag = true;
        zoom_old = zoom;
        zoom = qFloor(curZoom);

        if (curZoom>12)
        {
            QList<QGLSceneNode *> childrens = allChildren();
            for (int i = 0; i < childrens.size(); ++i)
            {
                removeNode(childrens.at(i));
            }
            tileNodeCache->clear();
        }
        else
        {
            int separation_old = qPow(2, zoom_old);
            for (int y = 0; y < separation_old; y++)
            {
                for (int x = 0; x < separation_old; x++)
                {
                    TileCacheNumber currentCacheNumber = TileCacheNumber(zoom_old, x, y);

                    if (tileNodeCache->contains(currentCacheNumber))
                    {
                        tileNodeCache->object(currentCacheNumber)->glSceneNodeObject()->setOptions(QGLSceneNode::HideNode);
                        removeNode(tileNodeCache->object(currentCacheNumber)->glSceneNodeObject());
                    }
                }
            }
        }

        buildEarthNode(a, 10, curZoom);
        emit displayed();
    }
    else
    {
        // only move camera
        buildEarthNode(a, 10, curZoom);
    }
}

Earth::~Earth()
{
    // clean textures
//    for (int i=0; i<m_LoadedTextures.count(); ++i) {
//        m_LoadedTextures.at(i)->cleanupResources();
//    }
    m_texture->cleanupResources();
}

void Earth::cleanupResources()
{

}

void Earth::setMapTheme(QString mapThemeName)
{
    if (mapThemeList.contains(mapThemeName) && mapThemeName != currentMapTheme)
    {
        // save to config
        QSettings *settings = new QSettings(m_configData->configFile, QSettings::IniFormat);
        settings->setValue("common/map_theme", mapThemeName);
        settings->sync();
        delete settings;

        currentMapTheme = mapThemeName;
        currentMapThemeUrl = mapThemeList.value(mapThemeName);

        tileExtension = (currentMapTheme.indexOf("Satellite") >= 0) ? "jpg": "png";

        tileNodeCache->clear();
        clearTileCache();
        buildEarthNode(a, 10, curZoom);
    }
}

void Earth::clearTileCache()
{
    // list files from cache dir
    QDir dir(cacheDir);
    QStringList lstFiles = dir.entryList(QDir::Files);
    QRegExp rxPNG(dir.absolutePath() + "/"+"(\\d+)-(\\d+)-(\\d+).png");
    QRegExp rxJPG(dir.absolutePath() + "/"+"(\\d+)-(\\d+)-(\\d+).jpg");

    //remove files (earth tile e.g. 0-0-0.png)
    foreach (QString entry, lstFiles)
    {
        QString entryAbsPath = dir.absolutePath() + "/" + entry;
//        QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
        if (rxPNG.exactMatch(entryAbsPath))
            QFile::remove(entryAbsPath);
        else if (rxJPG.exactMatch(entryAbsPath))
            QFile::remove(entryAbsPath);
    }
}
