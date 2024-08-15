#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<string>
#include <iostream>
#include <regex>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,socket(new tcp(this))
{
    ui->setupUi(this);
    ui->searchBtn->setIcon(QIcon(":/icon/icon/search .png"));
    ui->listWidget->hide();  // 默认隐藏推荐列表
    QObject::connect(ui->searchLine,&QLineEdit::textChanged,this,&MainWindow::Keyword_recommendation);
    QObject::connect(socket,&tcp::changeLabel,this,&MainWindow::Label_show);
    QObject::connect(socket,&tcp::web_changeLable,this,&MainWindow::web_Label_show);
    QObject::connect(ui->listWidget,&QListWidget::itemClicked, this, &MainWindow::onRecommendationClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QStringList MainWindow::getList(QString data)
{
    //第一个表示功能1 KEY关键字搜索
    //recv = ["KEY","我中有你","你中有我"]
     QStringList items;
 // 去掉起始的 "[" 和结尾的 "]"
    data = data.mid(data.indexOf('[') + 1, data.lastIndexOf(']') - data.indexOf('[') - 1);

    // 通过逗号拆分字符串
    QStringList rawItems = data.split(',');

    // 去掉每个元素的引号，并将其装入 items 列表
    for (QString &item : rawItems) {
        item = item.trimmed();  // 去掉首尾空格
        item = item.mid(1, item.length() - 2);  // 去掉引号
        items.append(item);
    }
    items.pop_front();
    return items;
}

QStringList MainWindow::getWebList(QString data)
{
    QStringList items;
    // 去掉起始的 "[" 和结尾的 "]"
    data = data.mid(data.indexOf('[') + 1, data.lastIndexOf(']') - data.indexOf('[') - 1);

    // 通过逗号拆分字符串
    QStringList rawItems = data.split(',');
//    标题：中国海油启动碳中和规划全面推动公司绿色低碳转型 摘要：海油  低碳  LNG  绿色  排放  链接：http://ccnews.people.com.cn/n1/2021/0118/c141677-32002853.html
//    摘要：海油  低碳  LNG  绿色  排放
//    链接：http://ccnews.people.com.cn/n1/2021/0118/c141677-32002853.html

    // 去掉每个元素的引号，并将其装入 items 列表
    for (QString &item : rawItems) {
        item = item.trimmed();  // 去掉首尾空格
        item = item.mid(1, item.length() - 2);  // 去掉引号
        //拆分对应
        std::string input = item.toUtf8().constData();
        // 正则表达式匹配“摘要”和“链接”并在其前后插入换行符
        input = std::regex_replace(input, std::regex("(摘要：)"), "\n$1");
        input = std::regex_replace(input, std::regex("(链接：)"), "\n$1");
        input +="\n\n";
        // 将处理后的 std::string 转换回 QString
        QString processedItem = QString::fromUtf8(input.c_str());
        items.append(processedItem);
    }
    items.pop_front();

    return items;
}

void MainWindow::Keyword_recommendation(QString data)
{
    // 构建JSON消息
    Json msg;
    msg["chose"] = 1;
    msg["text"] = data.toUtf8().constData(); // 确保转换为const char*
    // 发送数据
    socket->sendmsg(msg);
}

void MainWindow::Label_show(QString data)
{
    ui->listWidget->clear();
    ui->listWidget->hide();
    if(data!="[]")
    {
        //recv = ["我中有你","你中有我"]
        //拆分
        QStringList items = getList(data);
        ui->listWidget->addItems(items);
        ui->listWidget->show();
        //允许点击列表项--->单选模式
        ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    }

}

void MainWindow::web_Label_show(QString data)
{
    ui->listWidget->clear();
    ui->listWidget->hide();
    if(data!="[]")
    {
        //recv = ["我中有你","你中有我"]
        //拆分
        //qDebug()<<data;
        QStringList items = getWebList(data);
        ui->listWidget->addItems(items);
        ui->listWidget->show();
        // 禁止点击列表项
        ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection);
    }

}

void MainWindow::onRecommendationClicked(QListWidgetItem *item)
{
    // 点击推荐项后，将其内容放入搜索栏并触发新的搜索
      ui->searchLine->setText(item->text());
      Web_search(item->text());
       // 隐藏推荐列表
      ui->listWidget->clear();
      ui->listWidget->hide();
}

void MainWindow::Web_search(QString data)
{
    // 构建JSON消息
    Json msg;
    msg["chose"] = 2;
    msg["text"] = data.toUtf8().constData(); // 确保转换为const char*
    // 发送数据
    socket->sendmsg(msg);
}




void MainWindow::on_searchBtn_clicked()
{
    Web_search(ui->searchLine->text());
    // 隐藏推荐列表
    ui->listWidget->hide();
}
