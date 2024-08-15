#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtWidgets/QtWidgets>
#include<QTcpSocket>
#include <QMessageBox>
#include"json.hpp"
#include"tcp.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

using Json = nlohmann::json;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     QStringList getList(QString data);
     QStringList getWebList(QString data);
private slots:
    void Keyword_recommendation(QString data);
    void Label_show(QString data);
    void web_Label_show(QString data);
    void onRecommendationClicked(QListWidgetItem *item);
    void Web_search(QString data);
    void on_searchBtn_clicked();

private:
    Ui::MainWindow *ui;
    tcp * socket;

};
#endif // MAINWINDOW_H
