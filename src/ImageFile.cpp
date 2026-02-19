#include "ImageFile.h"
#include <QDirListing>
#include <QImageReader>
#include <QtConcurrent>

bool File2::is_image() const {
  auto iformat = QImageReader::imageFormat(this->path);
  return (iformat.size() > 0);
}
File2::File2(QString path, qint64 size, QDateTime lastModified,
             QDateTime creationTime)
    : path(path), size(size), lastModified(lastModified),
      creationTime(creationTime) {};
QList<File2> File2::loadFiles(QString path) {
  QDirListing dirList(path, QDirListing::IteratorFlag::FilesOnly);
  QList<File2> files;
  for (const auto &dirEntry : dirList) {
    File2 f2 = File2(dirEntry.absoluteFilePath(), dirEntry.size(),
                     dirEntry.lastModified(QTimeZone::systemTimeZone()),
                     dirEntry.birthTime(QTimeZone::systemTimeZone()));
    files.append(f2);
  }
  return files;
}

bool File2::operator==(const File2 &x) const { return this->path == x.path; };
