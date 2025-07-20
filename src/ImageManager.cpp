#include "ImageManager.h"
#include <QFuture>
#include <QFutureWatcher>
#include <QImageReader>
#include <QLabel>
#include <QPainter>
#include <QtConcurrent>
#include <algorithm>
#include <random>

ImageManager::ImageManager(QString path) : QObject(), currentDirectory(path) {
  QDir targetDirectory(path);
  if (!targetDirectory.exists()) {
    if (!targetDirectory.cdUp()) {
      qFatal() << "Directory at" << path << "does not exist";
      exit(1);
    }
  }
  qInfo() << "Loading files from" << targetDirectory.path();
  QElapsedTimer timer;
  timer.start();
  fileList = File2::loadFiles(targetDirectory.path());
  qInfo() << "Loading" << fileList.size() << "files took" << timer.elapsed()
          << "ms";

  sortBy(SortKey::filename);

  if (fileList.size() > 0) {
    auto targetFile = std::find_if(
        fileList.begin(), fileList.end(),
        [&path](File2 const &f) -> bool { return f.path == path; });
    if (targetFile != fileList.end()) {
      setImageTo(*targetFile);
    } else {
      setImageTo(fileList[0]);
    }
  }
  connect(&loadFutureWatcher, &QFutureWatcher<QImage>::finished, [this]() {
    loading = false;
    auto result = loadFutureWatcher.result();
    if (result.second.has_value()) {
      currentImage = CurrentImage{result.first, *result.second};
      imageSetTo(currentImage);
    } else {
      auto img = new QImage();
      currentImage = CurrentImage{result.first, *img};
      imageSetTo(currentImage);
    }
  });
  QImageReader::setAllocationLimit(0);
}

void ImageManager::sortBy(SortKey key) {
  std::random_device rd;
  if (key == random) {
    std::shuffle(fileList.begin(), fileList.end(), std::mt19937(rd()));
    return;
  }
  std::sort(fileList.begin(), fileList.end(),
            [&key](const File2 &a, const File2 &b) {
              switch (key) {
              case filename:
                return a.path < b.path;
              case size:
                return a.size < b.size;
              case modification:
                return a.lastModified < b.lastModified;
              default:
                qWarning() << "Unknown sort key";
                return a.path < b.path;
              }
              return true;
            });
}

void ImageManager::previousImage() {
  if (loading)
    return;
  auto idx = fileList.indexOf(*currentFile);
  setImageTo(fileList[(idx - 1 + fileList.size()) % fileList.size()]);
  imageCache.preloadImage(
      fileList[(idx - 2 + fileList.size()) % fileList.size()]);
}
void ImageManager::nextImage() {
  if (loading)
    return;
  auto idx = fileList.indexOf(*currentFile);
  setImageTo(fileList[(idx + 1) % fileList.size()]);
  imageCache.preloadImage(fileList[(idx + 2) % fileList.size()]);
}

void ImageManager::rotate(bool ccw) {
  QImage modifiedImage(currentImage.img.height(), currentImage.img.width(),
                       QImage::Format_ARGB32_Premultiplied);
  QPainter painter(&modifiedImage);
  if (ccw) {
    painter.translate(0, currentImage.img.width());
    painter.rotate(-90);
  } else {
    painter.translate(currentImage.img.height(), 0);
    painter.rotate(90);
  }
  painter.drawImage(0, 0, currentImage.img);
  currentImage = CurrentImage{currentImage.path, modifiedImage};
  imageSetTo(currentImage);
}
void ImageManager::flipVertical() {
  QImage modifiedImage(currentImage.img.width(), currentImage.img.height(),
                       QImage::Format_ARGB32_Premultiplied);
  QPainter painter(&modifiedImage);
  painter.translate(0, currentImage.img.height());
  painter.scale(1, -1);
  painter.drawImage(0, 0, currentImage.img);
  currentImage = CurrentImage{currentImage.path, modifiedImage};
  imageSetTo(currentImage);
}
void ImageManager::flipHorizontal() {
  QImage modifiedImage(currentImage.img.width(), currentImage.img.height(),
                       QImage::Format_ARGB32_Premultiplied);
  QPainter painter(&modifiedImage);
  painter.translate(currentImage.img.width(), 0);
  painter.scale(-1, 1);
  painter.drawImage(0, 0, currentImage.img);
  currentImage = CurrentImage{currentImage.path, modifiedImage};
  imageSetTo(currentImage);
}
void ImageManager::setImageTo(const File2 &file) {
  qInfo() << "setting file to" << file.path;
  loading = true;
  currentFile = &file;
  loadFutureWatcher.setFuture(imageCache.loadImage(file));
  return;
}
QString ImageManager::currentFilePath() {
  if (currentFile == nullptr) {
    return "";
  }
  return currentFile->path;
}
