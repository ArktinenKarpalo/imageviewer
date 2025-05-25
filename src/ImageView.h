#pragma once
#include <QCheckBox>
#include <QChronoTimer>
#include <QDateTime>
#include <QFile>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QString>
#include <QToolBar>
#include <QWidget>
#include <QtLogging>
#include <chrono>
using namespace std::chrono_literals;

class ImageView : public QWidget {
  Q_OBJECT
signals:
  void rotateImage(bool ccw);

private:
  QImage img = QImage();
  bool fittedToScreen = false;
  bool dragging = false;
  QPointF imageOffset;
  QPointF previous;
  qreal zoom = 1.0;
  QDateTime previousWheelAt = QDateTime::currentDateTime() - 5s;
  const std::chrono::milliseconds SCROLL_INTERVAL = 40ms;
  const qreal SCROLL_AMOUNT = 0.10;

public:
  ImageView(QWidget *parent = nullptr);

  void setImage(QImage img);
  void zoom1to1();
  void centerToScreen();
  void fitToScreen();

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

  void paintEvent(QPaintEvent *event) override;
};
