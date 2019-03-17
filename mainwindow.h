#include <QMainWindow>
#include <QtGui>
#include <QVector>
#include <QtWidgets>
#include "b.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void drawpov(); // рисование поверхностей
    void drawmag(); // рисование линий магнитной индукции
    double potenzial( int i, int j);//расчёт потенциала
    B induction(int x,int y);

protected:

  virtual void mouseMoveEvent(QMouseEvent *pe);
  virtual void mousePressEvent(QMouseEvent *pe);
  virtual void mouseDoubleClickEvent(QMouseEvent *pe);

private slots:

    void about();

    void save();

    void autozap();

    void on_Plus_clicked();

    void on_Potenz_returnPressed();

    void on_Del_clicked();

    void on_Clear_clicked();

    void on_Smena_clicked();

    void on_condenser_clicked();

private:
    Ui::MainWindow *ui;
    int pictWidth,pictHeight; // ширина и высота картинки
    QPixmap fieldpov; // картинка поверхностей
    QPixmap  fieldmag; //картинка линий магнитной индукции
    QVector <QPoint> z; // координаты зарядов
    QVector <int> q; // значения зарядов
    QVector <QPoint> prov; // координаты проводников
    QVector <int> tok; // значения силы тока
    int k; // коэффициент потенциала
    double poz; // искомый потенциал
    bool plus; //добавление заряда/
    bool del;// удаление заряда
    bool drag; // перемещение
    int inddrag; // индекс перемещения
    bool povmag; // обозначение задания
    QLabel* w_x;
    QLabel* w_y;
    QLabel* w_load;
    QLabel* w_poz;
    QMenu* pmenu;
};


