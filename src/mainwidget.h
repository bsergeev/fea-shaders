#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QVector2D>

class GeometryEngine;

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit MainWidget(QWidget* parent = nullptr);
  ~MainWidget();

  void fitAll();

protected:
  void mousePressEvent  (QMouseEvent* e) override;
  void mouseReleaseEvent(QMouseEvent* e) override;
  void mouseMoveEvent   (QMouseEvent* e) override;
  void wheelEvent   (QWheelEvent* event) override;
  void keyPressEvent(QKeyEvent* keyEvnt) override;

  void initializeGL()         override;
  void resizeGL(int w, int h) override;
  void paintGL()              override;

private:
  void initShaders();

//data:
  QOpenGLShaderProgram m_shaderProgram;

  GeometryEngine* m_model = nullptr;

  float m_offsetX = 0.0f;
  float m_offsetY = 0.0f;
  float m_scaleX  = 1.0f;
  float m_scaleY  = 1.0f;

  int  m_screenW = 0;
  int  m_screenH = 0;

  bool m_isZoomFit = true;
  bool m_dragging = false;
  QVector2D m_mousePressPosition{};
};
