// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// Qt5
#include <QApplication>
#include <QWidget>

class Widget : public QWidget {
public:
  explicit Widget(QWidget* pParent = nullptr) noexcept {}

  ~Widget() noexcept override = default;

};

auto main(int argc, char *argv[]) -> int {
  QApplication application{argc, argv};

  Widget widget;
  widget.setWindowTitle("HelloWorld!");
  widget.setFixedSize(640, 480);
  widget.show();

  return QApplication::exec();
}
