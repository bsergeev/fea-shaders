#include "mainwidget.h"
#include "geometryengine.h"

#include <QMouseEvent>

#include <iostream> // <<< DEBUG
#include <math.h>

MainWidget::MainWidget(QWidget* parent)
  : QOpenGLWidget(parent)
{
}

MainWidget::~MainWidget() {
  makeCurrent(); // make sure the context is current when deleting the buffers
  delete geometries;
  doneCurrent();
}

void MainWidget::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();

//     glEnable(GL_DEPTH_TEST);
//     glEnable(GL_CULL_FACE);

    geometries = new GeometryEngine();

    // Use QBasicTimer because its faster than QTimer
//    timer.start(12, this);
}

void MainWidget::initShaders() {
  const char* vertexShaderSource = R"(
#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform   vec4 ScaleParams;
attribute vec3 aPos;
attribute vec3 aColor;
varying   vec3 ourColor;

void main() {
  gl_Position = vec4(ScaleParams.z*aPos.x - ScaleParams.x, ScaleParams.w*aPos.y - ScaleParams.y, aPos.z, 1.0);
  ourColor = aColor;
})";
  const char* fragmentShaderSource = R"(
#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

varying vec3 ourColor;
void main() {
  gl_FragColor = vec4(ourColor, 1.0f);
})";

  if (!program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource))
    close();

  if (!program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource))
    close();

  if (!program.link())
    close();

  if (!program.bind())
    close();
}

void MainWidget::paintGL() {
  glClearColor(0.92f, 0.95f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

  program.setUniformValue("ScaleParams", XOffset, YOffset, CoordScaleX, CoordScaleY);

  geometries->drawCubeGeometry(&program);
}

void MainWidget::resizeGL(int w, int h) {
  int ScrW = w;
  int ScrH = h;

  if (m_isZoomFit) {
    const auto [minX, maxX, minY, maxY] = geometries->getMinMaxCoords();
    if (ScrW != 0 && ScrH != 0 && (maxX - minX) > 0) {
      if (ScrH / ScrW < (maxY - minY) / (maxX - minX)) { // height limited
        CoordScaleY = static_cast<float>(2.0 / (maxY - minY));
        CoordScaleX = static_cast<float>(CoordScaleY * ScrH / ScrW);
        YOffset = static_cast<float>((maxY + minY) / (maxY - minY));
        XOffset = static_cast<float>((maxX + minX) / (maxY - minY) * ScrH / ScrW);
      } else { // width limited
        CoordScaleX = static_cast<float>(2.0 / (maxX - minX));
        CoordScaleY = static_cast<float>(CoordScaleX * ScrW / ScrH);
        YOffset = static_cast<float>((maxY + minY) / (maxX - minX) * ScrW / ScrH);
        XOffset = static_cast<float>((maxX + minX) / (maxX - minX));
      }
    }
  } else {
    //TODO
  }
}

void MainWidget::wheelEvent(QWheelEvent* e) {
  QPoint n = e->angleDelta();
  const int dx = n.x();
  const int dy = n.y();
  const int d = (abs(dx) > abs(dy)) ? dx : dy;

  const double zoom = (d > 0)? 1.2 : 1.0/1.2;

  // Screen coordinates of the cursor
  QPoint pos = e->pos();

  // Real coordinates for this cursor location
  const double x = (pos.x()*2.0/width() - 1.0  + XOffset)/CoordScaleX;
  const double y = (1.0 - pos.y()*2.0/height() + YOffset)/CoordScaleY;

  XOffset += x*(zoom - 1.0)*CoordScaleX;
  YOffset += y*(zoom - 1.0)*CoordScaleY;
  CoordScaleX *= zoom;
  CoordScaleY *= zoom;

  update();
}

void MainWidget::mousePressEvent(QMouseEvent* e) {
  mousePressPosition = QVector2D(e->localPos());
  m_dragging = true;
}

void MainWidget::mouseReleaseEvent(QMouseEvent*) {
  m_dragging = false;
}

void MainWidget::mouseMoveEvent(QMouseEvent* e) {
  if (m_dragging) {
    QVector2D newMousePos = QVector2D(e->localPos());
    QVector2D diff = newMousePos - mousePressPosition;

    XOffset -= 2.0f * diff.x() / width();
    YOffset += 2.0f * diff.y() / height();
     
    mousePressPosition = std::move(newMousePos);
    update();
  }
}