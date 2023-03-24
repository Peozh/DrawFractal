#include "openglfractalwidget.h"
//#include <QOffscreenSurface>
#include <QFile>

OpenGLFractalWidget::OpenGLFractalWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{
    std::random_device rnd_device;
    std::mt19937 mersenne_engine { rnd_device() };
    std::uniform_real_distribution<float> dist {-1.0f, 1.0f};
    constant.real(dist(mersenne_engine));
    constant.imag(dist(mersenne_engine));
    exponent = 2.0f;
    log_expression = false;

    this->scale = 1.0f / float(xPixels / 2);
    this->xPivot = float(xPixels/2.0);
    this->yPivot = float(yPixels/2.0);
    this->xPixels = 1000;
    this->yPixels = 1000;

//    texture.resize(xPixels*yPixels);
//    render({xPixels, yPixels});

//    QOffscreenSurface surf;
//    surf.create();

//    QOpenGLContext ctx;
//    ctx.create();
//    ctx.makeCurrent(&surf);

//    qDebug () << (const char*)ctx.functions()->glGetString(GL_VERSION); //460
//    qDebug () << (const char*)ctx.functions()->glGetString(GL_EXTENSIONS);
}

void OpenGLFractalWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        this->dragStartPoint = event->position();
        if (event->modifiers() && Qt::KeyboardModifier::AltModifier) { exponentChangeMode = true; dragStartExponent = exponent; }
    }
    else if (event->buttons() & Qt::RightButton)
    {
        this->log_expression = !log_expression;
        this->update();
    }
}

void OpenGLFractalWidget::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << "(x, y) = (" << event->position().x() << ", " << event->position().y() << ")";
    if ((event->buttons() & Qt::LeftButton) && exponentChangeMode) // change exponent
    {
        this->exponent = dragStartExponent + (dragStartPoint.y() - event->position().y())*0.001f;
        emit exponentChanged(exponent);
    }
    else if ((event->buttons() & Qt::LeftButton))  // change constant
    {
        this->constant.real((event->position().x() - this->xPixels/2)/(this->xPixels/2));
        this->constant.imag((event->position().y() - this->yPixels/2)/(this->yPixels/2));
        emit constantChanged(constant.real(), constant.imag());
    }
    this->update();
}

void OpenGLFractalWidget::mouseReleaseEvent(QMouseEvent *event)
{
    exponentChangeMode = false;
}

void OpenGLFractalWidget::wheelEvent(QWheelEvent *event)
{
    if(event->angleDelta().y() > 0) // up Wheel
        zoomIn(event->position());
    else if(event->angleDelta().y() < 0) //down Wheel
        zoomOut(event->position());
//    this->updateTexture();
    this->update();
}

void OpenGLFractalWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1.0f);

//    render({xPixels, yPixels});

//    glGenTextures(1, &textureId); // 텍스쳐 id 에 텍스쳐를 하나 할당합니다.
//    glActiveTexture(GL_TEXTURE0); // 활성화할 텍스쳐 슬롯을 지정합니다.
//    glBindTexture(GL_TEXTURE_2D, textureId); // 현재 활성화된 텍스쳐 슬롯에 실질 텍스쳐를 지정합니다.
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, xPixels, yPixels, 0, GL_RED, GL_FLOAT, texture.data()); // 텍스쳐 이미지가 RED 단일 채널이며, float 입니다. border 는 0 만 유효합니다.
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // s 축의 비어있는 텍스쳐 외부를 border 색상으로 채웁니다.
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); // t 축의 비어있는 텍스쳐 외부를 border 색상으로 채웁니다.
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // 텍스쳐 확대 시 fragment 를 최근접 값으로 설정합니다.
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // 텍스쳐 축소 시 fragment 를 최근접 값으로 설정합니다.
//    float colour[4] = {0.0f, 0.0f, 0.0f, 0.0f};
//    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colour); // 텍스쳐 border 색상을 결정합니다.
//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // (default)

    glEnable(GL_DEPTH_TEST); // 최적화 : Depth 테스트 실패 시 그려지지 않음
    glDepthFunc(GL_LEQUAL); // 겹치거나 가까우면 그리기
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // 스텐실 테스트와 뎁스 테스트 모두 통과 시 stencil buffer 를 glStencilFunc 에서 지정한 ref 로 설정합니다. 나머지 경우 유지.

    glEnable(GL_BLEND); // 아래에서 설정할 블렌딩 효과 활성화
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 전면(SRC)과 뒷면(DST)에 각각 곱해줄 계수
    glBlendEquation(GL_FUNC_ADD); // 위에서 얻은 두 항 간의 연산 방법 (default)

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    glEnable(GL_CULL_FACE); // 최적화 : 후면 제거 활성화
    glFrontFace(GL_CCW); // 전면/후면 판단 기준 (default)
    glCullFace(GL_BACK); // 후면만 폐기 (default)

    //-----------------------------------------------------------

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char* vshaderSrc = R"(
    #version 460 core
    layout (location = 0) in vec4 position;

    void main() {
        gl_Position = position;
    }
    )";
    vshader->compileSourceCode(vshaderSrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    QFile frag(":/fragmentShaderSource.frag");
    frag.open(QIODevice::ReadOnly | QIODevice::Text);
    fshader->compileSourceCode(frag.readAll());

    this->program = new QOpenGLShaderProgram();

    program->setParent(this);
    program->addShader(vshader);
    program->addShader(fshader);
    program->link();

    this->xPivotLocation = program->uniformLocation("xPivot"); // float
    this->yPivotLocation = program->uniformLocation("yPivot"); // float
    this->constantLocation = program->uniformLocation("constant");    // vec2 float
    this->pixelsLocation = program->uniformLocation("pixels");    // vec2 float
    this->scaleLocation = program->uniformLocation("scale");    // float
    this->max_iterationLocation = program->uniformLocation("max_iteration"); // int
    this->exponentLocation = program->uniformLocation("exponent");    // float
    this->log_expressionLocation = program->uniformLocation("log_expression");    // bool

//    glUseProgram(program_id);
//    glUniform2f(uniform_WindowSize, Width, Height);


}

void OpenGLFractalWidget::resizeGL(int w, int h)
{
    this->xPixels = w;
    this->yPixels = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right, top, bottom, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OpenGLFractalWidget::paintGL()
{
    program->bind();
    program->setUniformValue(xPivotLocation, this->xPivot);
    program->setUniformValue(yPivotLocation, this->yPivot);
    std::vector<float> constants = { this->constant.real(), this->constant.imag() };
    program->setUniformValueArray(constantLocation, constants.data(), 1, 2);
    std::vector<float> pixels = { float(this->xPixels), float(this->yPixels) };
    program->setUniformValueArray(pixelsLocation, pixels.data(), 1, 2);
    program->setUniformValue(scaleLocation, this->scale);
    program->setUniformValue(max_iterationLocation, this->max_iteration);
    program->setUniformValue(exponentLocation, this->exponent);
    program->setUniformValue(log_expressionLocation, this->log_expression);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right, top, bottom, -1, 1);

    GLfloat backgroundVertices[4][3] = {
        {-1, -1, 0},
        { 1, -1, 0},
        { 1,  1, 0},
        {-1,  1, 0} };
    GLubyte VertexOrder[4] = { 0, 1, 2, 3 };
//    GLfloat texture2DCoords[4][2] = {
//        {0, 1},
//        {1, 1},
//        {1, 0},
//        {0, 0}
//    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // draw background
    glStencilMask(0x00);
    // 바닥을 그리는 동안에는 stencil buffer를 수정하지 않습니다
    // stencil buffer 작성 (1 & 0x00 = 0) 비활성화
//    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, textureId);
    glVertexPointer(3, GL_FLOAT, 0, backgroundVertices);
//    glTexCoordPointer(2, GL_FLOAT, 0, texture2DCoords);
    glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_BYTE, VertexOrder);

//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisable(GL_TEXTURE_2D);

    program->release();
}

std::complex<float> OpenGLFractalWidget::computeNext(std::complex<float> current)
{
    // Zn^2 + C
    return current*current + constant;
}

float OpenGLFractalWidget::normSquare(std::complex<float> z)
{
    return z.real()*z.real() + z.imag()*z.imag();
}

int OpenGLFractalWidget::computeIterations(std::complex<float> z0)
{
    std::complex<float> zn = z0;
    int iteration = 0;
    while (normSquare(zn) < 4.0 && iteration < max_iteration)
    {
        zn = computeNext(zn);
        ++iteration;
    }
    const float norm = sqrt(normSquare(zn));
    const float floatIteration = float(iteration) - log2(std::max(1.0f, log2f(norm)));
    return floatIteration;
}

void OpenGLFractalWidget::setPixelColor(int x, int y, float color)
{
//    this->texture[x + y*xPixels] = color;
}

void OpenGLFractalWidget::render(std::complex<int> render_size)
{
    // compute the scale of the coordinates
    for (int y = 0; y < render_size.imag(); ++y)
    {
        for (int x = 0; x < render_size.real(); ++x)
        {
            // compute pixel's coordinates
            const float px = float(x - xPivot) * scale;
            const float py = float(y - yPivot) * scale;
            // compute color
            const float iterations = computeIterations({px, py});
            setPixelColor(x, y, Gradient::getColor(iterations, max_iteration));
        }
    }
}

void OpenGLFractalWidget::zoomIn(QPointF position)
{
    double zoomRatio = 0.9;

    double x_left_old = -xPivot * scale;
    double x_right_old = float(xPixels - xPivot) * scale;
    double xRatio = position.x()/this->xPixels; // [0, 1]
    double xCoord = x_left_old*(1.0-xRatio) + x_right_old*xRatio; // [left, right]
    double x_left_new = x_left_old*zoomRatio + xCoord*(1.0-zoomRatio);

    double y_top_old = -yPivot * scale;
    double y_bottom_old = float(yPixels - yPivot) * scale;
    double yRatio = position.y()/this->yPixels; // [0, 1]
    double yCoord = y_top_old*(1.0-yRatio) + y_bottom_old*yRatio; // [top, bottom]
    double y_top_new = y_top_old*zoomRatio + yCoord*(1.0-zoomRatio);

    this->scale *= zoomRatio;
    this->xPivot = -x_left_new/scale;
    this->yPivot = -y_top_new/scale;
}

void OpenGLFractalWidget::zoomOut(QPointF position)
{
    double zoomRatio = 0.9;

    double x_left_old = -xPivot * scale;
    double x_right_old = float(xPixels - xPivot) * scale;
    double xRatio = position.x()/this->xPixels; // [0, 1]
    double xCoord = x_left_old*(1.0-xRatio) + x_right_old*xRatio; // [left, right]
    double x_left_new = (x_left_old-xCoord)/zoomRatio + xCoord;

    double y_top_old = -yPivot * scale;
    double y_bottom_old = float(yPixels - yPivot) * scale;
    double yRatio = position.y()/this->yPixels; // [0, 1]
    double yCoord = y_top_old*(1.0-yRatio) + y_bottom_old*yRatio; // [top, bottom]
    double y_top_new = (y_top_old-yCoord)/zoomRatio + yCoord;

    this->scale /= zoomRatio;
    this->xPivot = -x_left_new/scale;
    this->yPivot = -y_top_new/scale;
}

void OpenGLFractalWidget::updateTexture()
{
//    render({xPixels, yPixels});
//    glActiveTexture(GL_TEXTURE0); // 활성화할 텍스쳐 슬롯을 지정합니다.
//    glBindTexture(GL_TEXTURE_2D, textureId); // 현재 활성화된 텍스쳐 슬롯에 실질 텍스쳐를 지정합니다.
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, xPixels, yPixels, 0, GL_RED, GL_FLOAT, texture.data()); // 텍스쳐 이미지가 RED 단일 채널이며, float 입니다. border 는 0 만 유효합니다.}
    this->update();
}

