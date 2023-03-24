#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->layout()->setSpacing(0);
    this->centralWidget()->setContentsMargins(0, 0, 0, 0);
    this->centralWidget()->layout()->setSpacing(0);
    this->centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
    this->setContentsMargins(0, 0, 0, 0);

    connect(ui->openGLWidget, SIGNAL(constantChanged(float, float)), ui->statusbar, SLOT(setConstantValue(float, float)));
    connect(ui->openGLWidget, SIGNAL(exponentChanged(float)), ui->statusbar, SLOT(setExponentValue(float)));

    ui->openGLWidget->constantChanged(ui->openGLWidget->constant.real(), ui->openGLWidget->constant.imag());
    ui->openGLWidget->exponentChanged(ui->openGLWidget->exponent);
}

MainWindow::~MainWindow()
{
    delete ui;
}

