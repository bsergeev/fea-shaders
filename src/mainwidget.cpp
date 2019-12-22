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
  mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent* e) {
    // Mouse release position - mouse press position
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

    // Rotation axis is perpendicular to the mouse position difference
    // vector
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // Accelerate angular speed relative to the length of the mouse sweep
    qreal acc = diff.length() / 100.0;

    // Calculate new rotation axis as weighted sum
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // Increase angular speed
    angularSpeed += acc;
}

void MainWidget::timerEvent(QTimerEvent*) {
    angularSpeed *= 0.99; // decrease angular speed (friction)
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0; // stop rotation when speed goes below threshold
    } else {
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
        update();
    }
}

void MainWidget::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    geometries = new GeometryEngine();

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

void MainWidget::initShaders() {
  const char* vertexShaderSource = R"(
#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;
attribute vec4 a_position;
attribute vec2 a_texcoord;
varying vec2 v_texcoord;

void main() {
  gl_Position = mvp_matrix * a_position;
  v_texcoord = a_texcoord;
})";
  const char* fragmentShaderSource = R"(
#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform sampler2D texture;
varying vec2 v_texcoord;

void main() {
  gl_FragColor = texture2D(texture, v_texcoord);
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

void MainWidget::initTextures() {
    texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::Repeat);
}

void MainWidget::resizeGL(int w, int h) {
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void MainWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);

    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);

    // Use texture unit 0 which contains cube.png
    program.setUniformValue("texture", 0);

    // Draw cube geometry
    geometries->drawCubeGeometry(&program);
}