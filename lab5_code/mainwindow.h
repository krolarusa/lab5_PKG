#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <plane.h>
#include <QGridLayout>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void showCirus(){
        p->hide();
        p = new plane(type::Cirus);
        l->addWidget(p,0,0,12,1);
        p->show();
    }
    void showCentral(){
        p->hide();
        p = new plane(type::Central);
        l->addWidget(p,0,0,12,1);
        p->show();
    }
private:
    plane* p = new plane(type::Cirus);
    QGridLayout *l = new QGridLayout;
    Ui::MainWindow *ui;
    QPushButton *pbCentral;
    QPushButton *pbCirus;
};
#endif // MAINWINDOW_H
