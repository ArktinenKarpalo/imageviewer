#include "ImageFile.h"
#include <QDirListing>
#include <QImageReader>

File2::File2(QString path, qint64 size, QDateTime lastModified,
             QDateTime creationTime)
    : path(path), size(size), lastModified(lastModified),
      creationTime(creationTime) {};
QList<File2> File2::loadFiles(QString path) {
  QDirListing dirList(path, QDirListing::IteratorFlag::FilesOnly);
  QList<File2> files;
  for (const auto &dirEntry : dirList) {
    QFile f(dirEntry.filePath());
    auto iformat = QImageReader::imageFormat(dirEntry.filePath());
    if (iformat.size() > 0) {
      File2 f = File2(dirEntry.absoluteFilePath(), dirEntry.size(),
                      dirEntry.lastModified(QTimeZone::systemTimeZone()),
                      dirEntry.birthTime(QTimeZone::systemTimeZone()));
      files.append(f);
    }
  }
  return files;
}

bool File2::operator==(const File2 &x) const { return this->path == x.path; };
