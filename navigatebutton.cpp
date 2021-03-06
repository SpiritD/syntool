#include "navigatebutton.h"
#include "QVector3DArray"
#include "QGLVertexBundle"
#include <QArray>
#include <qmath.h>

NavigateButton::NavigateButton(QObject *parent, QSharedPointer<QGLMaterialCollection> palette, float maxScale)
    : QGLSceneNode(parent)
{
    qint32 m_maxZoom = qint32(log10(maxScale)/log10(2.0));
    numberDivisions = m_maxZoom*4 + 1;
    m_curDivisions = -1;

    setObjectName("Buttons");
    setPalette(palette);
    setOption(QGLSceneNode::CullBoundingBox, false);
    createButtons();
    viewPointer = qobject_cast<QGLView*>(parent);
}

NavigateButton::~NavigateButton()
{
    for (int i=0; i<m_LoadedTextures.count(); ++i) {
        m_LoadedTextures.at(i)->cleanupResources();
    }
}

void NavigateButton::draw(QGLPainter *painter, float scale, bool drawCoords, qreal lat, qreal lon)
{
    qint32 curDivisions = qint32((log10(scale)/log10(2.0))*4);

    painter->projectionMatrix().push();
    painter->modelViewMatrix().push();

    QRect rect = painter->currentSurface()->viewportRect();
    QMatrix4x4 projm;
    projm.ortho(rect);
    painter->projectionMatrix() = projm;
    painter->modelViewMatrix().setToIdentity();

    if (navButton->position().isNull())
    {
        QVector2D pos(m_size - 10, 38);
        navButton->setPosition(pos);
    }

    if (zoomInButton->position().isNull())
    {
        QVector2D pos(m_size - 10, 96);
        zoomInButton->setPosition(pos);
    }

    if (zoomOutButton->position().isNull())
    {
        QVector2D pos(m_size - 10, 288);
        zoomOutButton->setPosition(pos);
    }

    if (m_curDivisions != curDivisions)
    {
        qint32 yPos = 254 - (124/float(numberDivisions)*curDivisions);
        QVector2D pos(m_size - 10, yPos);
        scaleButton->setPosition(pos);
        m_curDivisions = curDivisions;
    }

    glDisable(GL_DEPTH_TEST);

    // draw scale line
    glLineWidth(2.0f);

    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatColor);

    painter->setVertexAttribute(QGL::Position , verts);
    painter->setColor(QColor(0,80,140));
    painter->draw(QGL::Lines,verts.size());

    QGLSceneNode::draw(painter);

    if (drawCoords && lat > -91 && lat < 91)
    {
        QString viewStr = "lat: " + QString::number(lat) + ", lon:" + QString::number(lon);
        drawText(painter, viewStr,
                 QPoint(viewPointer->size().width() - 350, viewPointer->size().height() - 20));
    }

    glEnable(GL_DEPTH_TEST);


    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();
}

void NavigateButton::drawText(QGLPainter *painter, const QString& str, const QPoint& posn)
{
    QFont f = QFont();
    f.setBold(true);
    f.setFamily("Monospace");
    f.setPixelSize(20);
    QFontMetrics metrics = QFontMetrics(f);
    QRect rect = metrics.boundingRect(str);
    rect.adjust(0, 0, 1, 1);

    QImage image(rect.size(), QImage::Format_ARGB32);
    image.fill(0);
    QPainter p2(&image);
    p2.setFont(f);
    p2.setPen(Qt::white);
    p2.drawText(-rect.x(), metrics.ascent(), str);
    p2.end();

    QGLTexture2D texture;
    texture.setImage(image);

    int x = posn.x();
    int y = posn.y();

    QVector2DArray vertices;
    vertices.append(x + rect.x(), y + metrics.ascent());
    vertices.append(x + rect.x(), y - metrics.descent());
    vertices.append(x + rect.x() + rect.width(), y - metrics.descent());
    vertices.append(x + rect.x() + rect.width(), y + metrics.ascent());

    QVector2DArray texCoord;
    texCoord.append(0.0f, 0.0f);
    texCoord.append(0.0f, 1.0f);
    texCoord.append(1.0f, 1.0f);
    texCoord.append(1.0f, 0.0f);

    painter->clearAttributes();
    painter->setStandardEffect(QGL::FlatReplaceTexture2D);
    texture.bind();
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::TextureCoord0, texCoord);
    painter->draw(QGL::TriangleFan, 4);
    painter->setStandardEffect(QGL::FlatColor);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void NavigateButton::createButtons()
{
    navButton = createButton(":/navigate_circle.png");

    zoomInButton = createButton(":/icons/zoom_in.png");

    zoomOutButton = createButton(":/icons/zoom_out.png");

    scaleButton = createButton(":/icons/scale_button.png");

    verts.clear();
    verts.append(m_size - 10, 254);
    verts.append(m_size - 10, 130);

    verts.append(m_size - 6, 254);
    verts.append(m_size - 14, 254);

    verts.append(m_size - 6, 130);
    verts.append(m_size - 14, 130);
}

QGLSceneNode* NavigateButton::createButton(QString imageName)
{
    // create _button
    QGLSceneNode* _button = new QGLSceneNode(this);
    _button->setObjectName(imageName);

    QGLMaterial *mat = new QGLMaterial;
    QImage im(imageName);
    m_size = 48;
    QGLTexture2D *tex = new QGLTexture2D(mat);
    m_LoadedTextures.push_back(tex);
    tex->setImage(im);
    mat->setTexture(tex);

    _button->setMaterial(mat);
    _button->setEffect(QGL::FlatReplaceTexture2D);

    QGeometryData data;
    QSize f = im.size() / 2;
    QVector2D a(-f.width(), -f.height());
    QVector2D b(f.width(), -f.height());
    QVector2D c(f.width(), f.height());
    QVector2D d(-f.width(), f.height());
    QVector2D ta(0, 1);
    QVector2D tb(1, 1);
    QVector2D tc(1, 0);
    QVector2D td(0, 0);
    data.appendVertex(a, b, c, d);
    data.appendTexCoord(ta, tb, tc, td);
    data.appendIndices(0, 1, 2);
    data.appendIndices(0, 2, 3);

    // the right hand arrow geometry is same as above, flipped X <-> -X
    data.appendGeometry(data);
    data.texCoord(4).setX(1);
    data.texCoord(5).setX(0);
    data.texCoord(6).setX(0);
    data.texCoord(7).setX(1);
    data.appendIndices(4, 5, 6);
    data.appendIndices(4, 6, 7);

    _button->setGeometry(data);
    _button->setCount(6);
    _button->setOption(QGLSceneNode::CullBoundingBox, false);
    return _button;
}

void NavigateButton::drawSector(QVector2D navigateVector, QGLPainter *painter)
{
    painter->projectionMatrix().push();
    painter->modelViewMatrix().push();

    QRect rect = painter->currentSurface()->viewportRect();
    QMatrix4x4 projm;
    projm.ortho(rect);
    painter->projectionMatrix() = projm;
    painter->modelViewMatrix().setToIdentity();

    glDisable(GL_DEPTH_TEST);

//    QGLSceneNode::draw(painter);

///////////////////////////////////////////////////
    painter->setStandardEffect(QGL::FlatColor);
    int alpha = qSqrt(qPow(navigateVector.x(), 2)+qPow(navigateVector.y(), 2));
//    painter->setColor(QColor(88, 131, 190, alpha*4));
    painter->setColor(QColor(46, 136, 216, alpha*4));

    QVector2DArray vertices;
    QVector3DArray normals;

    QVector3D pos = navButton->position();
    int radius = navButton->boundingBox().maximum().x() - pos.x();

    float a=qAtan(navigateVector.y()/navigateVector.x());

    vertices.append(pos.x(), pos.y());
    int x[5] = {};
    int y[5] = {};
    for (int i= -2; i<3; i++)
    {
        x[i]= (navigateVector.x() >= 0) ? pos.x()-radius*qCos(a+i/6.0) : pos.x()+radius*qCos(a+i/6.0);
        y[i]= (navigateVector.x() >= 0) ? pos.y()-radius*qSin(a+i/6.0) : pos.y()+radius*qSin(a+i/6.0);
        vertices.append(x[i], y[i]);
    }

    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);
    normals.append(0.0f, 0.0f, 1.0f);

    painter->clearAttributes();
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::Normal, normals);

    painter->draw(QGL::TriangleFan, 6);

////////////////////////////////////////////////////

    glEnable(GL_DEPTH_TEST);

    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();
}
