#include "route_window.h"
#include "ui_route_window.h"
#include <QListView>
#include <QStandardItemModel>
#include "../run_settings.h"

#include <fstream>
#include <string>
#include "../system.h"
#include "../file_names.h"
#include "../route.h"

using namespace std;

RouteWindow::RouteWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::RouteWindow)
{
  ui->setupUi(this);
  ui->graphicsView->setScene(&scene);
  scene.addText("Hello, world!");


  auto track_names =  file_names.get_track_names();
  for(string track_name:track_names) {
    ui->track_list->addItem(track_name.c_str());
  }
  if(ui->track_list->count() > 0)
    ui->track_list->item(0)->setSelected(true);

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
}

void RouteWindow::on_route_list_itemSelectionChanged()
{
  QList<QListWidgetItem *> selection = ui->route_list->selectedItems();
  if(selection.size()==1) {
    //ui->run_list->clear();
    ui->run_list->setRowCount(0);
    ui->run_list->setColumnCount(3);
    ui->run_list->setSortingEnabled(true);
    QStringList labels;
    labels << "run" << "a" << "v";
    ui->run_list->setHorizontalHeaderLabels(labels);
    ui->run_list->verticalHeader()->setVisible(false);
    auto run_names = file_names.get_run_names(get_track_name(), get_route_name());
    for(string run_name:run_names) {
      run_name = run_name ;
      int row = ui->run_list->rowCount();

      ui->run_list->insertRow(row);
      RunSettings run_settings;
      try {
        run_settings.load_from_file(file_names.config_file_path(get_track_name(),get_route_name(),run_name));
      } catch (...) {}

      ui->run_list->setItem(row,0,new QTableWidgetItem(run_name.c_str()));
      ui->run_list->setItem(row,1,new QTableWidgetItem(QString::number(run_settings.max_a)));
      ui->run_list->setItem(row,2,new QTableWidgetItem(QString::number(run_settings.max_v)));

    }
  }
  if(ui->run_list->rowCount() > 0) {
    ui->run_list->selectRow(0);
  }

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

void RouteWindow::on_run_list_itemSelectionChanged()
{
  clear_scene();
  QPen route_pen,run_pen;
  route_pen.setColor(Qt::green);
  route_pen.setCosmetic(true);
  route_pen.setWidth(2);

  run_pen.setColor(Qt::blue);
  run_pen.setWidth(1);
  run_pen.setCosmetic(true);

  string run_path;
  current_run = NULL;

  try {
    Route route;
    route.load_from_file(file_names.path_file_path(get_track_name(),get_route_name()));
    add_route_to_scene(scene, route, route_pen);


    QModelIndexList selected_list = ui->run_list->selectionModel()->selectedRows();
    ui->run_data->clear();
    for( int i=0; i<selected_list.count(); i++) {
      int selected_row = selected_list.at(i).row();
      string run_name = ui->run_list->item(selected_row,0)->text().toStdString();
      try {
        unique_ptr<Route> r(new Route());
        current_run = std::move(r);
        string run_path = file_names.path_file_path(get_track_name(),get_route_name(),run_name);
        current_run->load_from_file(run_path.c_str());
        if(i==0) {
          show_run_data(*current_run);
        }
        add_route_to_scene(scene, *current_run, run_pen);
        ui->run_position_slider->setMaximum(current_run->nodes.size());
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

void RouteWindow::show_run_data(Route &run)
{
  QTableWidget &t = *(ui->run_data);
  t.clear();
  QStringList labels;
  labels << "secs" << "str" << "esc" << "front_x" << "front_y" << "heading" << "velocity";
  t.setColumnCount(labels.size());
  t.setRowCount(run.nodes.size());
  t.horizontalHeader()->setVisible(true);

  t.setHorizontalHeaderLabels(labels);
  t.verticalHeader()->setVisible(false);
  for(unsigned int i = 0; i < run.nodes.size(); i++) {
    RouteNode & node = run.nodes[i];
    int col=-1;

    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.secs)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.str)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.esc)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.front_x)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.front_y)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.heading)));
    t.setItem(i,++col,new QTableWidgetItem(QString::number(node.velocity)));

  }
  t.resizeColumnsToContents();

}

void RouteWindow::on_run_data_itemSelectionChanged()
{
  QModelIndexList selected_list = ui->run_data->selectionModel()->selectedRows();
  if(selected_list.size()==1) {
    int selected_row = selected_list.at(0).row();
    ui->run_position_slider->setValue(selected_row);

    QPen car_pen;
    car_pen.setColor(Qt::black);
    car_pen.setCosmetic(true);
    car_pen.setWidth(2);

    RouteNode & node = current_run->nodes[selected_row];


    if(car_graphic){
      scene.removeItem(car_graphic);
    }
    double car_width = .5;
    car_graphic = (QGraphicsItem*) scene.addEllipse(
          node.front_x-car_width/2,
          -node.front_y-car_width/2,car_width,car_width,car_pen);

  }

}

void RouteWindow::on_run_position_slider_valueChanged(int value)
{
  ui->run_data->selectRow(value);
}
