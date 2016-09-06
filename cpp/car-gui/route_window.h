#ifndef ROUTE_WINDOW_H
#define ROUTE_WINDOW_H

#include <QDialog>
#include "../file_names.h"
#include <QGraphicsScene>
#include <QStandardItemModel>
#include "../route.h"
#include <memory>


#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

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

  double get_k_smooth();

  void show_run_data(Route & run);

  void on_run_data_itemSelectionChanged();

  void on_run_position_slider_valueChanged(int value);

private:
  QGraphicsItem * car_graphic = NULL;
  unique_ptr<Route> current_route = NULL;
  unique_ptr<Route> current_run = NULL;
  QGraphicsScene scene;
  Ui::RouteWindow *ui;
  FileNames file_names;
  QStandardItemModel runs_model;
  string get_track_name();
  string get_route_name();
  string get_run_name();
  void clear_scene();
  void add_chart(Route & run);
  QChart * line_chart = nullptr;
  QChartView * chart_view = nullptr;

  void hide_run_data();
  void remove_line_chart();
};

#endif // ROUTE_WINDOW_H

