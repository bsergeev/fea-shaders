#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector2D>

class GeometryEngine;

class GeometryEngine;

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit MainWidget(QWidget* parent = nullptr);
  ~MainWidget();

protected:
  void mousePressEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  void initShaders();

private:
  QOpenGLShaderProgram program;

  GeometryEngine* model = nullptr;

  float XOffset = 0.0f;
  float YOffset = 0.0f;
  float CoordScaleX = 1.0f;
  float CoordScaleY = 1.0f;

  bool m_isZoomFit = true;
  bool m_dragging = false;

  QVector2D   mousePressPosition;
};
