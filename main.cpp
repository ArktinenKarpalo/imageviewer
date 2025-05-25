#include "src/MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow mw(argc == 2 ? argv[1] : ".");
  mw.resize(800, 600);
  mw.show();

  return a.exec();
}
