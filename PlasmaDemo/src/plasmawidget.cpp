#include "plasmawidget.h"
#include <QOpenGLShader>

static const char *vshader = R"(
attribute vec2 a_pos;
varying vec2 v_uv;
void main() {
    v_uv = a_pos * 0.5 + 0.5;
    gl_Position = vec4(a_pos, 0.0, 1.0);
}
)";

// ── Effect 0: Classic Plasma ──
static const char *plasma_frag = R"(
precision mediump float;
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float v = 0.0;
    v += sin((p.x * 3.0 + t * 0.8) * 2.0);
    v += sin((p.y * 4.0 + t * 0.6) * 2.0);
    v += sin(((p.x + p.y) * 2.5 + t * 0.5) * 2.0);
    v += sin(length(p) * 5.0 - t * 1.2);

    float cx = p.x + 0.5 * sin(t * 0.3);
    float cy = p.y + 0.5 * cos(t * 0.4);
    v += sin(atan(p.y - cy, p.x - cx) * 6.0 + t * 1.5) * 0.5;

    v = v / 5.0 + 0.5;
    float hue = v, sat = 0.9, val = 0.9;

    if (u_palette == 0) {
        hue = fract(hue + t * 0.05);
        sat = 0.9;
        val = 0.8 + 0.2 * sin(v * 6.28 + t);
    } else if (u_palette == 1) {
        hue = fract(v * 2.0 + t * 0.03);
        sat = 0.7 + 0.3 * sin(v * 8.0 + t * 0.7);
    } else if (u_palette == 2) {
        hue = 0.55 + 0.15 * sin(v * 6.28 + t * 0.4);
        sat = 0.5 + 0.5 * v;
        val = 0.5 + 0.5 * (1.0 - abs(v - 0.5) * 2.0);
    } else {
        vec3 rgb = 0.5 + 0.5 * cos(v * 6.28 + vec3(0.0, 2.1, 4.2) + t * 0.3);
        gl_FragColor = vec4(rgb * 0.9, 1.0);
        return;
    }
    gl_FragColor = vec4(hsv2rgb(vec3(hue, sat, val)), 1.0);
}
)";

// ── Effect 1: Fire ──
static const char *fire_frag = R"(
precision mediump float;
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

void main() {
    vec2 uv = v_uv;
    float t = u_time;

    float v = 0.0;
    for (int i = 0; i < 5; i++) {
        float fi = float(i);
        float x = uv.x + sin(t * 0.5 + fi * 1.7) * 0.15;
        float y = uv.y + cos(t * 0.3 + fi * 2.3) * 0.1;
        float d = length(vec2(x - 0.5, y - 0.5 + fi * 0.15));
        v += (1.0 - smoothstep(0.0, 0.4, d)) * (0.5 + 0.5 * sin(t * 2.0 + fi));
    }

    float flicker = 0.8 + 0.2 * sin(t * 5.0 + uv.x * 20.0);
    v = clamp(v * 0.35 * flicker, 0.0, 1.0);
    v = pow(v, 0.6);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.1, 0.0, 0.0), vec3(1.0, 0.3, 0.0), v);
        col = mix(col, vec3(1.0, 0.9, 0.3), smoothstep(0.5, 1.0, v));
    } else if (u_palette == 1) {
        col = mix(vec3(0.0, 0.0, 0.1), vec3(0.0, 0.5, 1.0), v);
        col = mix(col, vec3(0.5, 1.0, 1.0), smoothstep(0.5, 1.0, v));
    } else if (u_palette == 2) {
        col = mix(vec3(0.0, 0.1, 0.0), vec3(0.0, 1.0, 0.3), v);
        col = mix(col, vec3(0.8, 1.0, 0.3), smoothstep(0.5, 1.0, v));
    } else {
        col = mix(vec3(0.1), vec3(0.8, 0.3, 0.8), v);
        col = mix(col, vec3(1.0, 0.8, 1.0), smoothstep(0.5, 1.0, v));
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 2: Ripples / Water ──
static const char *ripple_frag = R"(
precision mediump float;
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec2 uv = v_uv;
    float ar = u_resolution.x / u_resolution.y;
    vec2 p = (uv - 0.5) * vec2(ar, 1.0);
    float t = u_time;

    float d = length(p);
    float wave = sin(d * 8.0 - t * 2.0) * 0.5 + 0.5;
    wave += sin(d * 15.0 + t * 1.5 + p.x * 3.0) * 0.25;
    wave += sin(d * 25.0 - t * 3.0 + p.y * 4.0) * 0.125;
    wave = wave / 1.875;

    float ripple = sin(atan(p.y, p.x) * 6.0 + d * 10.0 - t * 1.5) * 0.3 + 0.5;

    float v = wave * 0.7 + ripple * 0.3;
    v = clamp(v, 0.0, 1.0);

    vec3 col;
    if (u_palette == 0) {
        col = mix(vec3(0.0, 0.1, 0.2), vec3(0.0, 0.5, 0.8), v);
        col = mix(col, vec3(0.5, 0.8, 1.0), smoothstep(0.6, 1.0, v));
    } else if (u_palette == 1) {
        col = hsv2rgb(vec3(fract(0.6 + v * 0.3 + t * 0.02), 0.7, 0.5 + v * 0.5));
    } else if (u_palette == 2) {
        col = mix(vec3(0.05, 0.05, 0.05), vec3(1.0, 0.6, 0.0), v);
        col = mix(col, vec3(1.0, 1.0, 0.5), smoothstep(0.5, 1.0, v));
    } else {
        col = 0.5 + 0.5 * cos(v * 6.28 + vec3(0.0, 1.0, 2.0) + t * 0.2);
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Effect 3: Mandelbrot ──
static const char *mandel_frag = R"(
precision mediump float;
varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;
uniform int u_palette;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float ar = u_resolution.x / u_resolution.y;
    vec2 uv = (v_uv - 0.5) * vec2(ar, 1.0);

    float zoom = 1.0 + 0.5 * sin(u_time * 0.1);
    float rot = u_time * 0.05;
    float cs = cos(rot), sn = sin(rot);
    vec2 z = vec2(uv.x * cs - uv.y * sn, uv.x * sn + uv.y * cs) * zoom;
    z += vec2(-0.5, 0.0);

    vec2 c = z;
    float it = 0.0;
    for (int i = 0; i < 128; i++) {
        if (dot(z, z) > 4.0) break;
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
        it += 1.0;
    }
    float v = it / 128.0;

    vec3 col;
    if (u_palette == 0) {
        col = hsv2rgb(vec3(fract(v * 3.0 + u_time * 0.02), 0.8, v * 0.5 + 0.5));
    } else if (u_palette == 1) {
        col = 0.5 + 0.5 * cos(v * 4.0 + vec3(0.0, 1.0, 2.0));
    } else if (u_palette == 2) {
        col = mix(vec3(0.05, 0.0, 0.1), vec3(1.0, 0.5, 0.0), v);
    } else {
        col = mix(vec3(0.0, 0.1, 0.0), vec3(0.0, 1.0, 0.5), v);
        col = mix(col, vec3(1.0, 1.0, 0.5), smoothstep(0.6, 1.0, v));
    }
    gl_FragColor = vec4(col, 1.0);
}
)";

// ── Implementation ──

PlasmaWidget::PlasmaWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMinimumSize(400, 300);
    resize(800, 600);
    setFocusPolicy(Qt::StrongFocus);

    connect(&m_timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
    m_timer.setInterval(16);
    m_elapsed.start();
    m_fpsTimer.start();

    setupShaders();
}

PlasmaWidget::~PlasmaWidget()
{
    makeCurrent();
    if (m_program)
        glDeleteProgram(m_program);
    doneCurrent();
}

void PlasmaWidget::setupShaders()
{
    m_effects = {
        { "Plasma",     vshader, plasma_frag },
        { "Fire",       vshader, fire_frag   },
        { "Ripples",    vshader, ripple_frag },
        { "Mandelbrot", vshader, mandel_frag },
    };
}

void PlasmaWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
    compileCurrentEffect();
    m_timer.start();
}

void PlasmaWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void PlasmaWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_program)
        return;

    glUseProgram(m_program);

    float time = m_elapsed.elapsed() / 1000.0f;
    if (m_paused)
        time = m_elapsed.elapsed() / 1000.0f;

    glUniform1f(u_time, time);
    glUniform2f(u_resolution, (float)width(), (float)height());
    glUniform1i(u_palette, m_palette);

    renderQuad();

    glUseProgram(0);

    // Overlay
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        paintOverlay(p);
    }

    // FPS counter (once per second)
    m_frameCount++;
    if (m_fpsTimer.elapsed() >= 1000) {
        m_fps = m_frameCount;
        m_frameCount = 0;
        m_fpsTimer.restart();
    }
}

void PlasmaWidget::paintOverlay(QPainter &p)
{
    QFont font("DejaVu Sans", 12);
    p.setFont(font);

    // Background strip
    QRect strip(0, 0, width(), 26);
    p.fillRect(strip, QColor(0, 0, 0, 160));

    // Text
    p.setPen(Qt::white);
    QString text = QString("Effect: %1  |  Palette: %2  |  FPS: %3")
        .arg(m_effects[m_effect].name)
        .arg(paletteName())
        .arg(m_fps);

    if (m_paused)
        text += "  |  PAUSED";

    p.drawText(strip.adjusted(8, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, text);

    // Help text at bottom
    QFont small("DejaVu Sans", 10);
    p.setFont(small);
    QString help = "Click: next effect  |  ←→: palette  |  ↑↓: effect  |  Space: pause";
    QRect helpRect(0, height() - 22, width(), 22);
    p.fillRect(helpRect, QColor(0, 0, 0, 120));
    p.drawText(helpRect.adjusted(8, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, help);
}

void PlasmaWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    makeCurrent();
    if (m_program)
        glDeleteProgram(m_program);
    m_program = 0;
    m_effect = (m_effect + 1) % m_effects.size();
    compileCurrentEffect();
    doneCurrent();
}

void PlasmaWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        makeCurrent();
        m_palette = (m_palette + 3) % 4;
        glUseProgram(m_program);
        glUniform1i(u_palette, m_palette);
        glUseProgram(0);
        doneCurrent();
        break;
    case Qt::Key_Right:
        makeCurrent();
        m_palette = (m_palette + 1) % 4;
        glUseProgram(m_program);
        glUniform1i(u_palette, m_palette);
        glUseProgram(0);
        doneCurrent();
        break;
    case Qt::Key_Up:
        makeCurrent();
        if (m_program)
            glDeleteProgram(m_program);
        m_program = 0;
        m_effect = (m_effect + 1) % m_effects.size();
        compileCurrentEffect();
        doneCurrent();
        break;
    case Qt::Key_Down:
        makeCurrent();
        if (m_program)
            glDeleteProgram(m_program);
        m_program = 0;
        m_effect = (m_effect + m_effects.size() - 1) % m_effects.size();
        compileCurrentEffect();
        doneCurrent();
        break;
    case Qt::Key_Space:
        m_paused = !m_paused;
        if (!m_paused)
            m_elapsed.restart();
        break;
    default:
        QOpenGLWidget::keyPressEvent(event);
    }
}

void PlasmaWidget::compileCurrentEffect()
{
    const auto &e = m_effects[m_effect];
    GLuint vs = compileShader(GL_VERTEX_SHADER, e.vert);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, e.frag);

    if (!vs || !fs)
        return;

    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glBindAttribLocation(m_program, 0, "a_pos");
    glLinkProgram(m_program);

    GLint linked;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char buf[1024];
        glGetProgramInfoLog(m_program, sizeof(buf), nullptr, buf);
        qWarning("Link error [%s]: %s", e.name, buf);
        glDeleteProgram(m_program);
        m_program = 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    u_time = glGetUniformLocation(m_program, "u_time");
    u_resolution = glGetUniformLocation(m_program, "u_resolution");
    u_palette = glGetUniformLocation(m_program, "u_palette");
}

GLuint PlasmaWidget::compileShader(GLenum type, const char *source)
{
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &source, nullptr);
    glCompileShader(sh);

    GLint compiled;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        char buf[1024];
        glGetShaderInfoLog(sh, sizeof(buf), nullptr, buf);
        qWarning("Shader compile (%s): %s",
                 type == GL_VERTEX_SHADER ? "vertex" : "fragment", buf);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

void PlasmaWidget::renderQuad()
{
    static const float verts[] = {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1,
    };
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
}

QString PlasmaWidget::paletteName() const
{
    switch (m_palette) {
    case 0: return "Rainbow";
    case 1: return "Warm";
    case 2: return "Cool";
    default: return "RGB";
    }
}
