#include "ImageCache.h"
#include "Utils.h"
#include <QFuture>
#include <QtConcurrent>

ImageCache::ImageCache() : QObject(), loadCache(LOAD_CACHE_MAX_SIZE) {};

QFuture<ImageCache::loadFutureResult> ImageCache::startLoad(const File2 &file) {
  return QtConcurrent::run([file, this]() -> loadFutureResult {
    std::optional<QImage> img = utils::loadImage(file.path);
    if (img.has_value()) {
      QImage *cimg = new QImage(img.value());
      auto locker = QMutexLocker(&loadMutex);
      loadCache.insert(file.path, cimg, cimg->sizeInBytes());
      loadFutures.remove(file.path);
      return {file.path, img};
    } else {
      return {file.path, {}};
    }
  });
}

QFuture<ImageCache::loadFutureResult> ImageCache::loadImage(const File2 &file) {
  auto locker = QMutexLocker(&loadMutex);
  auto cached = loadCache.take(file.path);
  if (cached == 0) {
    if (loadFutures.contains(file.path)) {
      qInfo() << "cache hit(loading) for" << file.path;
      return loadFutures[file.path];
    }
    qInfo() << "cache miss for" << file.path;
    auto loadFuture = startLoad(file);
    loadFutures[file.path] = loadFuture;
    return loadFuture;
  } else {
    qInfo() << "cache hit(loaded) for" << file.path;
    loadFutureResult res = {file.path, QImage(*cached)};
    auto fut = QtFuture::makeReadyValueFuture(res);
    loadCache.insert(file.path, cached, cached->sizeInBytes());
    return fut;
  }
}

void ImageCache::preloadImage(const File2 &file) {
  auto locker = QMutexLocker(&loadMutex);
  if (!loadCache.contains(file.path) && !loadFutures.contains(file.path)) {
    auto loadFuture = QtConcurrent::run([file, this]() -> loadFutureResult {
      std::optional<QImage> img = utils::loadImage(file.path);
      if (img.has_value()) {
        QImage *cimg = new QImage(img.value());
        auto locker = QMutexLocker(&loadMutex);
        loadCache.insert(file.path, cimg, img->sizeInBytes());
        loadFutures.remove(file.path);
        return {file.path, img};
      } else {
        return {file.path, {}};
      }
    });
    loadFutures[file.path] = loadFuture;
  }
}
