#ifndef INCIDENT_H
#define INCIDENT_H

// classe incident pr stocker en BDD
#include<QFile>
#include<QObject>
#include<QString>
#include<QStringList>
#include"Incident_Travaux.h"
#include<QGeoCoordinate>
#include<QWebView>

using namespace std;
class Incident_obj :public QObject{

    //think in the future about an abstract class thal will be inherited by "incident" and "travaux neufs"
    Q_OBJECT
    Q_PROPERTY(QString i_coordinate READ getI_coordinate WRITE setI_coordinate)
public slots:
    void addAgent(QString agentName);
    void removeAgent(QObject *agentObj);
    void QtyFournituresChanged(QStandardItem *item);


public:
    explicit Incident_obj(QObject *parent = 0);
    ~Incident_obj();
    enum incident_TYPE {AERIEN=306,SOUSTERRAIN=406,POSTE=206}; // imputaion
    QMap<QString,int> i_fournitures;

private :
QWebView *i_webView;
    int i_NumID; //auto_increment + intern
    QString i_Objet;
    QString i_Direction;
    QString i_Agence ;
    QStringList i_Agent;
    QString i_Date;
    QString i_Date_Facture;//date facture changeable
    QString i_Date_Cmd;
    int i_Commande; // Nxx/DTE/yyyy
    QStringList i_heure_debut_fin;//majoration 50% sur MEO , majoration travail de nuit : 50% ##jours feries
    int i_Convention;
    int i_numero_facture;
    bool majoration_Fifty_Night;
    bool majoration_Fifty_DayOff;
    QStringList i_Presence;

    Incident_Travaux *i_Travaux;
    double i_Montant;
    int imputation;//"IMPUTATION 932 1C1 imputation" == type visible facture
    QStringList i_notes;
    //private slots:
    //   void presenceChanged();
    QString i_coordinate;//vector for routes
    int Type;


public:
    void i_Preview(int type,QWebView *webview);
    static void prepareData4Plot(QAbstractItemModel *model,QVector<QString> x,QVector<double> y);


    bool i_Serialize (QString filepath);

    void i_DeSerialize (QString filepath);

    //Converts Numbers to written form
    static const QString ChiffreEnLettre(double montant);

    // PDF printing function
    int Incident_print(QWidget *parent);

    void add_Presence(QString str) {i_Presence.append(str);/*if(i_Presence.size()>1) emit presenceChanged();*/}

    void remove_Presence(int i) {i_Presence.removeAt(i);}
    void add_Fourniture(QString str,int qty) {i_fournitures.insert(str, qty);}

    void remove_Fournitures(QString key) {i_fournitures.remove(key);}
    // Return Agence
    QString get_i_Agence();
    // Set Agence
    void set_i_Agence(QString i_Agence);

    //
    QStringList get_i_Agent();
    void set_i_Agent(QStringList i_Agent);

    //
    QString get_i_Date();
    void set_i_Date(QString i_Date);

    //
    QStringList get_i_Heure_debut_fin();
    void set_i_Heure_debut_fin(QString i_heure,int pos);

    //
    QString get_i_Objet();
    void set_i_Objet(QString i_Objet);

    int get_i_Convention(){return i_Convention;}
    void set_i_Convention(int _convention){i_Convention=_convention;}

    double get_i_Montant(){return i_Montant;}
    void set_i_Montant(double _montant){i_Montant=_montant;}

    int get_i_NumID(){return i_NumID;}
    void set_i_NumID(double _NumID){i_NumID=_NumID;}

    int get_i_Commande();
    void set_i_Commande(int value);

    Incident_Travaux *getI_Travaux() ;
    void setI_Travaux(Incident_Travaux *value);
    QStringList getI_Presence() const;
    void setI_Presence(const QStringList &value);
    QString getI_Date_Facture() const;
    void setI_Date_Facture(const QString &value);

    QString getI_Date_Cmd() const;
    void setI_Date_Cmd(const QString &value);

    int getImputation() const;
    void setImputation(int value);

    void i_print();
    bool getMajoration_Fifty_Night() const;
    void setMajoration_Fifty_Night(bool value);

    bool getMajoration_Fifty_DayOff() const;
    void setMajoration_Fifty_DayOff(bool value);

    int getI_numero_facture() const;
    void setI_numero_facture(int value);

    QString getI_coordinate() const;
    void setI_coordinate(const QString &value);

    QString getI_Direction() const;
    void setI_Direction(const QString &value);
    QStringList getI_notes() const;
    void setI_notes(const QStringList &value);
    int getType() const;
    void setType(int value);
    QMap<QString,int> getI_fournitures() ;
    void setI_fournitures( QMap<QString,int> &value);
    QWebView *getI_webView();
    void setI_webView(QWebView *value);
};

#endif // INCIDENT_H
