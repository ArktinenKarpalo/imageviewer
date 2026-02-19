#include "ImageView.h"
#include <QImage>
#include <QRectF>
#include <QElapsedTimer>

ImageView::ImageView(QWidget *parent) : QWidget(parent) {
  fitToScreen();
  setFocusPolicy(Qt::ClickFocus);
};
void ImageView::setImage(QImage img) {
  this->img = img;
  update();
  fitToScreen();
}

void ImageView::zoom1to1() {
  if (img.isNull()) {
    return;
  }
  auto newZoom = 1;
  auto prevZoom = this->zoom;
  imageOffset +=
      ((imageOffset / prevZoom - QPointF(width(), height()) / 2 / prevZoom) *
       (newZoom - prevZoom));
  this->zoom = newZoom;
  update();
  fittedToScreen = false;
}
void ImageView::centerToScreen() {
  if (img.isNull()) {
    return;
  }
  imageOffset = QPointF(
      (width()) / 2.0 - img.width() / 2.0 * zoom / devicePixelRatio(),
      (height()) / 2.0 - img.height() / 2.0 * zoom / devicePixelRatio());
  update();
}
void ImageView::fitToScreen() {
  if (img.isNull()) {
    return;
  }
  qreal ratio =
      std::max((qreal)img.width() / width(), (qreal)img.height() / height()) /
      devicePixelRatio();
  if (ratio > 1) {
    zoom = 1 / ratio;
  } else {
    zoom = 1;
  }
  centerToScreen();
  update();
  fittedToScreen = true;
} // namespace ImageView
void ImageView::keyPressEvent(QKeyEvent *event) {
  auto prevZoom = zoom;
  switch (event->key()) {
  case Qt::Key::Key_Up:
    zoom = zoom * (1.0 + SCROLL_AMOUNT);
    imageOffset += ((imageOffset / prevZoom -
                     QPoint(width() / 2, height() / 2) / prevZoom) *
                    (zoom - prevZoom));
    update();
    fittedToScreen = false;
    break;
  case Qt::Key::Key_Down:
    zoom = zoom * (1.0 - SCROLL_AMOUNT);
    imageOffset += ((imageOffset / prevZoom -
                     QPoint(width() / 2, height() / 2) / prevZoom) *
                    (zoom - prevZoom));
    update();
    fittedToScreen = false;
    break;
  case Qt::Key::Key_R: {
    if (event->modifiers() & Qt::KeyboardModifier::ShiftModifier) {
      rotateImage(true);
    } else {
      rotateImage(false);
    }
  } break;
  default:
    QWidget::keyPressEvent(event);
  }
}
void ImageView::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    dragging = true;
    previous = event->pos();
  }
}
void ImageView::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    dragging = false;
  }
}
void ImageView::mouseMoveEvent(QMouseEvent *event) {
  if (dragging) {
    imageOffset += event->pos() - previous;
    previous = event->pos();
    update();
    fittedToScreen = false;
  }
}
void ImageView::resizeEvent(QResizeEvent *) {
  if (fittedToScreen)
    fitToScreen();
}
void ImageView::wheelEvent(QWheelEvent *event) {
  auto now = QDateTime::currentDateTime();
  if (event->pixelDelta().y() > 0 && now - previousWheelAt > SCROLL_INTERVAL) {
    auto prevZoom = zoom;
    zoom = zoom * (1.0 + SCROLL_AMOUNT);
    imageOffset += ((imageOffset / prevZoom - event->position() / prevZoom) *
                    (zoom - prevZoom));
    previousWheelAt = now;
    update();
    fittedToScreen = false;
  } else if (event->pixelDelta().y() < 0 &&
             now - previousWheelAt > SCROLL_INTERVAL) {
    auto prevZoom = zoom;
    zoom = zoom * (1.0 - SCROLL_AMOUNT);
    imageOffset += ((imageOffset / prevZoom - event->position() / prevZoom) *
                    (zoom - prevZoom));
    previousWheelAt = now;
    update();
    fittedToScreen = false;
  }
}

void ImageView::paintEvent(QPaintEvent *) {

  if (!img.isNull()) {
    imageOffset.setX(
        std::max(imageOffset.x(), ((-img.width() * zoom) + width() / 2.0)));
    imageOffset.setX(std::min(imageOffset.x(), ((width() / 2.0))));
    imageOffset.setY(
        std::max(imageOffset.y(), ((-img.height() * zoom) + height() / 2.0)));
    imageOffset.setY(std::min(imageOffset.y(), height() / 2.0));
  }

  qInfo() << "ImageView paintevent";
  QElapsedTimer timer;
  timer.start();
  QPainter painter(this);
  if (zoom < 1.0) {
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  } else {
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
  }
  // Clip into bounds
  if (!img.isNull()) {
    painter.scale(1.0 / devicePixelRatio(), 1.0 / devicePixelRatio());
    painter.translate((imageOffset * devicePixelRatio()).toPoint());
    if (zoom != 1.0) {
      qInfo() << "zoom" << zoom;
      painter.scale(zoom, zoom);
    }
    painter.drawImage(0, 0, img);
  }
  qInfo() << "Paint took" << timer.elapsed() << "ms";
}
