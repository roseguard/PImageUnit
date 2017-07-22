#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QImage>
#include <QPainter>
#include <QClipboard>
#include <QCheckBox>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    void makeUnit();
public slots:
    void start();
    void getHtml(QNetworkReply *reply);
    void getImage(QNetworkReply *reply);
private:
    QNetworkAccessManager   *manager;
    QNetworkRequest         request;

    QList<QByteArray>       imageUrlList;
    QList<QPixmap*>         imageList;

    QClipboard              *clipBoardManager;

    QVBoxLayout     *lay;

    QLabel          *adrLab;
    QLineEdit       *adrLine;

    QCheckBox       *saveToClipBoard;

    QLabel          *saveLab;
    QLineEdit       *saveLine;

    QLabel          *status;

    QPushButton     *startButt;


    QLabel          *copyRight;
};

#endif // WIDGET_H
