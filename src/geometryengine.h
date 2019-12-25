#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

#include <utility> 

class QOpenGLShaderProgram;

class GeometryEngine : protected QOpenGLFunctions
{
public:
    GeometryEngine();
    ~GeometryEngine();

    std::tuple<double, double, double, double> getMinMaxCoords() const;
    void drawModelGeometry(QOpenGLShaderProgram& program);

private:
    void initModelGeometry();

    QOpenGLBuffer m_arrayBuf;
    QOpenGLBuffer m_indexBuf;

    size_t m_numElems = 0;
    double m_minX = 0.0;
    double m_maxX = 0.0;
    double m_minY = 0.0;
    double m_maxY = 0.0;
};
