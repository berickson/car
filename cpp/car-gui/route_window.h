#ifndef ROUTE_WINDOW_H
#define ROUTE_WINDOW_H

#include <QDialog>
#include "../file-names.h"
#include <QGraphicsScene>
#include <QStandardItemModel>
#include "../route.h"
#include "../run-settings.h"
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

  void show_track_names();

private slots:
  void on_track_list_itemSelectionChanged();
  void on_route_list_itemSelectionChanged();

  void on_run_list_itemSelectionChanged();

  double get_k_smooth();

  void show_run_data(Route & run);

  void on_run_data_itemSelectionChanged();

  void on_run_position_slider_valueChanged(int value);

  void on_folder_picker_button_clicked();

  void on_synch_folder_button_clicked();

private:
  QPen original_route_pen,planned_route_pen,run_pen;

  QGraphicsItem * car_graphic = NULL;
  QGraphicsItem * chart_marker = NULL;
  shared_ptr<Route> original_route = NULL;
  shared_ptr<Route> planned_route = NULL;
  shared_ptr<Route> current_run = NULL;
  shared_ptr<RunSettings> current_run_settings = NULL;
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
  QLineSeries *run_series;
  QLineSeries *planned_route_series;
  QLineSeries *original_route_series;

  void hide_run_data();
  void remove_line_chart();
  void set_run_position(int selected_row);
};

#endif // ROUTE_WINDOW_H

