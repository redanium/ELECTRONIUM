#include "Incident_Toolbar.h"
#include"mainwindow.h"
#include <QToolBar>
#include <QIcon>
#include <QAction>
#include "findwidgettool.h"


Incident_Toolbar::Incident_Toolbar(QWidget *parent, int type)
    : QToolBar(parent)
{

    setWindowTitle("Barre d'outils");
    setWindowIcon(QIcon(":/files/images/tools.png"));
    setMovable(true);
    switch (type) {
    case 0:

    {
        setWindowTitle("Barre d'outils");

        setObjectName(QString("toolBar_%1").arg(type));
        //0
        addAction(QIcon(":/files/images/incident.png"), "Nouveau \n I&ncident");
        //1
        addAction(QIcon(":/files/images/electrical.png"), "&Nouveau \n Travaux Neufs");
        //2
        addAction(QIcon(":/files/images/gear.png"), "&Nouveau &Entretiens\n");
        //3
        addAction(QIcon(":/files/images/table.png"), "&Incidents\n");
        //4
        addAction(QIcon(":/files/images/closed_slot.png"),"on\n");

    }
        break;

    case 1:
    {
        setWindowTitle("Barre d'outils incident/entretien");

        setObjectName(QString("toolBar_%1").arg(type));
        //0
        addAction(QIcon(":/files/images/db.png"), "BDD\n");
        //1
        addAction(QIcon(":/files/images/print_preview.png"),"Apercu\n");
        //2
        addAction(QIcon(":/files/images/pdf.png"), "PDF\n");
        //3
        addAction(QIcon(":/files/images/print.png"),"Imprimer\n");
        //4
        addAction(QIcon(":/files/images/hd.jpg"),"Disque\n");
    }
        break;

    case 2:
    {
        setWindowTitle("Barre d'outils recherche");

        setObjectName(QString("toolBar_%1").arg(type));
        addWidget(new FindWidgetTool(this));
    }
        break;
    }
    setIconSize(QSize(20,20));


}

