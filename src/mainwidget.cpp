#include "mainwidget.h"
#include "geometryengine.h"

#include <QMouseEvent>

#include <math.h>

MainWidget::MainWidget(QWidget* parent)
  : QOpenGLWidget(parent)
{
}

MainWidget::~MainWidget() {
  makeCurrent(); // make sure the context is current when deleting the buffers
  delete m_model;
  doneCurrent();
}

void MainWidget::initializeGL() {
  initializeOpenGLFunctions();

  glClearColor(0.92f, 0.95f, 1.0f, 1.0f);

  initShaders();

  m_model = new GeometryEngine();
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

  if (!m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource))
    close();

  if (!m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource))
    close();

  if (!m_shaderProgram.link())
    close();

  if (!m_shaderProgram.bind())
    close();
}

void MainWidget::paintGL() {
  glClearColor(0.92f, 0.95f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  m_shaderProgram.setUniformValue("ScaleParams", m_offsetX, m_offsetY, m_scaleX, m_scaleY);

  m_model->drawModelGeometry(m_shaderProgram);
}

void MainWidget::fitAll() {
  const auto [minX, maxX, minY, maxY] = m_model->getMinMaxCoords();
  if (m_screenW != 0 && m_screenH != 0 && (maxX - minX) > 0) {
    if (m_screenH / m_screenW < (maxY - minY) / (maxX - minX)) { // height limited
      m_scaleY  = static_cast<float>(2.0 / (maxY - minY));
      m_scaleX  = static_cast<float>(m_scaleY * m_screenH / m_screenW);
      m_offsetY = static_cast<float>((maxY + minY) / (maxY - minY));
      m_offsetX = static_cast<float>((maxX + minX) / (maxY - minY) * m_screenH / m_screenW);
    } else { // width limited
      m_scaleX  = static_cast<float>(2.0 / (maxX - minX));
      m_scaleY  = static_cast<float>(m_scaleX * m_screenW / m_screenH);
      m_offsetY = static_cast<float>((maxY + minY) / (maxX - minX) * m_screenW / m_screenH);
      m_offsetX = static_cast<float>((maxX + minX) / (maxX - minX));
    }
  }
  m_isZoomFit = true;
}

void MainWidget::resizeGL(int w, int h) {
  const auto oldW = static_cast<double>(m_screenW), 
             oldH = static_cast<double>(m_screenH);
  m_screenW = w;  m_screenH = h;

  if (m_isZoomFit) {
    fitAll();
  } else {
    const double dh = oldH / static_cast<double>(h);
    const double dw = oldW / static_cast<double>(w);
    m_scaleX  = static_cast<float>(dw * m_scaleX);
    m_scaleY  = static_cast<float>(dh * m_scaleY);
    m_offsetX = static_cast<float>(dw * m_offsetX);
    m_offsetY = static_cast<float>(dh * m_offsetY);
  }
}

void MainWidget::wheelEvent(QWheelEvent* e) {
  QPoint n = e->angleDelta();
  const int dx = n.x();
  const int dy = n.y();
  const int d = (abs(dx) > abs(dy)) ? dx : dy;

  const double zoom = (d > 0)? 1.2 : 1.0/1.2;

  QPoint screenCursorPos = e->pos();

  // Real coordinates for this cursor location
  const double x = (screenCursorPos.x()*2.0/width() - 1.0  + m_offsetX) / m_scaleX;
  const double y = (1.0 - screenCursorPos.y()*2.0/height() + m_offsetY) / m_scaleY;

  m_offsetX += x*(zoom - 1.0)*m_scaleX;
  m_offsetY += y*(zoom - 1.0)*m_scaleY;
  m_scaleX  *= zoom;
  m_scaleY  *= zoom;

  m_isZoomFit = false;

  update();
}

void MainWidget::mousePressEvent(QMouseEvent* e) {
  m_mousePressPosition = QVector2D(e->localPos());
  m_dragging = true;
}

void MainWidget::mouseReleaseEvent(QMouseEvent*) {
  m_dragging = false;
}

void MainWidget::mouseMoveEvent(QMouseEvent* e) {
  if (m_dragging) {
    auto newMousePos = QVector2D(e->localPos());
    const QVector2D diff = newMousePos - m_mousePressPosition;

    m_offsetX -= 2.0f * diff.x() / width();
    m_offsetY += 2.0f * diff.y() / height();
     
    m_mousePressPosition = std::move(newMousePos);

    m_isZoomFit = false;

    update();
  }
}
void MainWidget::keyPressEvent(QKeyEvent* keyevent) {
  const int key = keyevent->key();
  if (key == Qt::Key_F) {
    fitAll();
    update();
  }
}
