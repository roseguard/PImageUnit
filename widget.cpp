#include "widget.h"
#include <QDebug>
#include <QApplication>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    lay = new QVBoxLayout(this);

    adrLab = new QLabel(tr("Адресс поста"), this);
    adrLine = new QLineEdit(this);

    saveToClipBoard = new QCheckBox(tr("Сохранять конечное изображение в буфер обмена"), this);

    saveLab = new QLabel(tr("Путь для сохранения"), this);
    saveLine = new QLineEdit("D:\\\\save.png" ,this);

    status = new QLabel("...", this);
    status->setAlignment(Qt::AlignCenter);

    startButt = new QPushButton(tr("Начать"), this);
    connect(startButt, SIGNAL(clicked(bool)), this, SLOT(start()));

    copyRight = new QLabel("roseguardd@gmail.com");
    copyRight->setAlignment(Qt::AlignRight);

    lay->addWidget(adrLab);
    lay->addWidget(adrLine);
    lay->addWidget(saveToClipBoard);
    lay->addWidget(saveLab);
    lay->addWidget(saveLine);
    lay->addWidget(status);
    lay->addWidget(startButt);
    lay->addWidget(copyRight);

    manager = new QNetworkAccessManager(this);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    clipBoardManager = QApplication::clipboard();
}

Widget::~Widget()
{

}

void Widget::start()
{
    startButt->setDisabled(true);
    status->setText(tr("Запускаем"));
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getHtml(QNetworkReply*)));
    request.setUrl(QUrl(adrLine->text()));
    manager->get(request);
}

void Widget::getHtml(QNetworkReply *reply)
{
    status->setText(tr("Получаем разметку страницы"));
    QByteArray array = reply->readAll();
    status->setText(tr("Удаляем лишнее"));
    int temp = array.indexOf("_start-->");
    array.remove(0, temp+9);
    temp = array.indexOf("<!--story_");
    array.remove(temp+10, array.length()-(temp+10));
    status->setText(tr("Ищем пути к изображениям"));
    QString temple = "data-large-image=\"";
    while(true)
    {
        temp = array.indexOf(temple);
        if(temp!=-1)
        {
            array.remove(0, temp+temple.length());
            temp = array.indexOf("\"");
            QByteArray tempCopy;
            for(int i = 0; i < temp; i++)
            {
                tempCopy.append(array.at(i));
            }
            imageUrlList.append(tempCopy);
            array.remove(0, temp);
        }
        else
        {
            break;
        }
    }
    if(imageUrlList.isEmpty())
    {
        status->setText(tr("Изображения не были найдены"));
        startButt->setEnabled(true);
        return;
    }
    status->setText(tr("Получаем изображения"));
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getHtml(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getImage(QNetworkReply*)));
    request.setUrl(QUrl(imageUrlList.first()));
    imageUrlList.removeFirst();
    manager->get(request);
}

void Widget::getImage(QNetworkReply *reply)
{
    QPixmap *pix = new QPixmap;
    QByteArray datas = reply->readAll();
    pix->loadFromData(datas);
    imageList.append(pix);
    if(imageUrlList.isEmpty())
    {
        disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getImage(QNetworkReply*)));
        makeUnit();
        startButt->setEnabled(true);
        return;
    }
    request.setUrl(QUrl(imageUrlList.first()));
    imageUrlList.removeFirst();
    manager->get(request);
}

void Widget::makeUnit()
{
    status->setText(tr("Объединяем изображения"));
    int totalHeight = 0;
    int maxWidth = 0;
    for(int i = 0; i < imageList.length(); i++)
    {
        totalHeight += imageList.at(i)->height();
        if(imageList.at(i)->width() > maxWidth)
            maxWidth = imageList.at(i)->width();
    }
    QPixmap finalImage(maxWidth, totalHeight);
    QPainter paint(&finalImage);
    int lastHeight = 0;
    for(int i = 0; i < imageList.length(); i++)
    {
        paint.drawPixmap(0, lastHeight, maxWidth, imageList.at(i)->height(), *imageList[i]);
        lastHeight+=imageList.at(i)->height();
        delete imageList[i];
    }
    imageList.clear();
    status->setText(tr("Сохраняем результаты"));
    if(!saveLine->text().isEmpty())
        finalImage.save(saveLine->text());
    if(saveToClipBoard->isChecked())
        clipBoardManager->setPixmap(finalImage);
    status->setText(tr("Готово"));
}
