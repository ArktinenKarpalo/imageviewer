#pragma once
#include <QDateTime>
#include <QFile>
#include <QList>
#include <QTimeZone>

class File2 {
public:
  QString path;
  qint64 size;
  QDateTime lastModified;
  QDateTime creationTime;

public:
  File2(QString path, qint64 size, QDateTime lastModified,
        QDateTime creationTime);
  static QList<File2> loadFiles(QString path);
  bool operator==(const File2 &x) const;
};
