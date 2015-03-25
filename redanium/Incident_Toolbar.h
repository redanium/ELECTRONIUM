#ifndef INCIDENT_TOOLBAR_H
#define INCIDENT_TOOLBAR_H

#include<QToolBar>
class Incident_Toolbar : public QToolBar{
  Q_OBJECT

  public:
     Incident_Toolbar (QWidget *parent = 0,int type=0);

};

#endif // INCIDENT_TOOLBAR_H
