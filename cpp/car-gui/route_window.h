#ifndef ROUTE_WINDOW_H
#define ROUTE_WINDOW_H

#include <QDialog>
#include "../file_names.h"
#include <QGraphicsScene>

namespace Ui {
class RouteWindow;
}

class RouteWindow : public QDialog
{
  Q_OBJECT

public:
  explicit RouteWindow(QWidget *parent = 0);
  ~RouteWindow();

private slots:
  void on_track_list_itemSelectionChanged();
  void on_route_list_itemSelectionChanged();

  void on_run_list_itemSelectionChanged();

  void on_k_smooth_slider_valueChanged();
  double get_k_smooth();

private:
  QGraphicsScene scene;
  Ui::RouteWindow *ui;
  FileNames file_names;
  string get_track_name();
  string get_route_name();
  string get_run_name();
};

#endif // ROUTE_WINDOW_H

