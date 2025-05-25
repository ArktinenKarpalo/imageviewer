#pragma once
#include "ImageCache.h"
#include "ImageFile.h"
#include <QCache>
#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QList>
#include <QObject>
#include <optional>
#include <utility>

class ImageManager : public QObject {
  Q_OBJECT
public:
  struct CurrentImage {
    QString path;
    QImage img;
  };

signals:
  void imageSetTo(CurrentImage currentImage);

private:
  QList<File2> fileList;
  QString currentDirectory;
  const File2 *currentFile;
  QFutureWatcher<std::pair<QString, std::optional<QImage>>> loadFutureWatcher;
  bool loading = true;
  CurrentImage currentImage;
  ImageCache imageCache;

public:
  enum SortKey {
    filename,
    size,
    modification,
    random,
  };
  ImageManager(QString path);
  void setImageTo(const File2 &file);
  void nextImage();
  void previousImage();
  void sortBy(SortKey key);
  void rotate(bool CCW = false);
  void flipHorizontal();
  void flipVertical();
  QString currentFilePath();
};
