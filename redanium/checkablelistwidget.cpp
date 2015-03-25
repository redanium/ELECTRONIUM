#include "checkablelistwidget.h"
#include "ui_checkablelistwidget.h"

checkableListWidget::checkableListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::checkableListWidget)
{

    ui->setupUi(this);
    m_menu= new QMenu(this);
    ui->toolButton_->setMenu(m_menu);
    //    ui->toolButton_->setPopupMode(QToolButton::MenuButtonPopup);

    signalMapper = new QSignalMapper(this);
}

checkableListWidget::~checkableListWidget()
{
    delete ui;
}

void checkableListWidget::addItem(QString itemText)
{
    QHBoxLayout *itemLayout=new QHBoxLayout;

    QWidget *itemWidget= new QWidget;
    itemWidget->setStyleSheet("QWidget{background-color: rgb(255, 170, 0);border-radius:4px;}");
    itemWidget->setObjectName(itemText);
    itemWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    QLabel *itemLabel= new QLabel(itemText);

    QToolButton *itemCloseBtn= new QToolButton;
    itemCloseBtn->setIcon(QIcon(":/files/images/close.png"));
    itemCloseBtn->setIconSize(QSize(12,12));

    itemLayout->addWidget(itemLabel);
    itemLayout->addWidget(itemCloseBtn);
    itemLayout->setSpacing(2);

    itemWidget->setLayout(itemLayout);
    itemWidget->setToolTip(itemText);

    ui->horizontalLayout_container->addWidget(itemWidget);

    connect(itemCloseBtn,SIGNAL(clicked()),itemWidget,SLOT(deleteLater()));
    connect(itemWidget,SIGNAL(destroyed(QObject*)),this,SIGNAL(deletingItem(QObject*)));


    emit addingItem(itemText);
    qDebug()<< parent()->objectName();

}

void checkableListWidget::setModel(QStandardItemModel *model)
{
    QList<QAction*> actionsList;
    m_model=model;
    signalMapper =new QSignalMapper(this);

    for (int i = 0; i < m_model->rowCount(); ++i) {
        QAction *action= new QAction(m_menu);
        action->setText(m_model->data(m_model->index(i,0)).toString());
        action->setIcon(QIcon(":/files/images/dummy_person.png"));

        actionsList.append(action);
        signalMapper->setMapping(action,m_model->data(m_model->index(i,0)).toString() );

        connect(action,SIGNAL(triggered()),this->signalMapper,SLOT(map()));


    }
    connect(signalMapper, SIGNAL(mapped(QString)),this, SLOT(addItem(QString)));
    //connect(this, SIGNAL(addingItem(QString)),this, SLOT(addItem(QString)));

    m_menu->clear();

    m_menu->addActions(actionsList);
}

void checkableListWidget::on_toolButton_clicked()
{
}

void checkableListWidget::insertItem(bool checked)
{
    //    foreach (QAction *var, m_menu->actions()) {
    //        if(var->isChecked()){
    //            if(ui->horizontalLayout_container->findChild<QWidget*>(var->text())==0)
    //         addItem(var->text());
    //        }
    //    }

    QList<QObject *> widgets=ui->horizontalLayout_container->children();
    qDebug()<< "Children List Size" << ui->horizontalLayout_container->children().size();
    foreach (QObject* wid, widgets) {
        qDebug()<<  wid->objectName();
    }
}

void checkableListWidget::on_toolButton__clicked()
{
    ui->toolButton_->menu()->show();

}
