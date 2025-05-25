#pragma once
#include "ImageManager.h"
#include "ImageView.h"
#include <QMainWindow>
#include <QString>

class MainWindow : public QMainWindow {
  Q_OBJECT
private:
  QToolBar *toolBar;
  QAction *fullscreenAction;
  ImageManager *imageManager;
  ImageView *imageView;

public:
  MainWindow(QString path = ".", QWidget *parent = nullptr);

protected:
  void toggleFullscreen();
  void keyPressEvent(QKeyEvent *event) override;
};
