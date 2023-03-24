#ifndef OPENGLFRACTALWIDGET_H
#define OPENGLFRACTALWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLShader>
#include <vector>
#include <complex>
#include <random>

struct Gradient
{
    static float getColor(float iteration, int max_iteration)
    {
        return iteration/max_iteration;
    }
};

class OpenGLFractalWidget : public QOpenGLWidget, public QOpenGLFunctions
{
    Q_OBJECT
//    std::vector<float> texture;
//    GLuint textureId;
    double left = -1.0;
    double right = 1;
    double top = -1.0;
    double bottom = 1;
    float scale;
    float xPivot;
    float yPivot;
    QOpenGLShaderProgram* program;

    int xPivotLocation;
    int yPivotLocation;
    int constantLocation;    // vec2
    int pixelsLocation;    // vec2
    int scaleLocation;    // float
    int max_iterationLocation; // int
    int exponentLocation; // float
    int log_expressionLocation;

    bool exponentChangeMode = false;
    QPointF dragStartPoint;
    float dragStartExponent;

public:
    int max_iteration = 200;
    int xPixels = 1000;
    int yPixels = 1000;
    std::complex<float> constant;
    float exponent;
    bool log_expression;


public:
    explicit OpenGLFractalWidget(QWidget *parent = nullptr);
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;


protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

private:
    std::complex<float> computeNext(std::complex<float> current);
    float normSquare(std::complex<float> z);
    int computeIterations(std::complex<float> z0);
    void setPixelColor(int x, int y, float color);
    void render(std::complex<int> render_size);
    void zoomIn(QPointF position);
    void zoomOut(QPointF position);

signals:
    void constantChanged(float real, float imag);
    void exponentChanged(float exponent);
public slots:
    void updateTexture();
};

#endif // OPENGLFRACTALWIDGET_H
