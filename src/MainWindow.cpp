#include "MainWindow.h"
#include <QMenuBar>
#include <QMouseEvent>
#include <QProcess>
#include <QPushButton>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QToolBar>
#include <QWidget>
#include <QtLogging>

MainWindow::MainWindow(QString path, QWidget *parent) : QMainWindow(parent) {
  QStackedWidget *centralWidget = new QStackedWidget();
  this->setCentralWidget(centralWidget);

  imageView = new ImageView();

  auto errLbl = new QLabel("An error occurred while loading image ");
  errLbl->setWordWrap(true);
  errLbl->setAlignment(Qt::AlignCenter);
  errLbl->setStyleSheet("font-size: 24pt;");

  centralWidget->addWidget(imageView);
  centralWidget->addWidget(errLbl);

  imageManager = new ImageManager(path);
  connect(
      imageManager, &ImageManager::imageSetTo,
      [this, centralWidget, errLbl](ImageManager::CurrentImage currentImage) {
        setWindowTitle("imageviewer - " + currentImage.path);
        if (!currentImage.img.isNull()) {
          imageView->setImage(currentImage.img);
          centralWidget->setCurrentWidget(imageView);
          imageView->setFocus(Qt::FocusReason::NoFocusReason);
        } else {
          errLbl->setText("An error occurred while loading image: " +
                          currentImage.path);
          centralWidget->setCurrentWidget(errLbl);
        }
      });

  connect(imageView, &ImageView::rotateImage, imageManager,
          &ImageManager::rotate);

  toolBar = new QToolBar(nullptr);
  this->addToolBar(toolBar);

  auto editMenu = menuBar()->addMenu("Edit");
  auto sortMenu = menuBar()->addMenu("Sort by");
  QAction *rotateCW = editMenu->addAction("Rotate 90° clockwise\tR");
  connect(rotateCW, &QAction::triggered, [this]() { imageManager->rotate(); });
  QAction *rotateCCW = editMenu->addAction("Rotate 90° anticlockwise\tShift+R");
  connect(rotateCCW, &QAction::triggered,
          [this]() { imageManager->rotate(true); });

  QAction *flipHorizontal = editMenu->addAction("Flip horizontally");
  connect(flipHorizontal, &QAction::triggered,
          [this]() { imageManager->flipHorizontal(); });
  QAction *flipVertical = editMenu->addAction("Flip vertically");
  connect(flipVertical, &QAction::triggered,
          [this]() { imageManager->flipVertical(); });

  QAction *sortByFilename = sortMenu->addAction("Filename");
  connect(sortByFilename, &QAction::triggered,
          [this]() { imageManager->sortBy(ImageManager::filename); });
  QAction *sortByLastModified = sortMenu->addAction("Last modified");
  connect(sortByLastModified, &QAction::triggered,
          [this]() { imageManager->sortBy(ImageManager::modification); });
  QAction *sortBySize = sortMenu->addAction("Size");
  connect(sortBySize, &QAction::triggered,
          [this]() { imageManager->sortBy(ImageManager::size); });
  QAction *sortByRandom = sortMenu->addAction("Random");
  connect(sortByRandom, &QAction::triggered,
          [this]() { imageManager->sortBy(ImageManager::random); });

  fullscreenAction = menuBar()->addAction("Toggle &fullscreen");
  connect(fullscreenAction, &QAction::triggered,
          [this]() { toggleFullscreen(); });

  QPushButton *zoomBtn = new QPushButton("zoom to 100%", nullptr);
  zoomBtn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  toolBar->addWidget(zoomBtn);
  connect(zoomBtn, &QAbstractButton::pressed, this,
          [=]() { imageView->zoom1to1(); });

  QPushButton *fitBtn = new QPushButton("fit to screen", nullptr);
  fitBtn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  connect(fitBtn, &QAbstractButton::pressed, this,
          [=]() { imageView->fitToScreen(); });
  toolBar->addWidget(fitBtn);
}

void MainWindow::toggleFullscreen() {
  if (isFullScreen()) {
    showNormal();
    menuBar()->show();
    toolBar->show();
  } else {
    showFullScreen();
    menuBar()->hide();
    toolBar->hide();
  }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key::Key_N &&
      event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
    QProcess::startDetached(QCoreApplication::applicationFilePath(),
                            {imageManager->currentFilePath()});
    return;
  }
  switch (event->key()) {
  case Qt::Key::Key_Right:
    imageManager->nextImage();
    break;
  case Qt::Key::Key_Left:
    imageManager->previousImage();
    break;
  case Qt::Key::Key_F:
    toggleFullscreen();
    break;
  case Qt::Key::Key_Escape:
    if (isFullScreen())
      toggleFullscreen();
    break;
  default:
    QWidget::keyPressEvent(event);
  }
}
