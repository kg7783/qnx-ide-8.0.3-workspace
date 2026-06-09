#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVector>
#include <QPainter>
#include <QString>

struct ShaderPair {
    const char *name;
    const char *vert;
    const char *frag;
};

class PlasmaWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit PlasmaWidget(QWidget *parent = nullptr);
    ~PlasmaWidget() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void compileCurrentEffect();
    GLuint compileShader(GLenum type, const char *source);
    void renderQuad();
    void setupShaders();
    void paintOverlay(QPainter &p);
    QString paletteName() const;

    QTimer m_timer;
    QElapsedTimer m_elapsed;

    int m_effect = 0;
    int m_palette = 0;
    bool m_paused = false;

    GLuint m_program = 0;
    int u_time = -1;
    int u_resolution = -1;
    int u_palette = -1;

    int m_frameCount = 0;
    int m_fps = 0;
    QElapsedTimer m_fpsTimer;

    QVector<ShaderPair> m_effects;
};
