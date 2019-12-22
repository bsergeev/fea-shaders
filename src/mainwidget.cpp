#include "mainwidget.h"

#include <QMouseEvent>

#include <math.h>

MainWidget::MainWidget(QWidget* parent)
  : QOpenGLWidget(parent)
{
}

MainWidget::~MainWidget() {
  // Make sure the context is current when deleting the texture and the buffers.
  makeCurrent();
  delete texture;
  delete geometries;
  doneCurrent();
}

void MainWidget::mousePressEvent(QMouseEvent* e) {
//  mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent* e) {
//     // Mouse release position - mouse press position
//     QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;
// 
//     // Rotation axis is perpendicular to the mouse position difference
//     // vector
//     QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
// 
//     // Accelerate angular speed relative to the length of the mouse sweep
//     qreal acc = diff.length() / 100.0;
// 
//     // Calculate new rotation axis as weighted sum
//     rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();
// 
//     // Increase angular speed
//     angularSpeed += acc;
}

// void MainWidget::timerEvent(QTimerEvent*) {
//     angularSpeed *= 0.99; // decrease angular speed (friction)
//     if (angularSpeed < 0.01) {
//         angularSpeed = 0.0; // stop rotation when speed goes below threshold
//     } else {
//         rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
//         update();
//     }
// }

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
attribute vec4 aPos;
attribute vec4 aColor;
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

void MainWidget::resizeGL(int w, int h) {
  int ScrW = w;
  int ScrH = h;
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
}

void MainWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program.setUniformValue("ScaleParams", XOffset, YOffset, CoordScaleX, CoordScaleY);

    geometries->drawCubeGeometry(&program);
}
