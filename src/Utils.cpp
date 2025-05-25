#include "Utils.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QImage>

namespace utils {
std::optional<QImage> loadImage(const QString &filename) {
  qInfo() << "Loading image from" << filename;
  QElapsedTimer timer;
  timer.start();
  QImage img;
  if (!img.load(filename)) {
    qInfo() << "Loading image failed: " << filename;
    return {};
  }
  qInfo() << "Loaded image" << filename << "in" << timer.elapsed() << "ms";
  return img;
}
}; // namespace utils
