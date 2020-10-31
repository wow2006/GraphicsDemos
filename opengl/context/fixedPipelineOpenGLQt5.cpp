// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Qt5
#include <QDebug>
#include <QApplication>
#include <QOpenGLWidget>

class OpenGLWidgets : public QOpenGLWidget {
public:
  explicit OpenGLWidgets(QWidget* pParent = nullptr) noexcept {
    makeCurrent();
  }

  ~OpenGLWidgets() noexcept override = default;

protected:
  void initializeGL() override {
    qDebug() << "Vendor:   " << reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    qDebug() << "Renderer: " << reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    qDebug() << "Version:  " << reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << "GLSL:     " << reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
  }

  void paintGL() override {}

  void resizeGL(int width, int height) override {}

};

auto main(int argc, char *argv[]) -> int {
  QApplication application{argc, argv};

  QSurfaceFormat format;
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setMajorVersion(4);
  format.setMinorVersion(5);
  QSurfaceFormat::setDefaultFormat(format);

  OpenGLWidgets widget;
  widget.setFormat(format);
  widget.setWindowTitle("HelloWorld!");
  widget.setFixedSize(640, 480);
  widget.show();

  return QApplication::exec();
}

