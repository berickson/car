#include "route_window.h"
#include "ui_route_window.h"
#include <QListView>

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
      ui->run_list->clear();
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
    ui->run_list->clear();
    auto run_names = file_names.get_run_names(get_track_name(), get_route_name());
    for(string run_name:run_names) {
      ui->run_list->addItem(run_name.c_str());
    }
  }
  if(ui->run_list->count() > 0)
    ui->run_list->item(0)->setSelected(true);

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

QString RouteWindow::get_run_name()
{
  QList<QListWidgetItem *> selection = ui->run_list->selectedItems();
  if(selection.size() == 1) {
    return (*selection.begin())->text();
  }
  return "";
}

void add_route_to_scene(QGraphicsScene & scene, Route & r,QPen &pen) {
  QPainterPath path;
  RouteNode & n = r.nodes.at(0);
  path.moveTo(n.front_x, -n.front_y);
  for(unsigned i = 1; i< r.nodes.size(); i++) {
    n = r.nodes.at(i);
    path.lineTo(n.front_x,-n.front_y);
  }
  scene.addPath(path,pen);

}

void RouteWindow::on_run_list_itemSelectionChanged()
{
  scene.clear();
  QPen route_pen,run_pen,smooth_pen;
  route_pen.setColor(Qt::green);
  route_pen.setCosmetic(true);
  route_pen.setWidth(2);

  run_pen.setColor(Qt::blue);
  run_pen.setWidth(1);
  run_pen.setCosmetic(true);

  smooth_pen.setColor(Qt::red);
  smooth_pen.setWidth(1);
  smooth_pen.setStyle(Qt::DashDotLine);
  smooth_pen.setCosmetic(true);

  try {
    if(get_run_name().size()){
      Route route,run,smooth;
      route.load_from_file(file_names.path_file_path(get_track_name(),get_route_name()));
      add_route_to_scene(scene, route, route_pen);
      run.load_from_file(file_names.path_file_path(get_track_name(),get_route_name(),get_run_name().toStdString()));
      add_route_to_scene(scene, run, run_pen);

      smooth.load_from_file(file_names.path_file_path(get_track_name(),get_route_name()));
      smooth.smooth(get_k_smooth());
      add_route_to_scene(scene, smooth, smooth_pen);

      for(auto item:scene.items()) {
        ui->graphicsView->fitInView(item , Qt::KeepAspectRatio );
      }
    }

  } catch (...) {
    scene.clear();
    scene.addText("could not load run");
  }
}


void RouteWindow::on_k_smooth_slider_valueChanged() {
  on_run_list_itemSelectionChanged();
  ui->k_smooth_value_label->setText(QString::number(get_k_smooth(),'.',2));
}

double RouteWindow::get_k_smooth() {
  return ui->k_smooth_slider->value()/100.;
}
