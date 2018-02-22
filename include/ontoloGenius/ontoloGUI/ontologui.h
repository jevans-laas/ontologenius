#ifndef ONTOLOGUI_H
#define ONTOLOGUI_H

#include <QMainWindow>
#include "include/ontoloGenius/ontoloGUI/QCheckBoxExtended.h"

#include "ros/ros.h"
#include <vector>
#include <string>

namespace Ui {
class ontoloGUI;
}

class ontoloGUI : public QMainWindow
{
    Q_OBJECT

public:
  explicit ontoloGUI(QWidget *parent = 0);
  ~ontoloGUI();

  void init(ros::NodeHandle* n) {n_ = n; }
  void wait();
  void start();
  void loadArguers();

private:
  Ui::ontoloGUI *ui;
  ros::NodeHandle* n_;
  std::vector<std::string> arguers_names_;
  std::vector<std::string> arguers_description_;

  void displayUnClosed();
  void constructArguersCheckBoxs();
  size_t getArguerIndex(QCheckBoxExtended* box);
  std::string getArguerDescription(std::string box);

public slots:
  void ClasshoverEnterSlot();
  void ClasshoverLeaveSlot();
  void PropertyhoverEnterSlot();
  void PropertyhoverLeaveSlot();
  void IndividualhoverEnterSlot();
  void IndividualhoverLeaveSlot();
  void classClickedSlot();
  void propertyClickedSlot();
  void individualClickedSlot();
  void closeOntologySlot();
  void ArguerClickedSlot(int);
  void ArguerhoverEnterSlot();
  void ArguerhoverLeaveSlot();
};

#endif // ONTOLOGUI_H
