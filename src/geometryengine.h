#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

#include <utility> 

class QOpenGLShaderProgram;

class GeometryEngine : protected QOpenGLFunctions
{
public:
    GeometryEngine();
    virtual ~GeometryEngine();

    std::tuple<double, double, double, double> getMinMaxCoords() const;
    void drawModelGeometry(QOpenGLShaderProgram& program);

private:
    void initModelGeometry();

    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;

    size_t numElems = 0;
    double minX = 0.0;
    double maxX = 1.0;
    double minY = 0.0;
    double maxY = 2.0;
};
