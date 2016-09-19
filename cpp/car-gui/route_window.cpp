#include "route_window.h"
#include "ui_route_window.h"
#include <QListView>
#include <QStandardItemModel>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "../run_settings.h"

#include <fstream>
#include <string>
#include "../system.h"
#include "../file_names.h"
#include "../route.h"

using namespace std;
QT_CHARTS_USE_NAMESPACE
RouteWindow::RouteWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::RouteWindow)
{
  original_route_pen.setColor(Qt::black);
  original_route_pen.setCosmetic(true);
  original_route_pen.setWidth(4);

  planned_route_pen.setColor(Qt::darkGray);
  planned_route_pen.setCosmetic(true);
  planned_route_pen.setWidth(2);

  run_pen.setColor(Qt::blue);
  run_pen.setWidth(1);
  run_pen.setCosmetic(true);

  ui->setupUi(this);
  ui->graphicsView->setScene(&scene);
  scene.addText("Hello, world!");


  auto track_names =  file_names.get_track_names();
  for(string track_name:track_names) {
    ui->track_list->addItem(track_name.c_str());
  }
  if(ui->track_list->count() > 0)
    ui->track_list->item(0)->setSelected(true);
  ui->track_list->adjustSize();

}

RouteWindow::~RouteWindow()
{
  delete ui;
}

void RouteWindow::on_track_list_itemSelectionChanged()
{
    string track_name = get_track_name();
    if(track_name.size() > 0) {
      ui->route_list->clear();
      auto route_names = file_names.get_route_names(track_name);
      for(string route_name:route_names) {
        ui->route_list->addItem(route_name.c_str());
      }
    }
    if(ui->route_list->count() > 0)
      ui->route_list->item(0)->setSelected(true);
    ui->route_list->adjustSize();
}

void RouteWindow::on_route_list_itemSelectionChanged()
{
  QList<QListWidgetItem *> selection = ui->route_list->selectedItems();
  if(selection.size()==1) {
    //ui->run_list->clear();
    ui->run_list->setRowCount(0);
    ui->run_list->setSortingEnabled(true);
    QStringList labels;
    labels
        << "run"
        << "a"
        << "v"
        << "prune_max"
        << "prune_tol"
        << "t_ahead"
        << "k_p"
        << "k_i"
        << "k_d"
        << "v_k_p"
        << "v_k_i"
        << "v_k_d"
        << "k_smooth"
        << "capture_video"
        << "slip_rate"
        << "slip_slop";

    ui->run_list->setColumnCount(labels.size());

    ui->run_list->setHorizontalHeaderLabels(labels);
    ui->run_list->verticalHeader()->setVisible(false);
    auto run_names = file_names.get_run_names(get_track_name(), get_route_name());
    for(string run_name:run_names) {
      run_name = run_name ;
      int row = ui->run_list->rowCount();

      ui->run_list->insertRow(row);
      {
        unique_ptr<RunSettings> rs(new RunSettings);
        current_run_settings = std::move(rs);
      }

      RunSettings & run_settings = *current_run_settings;
      try {
        run_settings.load_from_file(file_names.config_file_path(get_track_name(),get_route_name(),run_name));
      } catch (...) {}
      int i=0;

      ui->run_list->setItem(row,i++,new QTableWidgetItem(run_name.c_str()));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.max_a)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.max_v)));

      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.prune_max)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.prune_tolerance)));

      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.t_ahead)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.steering_k_p)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.steering_k_i)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.steering_k_d)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.v_k_p)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.v_k_i)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.v_k_d)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.k_smooth)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.capture_video)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.slip_rate)));
      ui->run_list->setItem(row,i++,new QTableWidgetItem(QString::number(run_settings.slip_slop?1:0)));

    }
    ui->run_list->resizeColumnsToContents();
  }
  if(ui->run_list->rowCount() > 0) {
    ui->run_list->selectRow(0);
  }
  //ui->run_list->adjustSize();

}

string RouteWindow::get_track_name()
{
  QList<QListWidgetItem *> selection = ui->track_list->selectedItems();
  if(selection.size() == 1) {
    return (*selection.begin())->text().toStdString();
  }
  return "";
}

string RouteWindow::get_route_name()
{
  QList<QListWidgetItem *> selection = ui->route_list->selectedItems();
  if(selection.size() == 1) {
    return (*selection.begin())->text().toStdString();
  }
  return "";
}

string RouteWindow::get_run_name()
{
  auto selection = ui->run_list->selectionModel()->selectedIndexes();
  if(selection.size() == 1) {
    return selection.at(0).data().toString().toStdString();
//    return runs_model.item(row)->text();
  }
  return "";
}

void add_route_to_scene(QGraphicsScene & scene, Route & r,QPen &pen) {
  QPainterPath path;
  RouteNode * n = & r.nodes.at(0);
  path.moveTo(n->front_x, -n->front_y);
  for(unsigned i = 1; i< r.nodes.size(); i++) {
    n = &r.nodes.at(i);
    path.lineTo(n->front_x,-n->front_y);
  }
  scene.addPath(path,pen);

}

void RouteWindow::clear_scene()
{
  car_graphic = NULL;
  scene.clear();
}

void RouteWindow::add_chart(Route & run)
{
  run_series = new QLineSeries(); // deleted by qt
  run_series->setPen(run_pen);
  original_route_series = new QLineSeries();
  original_route_series->setPen(original_route_pen);
  planned_route_series = NULL;
  if(planned_route) {
    planned_route_series = new QLineSeries();
    planned_route->reset_position_to_start();
    planned_route_series->setPen(planned_route_pen);
  }
  original_route->reset_position_to_start();


  for(RouteNode & node: run.nodes) {
    if(planned_route) {
      planned_route->set_position(node.get_front_position(), node.get_front_position(), node.velocity);
      planned_route_series->append(node.secs, planned_route->get_velocity());
    }

    original_route->set_position(node.get_front_position(), node.get_front_position(), node.velocity);
    original_route_series->append(node.secs, original_route->get_velocity());

    run_series->append(node.secs, node.velocity);
  }



  if(line_chart!=nullptr) {
    delete line_chart;
    line_chart = nullptr;
  }

  line_chart= new QChart();
  line_chart->legend()->hide();
  line_chart->addSeries(original_route_series);
  if(planned_route_series) {
    line_chart->addSeries(planned_route_series);
  }
  line_chart->addSeries(run_series);
  line_chart->createDefaultAxes();
  line_chart->setTitle("Run velocity vs. time");
  if(chart_view != nullptr) {
    delete chart_view;
    chart_view = nullptr;
  }
  chart_view = new QChartView(line_chart);

  QPen car_pen;
  car_pen.setColor(Qt::red);
  car_pen.setCosmetic(true);
  car_pen.setWidth(2);
  chart_marker = chart_view->scene()->addEllipse(0,0,10,10,car_pen);


  chart_view->setRenderHint(QPainter::Antialiasing);
  ui->splitter_3->addWidget(chart_view);
}

void RouteWindow::on_run_list_itemSelectionChanged()
{
  clear_scene();

  string run_path;
  current_run = NULL;

  try {
    std::unique_ptr<Route> r (new Route());
    original_route = std::move(r);



    original_route->load_from_file(file_names.path_file_path(get_track_name(),get_route_name()));
    add_route_to_scene(scene, *original_route, original_route_pen);


    QModelIndexList selected_list = ui->run_list->selectionModel()->selectedRows();
    ui->run_data->clear();
    hide_run_data();
    for( int i=0; i<selected_list.count(); i++) {
      int selected_row = selected_list.at(i).row();
      string run_name = ui->run_list->item(selected_row,0)->text().toStdString();
      try {
        // load planned route
        {
          std::string file_path;
          planned_route = NULL;
          file_path = file_names.planned_path_file_path(get_track_name(),get_route_name(),run_name);
          if(file_exists(file_path)){
            planned_route = std::make_shared<Route>();
            planned_route->load_from_file(file_path);
            add_route_to_scene(scene, *planned_route, planned_route_pen);
          }
        }

        unique_ptr<Route> r(new Route());
        current_run = std::move(r);
        string run_path = file_names.path_file_path(get_track_name(),get_route_name(),run_name);
        current_run->load_from_file(run_path.c_str());
        if(i==0) {
          show_run_data(*current_run);
        }
        add_route_to_scene(scene, *current_run, run_pen);
        ui->run_position_slider->setMaximum(current_run->nodes.size()-1);
      }
      catch (...){
        string m = "could not load run" + run_name;
        scene.addText(m.c_str());
      }
    }


    for(auto item:scene.items()) {
      ui->graphicsView->fitInView(item , Qt::KeepAspectRatio );
    }

  } catch (...) {
    //scene.clear();

  }
}



double RouteWindow::get_k_smooth() {
  return ui->run_position_slider->value()/100.;
}

void RouteWindow::hide_run_data() {
  remove_line_chart();
  ui->run_data->clear();
}

void RouteWindow::remove_line_chart() {
  if(line_chart!=nullptr) {
    delete line_chart;
    line_chart = nullptr;
  }

  if(chart_view != nullptr) {
    delete chart_view;
    chart_view = nullptr;
  }

}

void RouteWindow::show_run_data(Route &run)
{

  QTableWidget &t = *(ui->run_data);
  t.clear();
  QStringList labels;
  labels << "secs" << "str" << "esc" << "front_x" << "front_y" << "heading" << "velocity" << "crv";
  t.setColumnCount(labels.size());
  t.setRowCount(run.nodes.size());
  t.horizontalHeader()->setVisible(true);

  t.setHorizontalHeaderLabels(labels);
  t.verticalHeader()->setVisible(false);
  for(unsigned int i = 0; i < run.nodes.size(); i++) {
    const RouteNode & node = run.nodes[i];
    int col=-1;

    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.secs)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.str)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.esc)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.front_x)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.front_y)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.heading)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.velocity)));
    if(i>0) {
      const RouteNode & prev = run.nodes[i-1];
      double ds = distance(prev.get_front_position(),node.get_front_position());
      Angle dtheta = (Angle::degrees(node.heading) - Angle::degrees(prev.heading));
      dtheta.standardize();
      double curvature = NAN;
      if(ds > 0 ) {
        curvature = dtheta.degrees()/ds;
      }
      t.setItem(i,++col,new QTableWidgetItem(QString::number(curvature)));
    }

  }
  t.resizeColumnsToContents();

  add_chart(run);

  if(run.nodes.size() > 0) {
    t.clearSelection();
    t.selectRow(0);
  }

}

void RouteWindow::on_run_data_itemSelectionChanged()
{
  QModelIndexList selected_list = ui->run_data->selectionModel()->selectedRows();
  if(selected_list.size()==1) {
    int selected_row = selected_list.at(0).row();
    set_run_position(selected_row);
  }

}

void RouteWindow::set_run_position(int selected_row) {
  QPen car_pen;
  car_pen.setColor(Qt::red);
  car_pen.setCosmetic(true);
  car_pen.setWidth(2);
  if(chart_marker) {
    chart_view->scene()->removeItem(chart_marker);
  }
  chart_marker = chart_view->scene()->addEllipse(0,0,10,10,car_pen);
  QPointF pos = line_chart->mapToPosition(run_series->at(selected_row));
  auto x =3;
  pos.setX(pos.x()-chart_marker->boundingRect().width()/2+x);
  pos.setY(pos.y()-chart_marker->boundingRect().height()/2+x);

  chart_marker->setPos(pos);
  ui->run_position_slider->setValue(selected_row);


  RouteNode & node = current_run->nodes[selected_row];


  if(car_graphic){
    scene.removeItem(car_graphic);
    car_graphic = NULL;
  }

  QGraphicsView * v = scene.views().at(0);
  QPointF p1 = v->mapToScene(QPoint(0,0));
  QPointF p2 = v->mapToScene(QPoint(1,1));
  double scale = p2.x()-p1.x();

  double car_width = 10 * scale;

  car_graphic = (QGraphicsItem*) scene.addEllipse(
        node.front_x-car_width/2,
        -node.front_y-car_width/2,car_width,car_width,car_pen);
}

void RouteWindow::on_run_position_slider_valueChanged(int value)
{
  set_run_position(value);
}
