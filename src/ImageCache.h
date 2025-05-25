#pragma once
#include "ImageFile.h"
#include <QCache>
#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <utility>

class ImageCache : public QObject {
  Q_OBJECT
private:
  static const qsizetype LOAD_CACHE_MAX_SIZE = 256 * 1000 * 1000;
  typedef std::pair<QString, std::optional<QImage>> loadFutureResult;

  QCache<QString, QImage> loadCache;
  QMap<QString, QFuture<loadFutureResult>> loadFutures;
  QMutex loadMutex; // Lock if accessing loadCache and loadFutures
  QFutureWatcher<loadFutureResult> loadFutureWatcher;

  QFuture<loadFutureResult> startLoad(const File2 &file);

public:
  ImageCache();
  QFuture<loadFutureResult> loadImage(const File2 &file);
  void preloadImage(const File2 &file);
};
