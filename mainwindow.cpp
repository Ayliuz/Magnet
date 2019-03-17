#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <cmath>
#include <QInputDialog>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    w_x=new QLabel(this);   // отслеживание координат мыши
    w_y=new QLabel(this);
    w_load=new QLabel(this);
    w_poz=new QLabel(this);


    statusBar()->addWidget(w_x,0);
    statusBar()->addWidget(w_y,0);
    statusBar()->addWidget(w_poz,0);
    statusBar()->addPermanentWidget(w_load,0);

    pmenu=new QMenu("&Меню");
    pmenu->addAction ("&О программе",this,SLOT(about()));
    pmenu->addSeparator();
    pmenu->addAction ("&Сменить режим",this,SLOT(on_Smena_clicked()));
    pmenu->addSeparator();
    pmenu->addAction ("&Автозаполнение",this,SLOT(autozap()));
    pmenu->addSeparator();
    pmenu->addAction ("&Сохранить картинку",this,SLOT(save()));
    pmenu->addSeparator();
    pmenu->addAction ("&Выйти",this,SLOT(close()));
    ui->menuBar->addMenu(pmenu);

    ui->label1->setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);
    setMouseTracking(true);

    pictHeight = 640;
    pictWidth = 850;
    fieldpov=QPixmap(pictWidth,pictHeight);
    fieldmag=QPixmap(pictWidth,pictHeight);
    ui->label1->resize(fieldmag.size());

    k=1000;
    poz=50;
    ui->Potenz->setText(QString().setNum(poz));

    plus=false;
    del=false;
    drag=false;
    povmag=1;
    setWindowTitle("Эквипотенциальные поверхности");

    prov.push_back(QPoint(119,345));
    tok.push_back(5);
    prov.push_back(QPoint(431,119));
    tok.push_back(5);
    prov.push_back(QPoint(683,532));
    tok.push_back(5);

    z.push_back(QPoint(115,345));
    q.push_back(5);
    z.push_back(QPoint(431,119));
    q.push_back(5);
    z.push_back(QPoint(683,532));
    q.push_back(5);
    drawpov();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::drawpov(){                         //рисование поверхности
    w_load->setText("Загрузка");
    setCursor(Qt::WaitCursor);
    QPainter paint;
    paint.begin(& fieldpov);
    paint.eraseRect(0,0,pictWidth,pictHeight);

    paint.setPen(QPen(Qt::blue,2));
    paint.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
    paint.setFont(QFont("Calibri",12,QFont::Normal));
    for (int i=0; i<z.size();++i){                      // рисование зарядов
        if (q[i]<0){
                paint.setPen(QPen(Qt::blue,2));
                paint.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
        }
        else if (q[i]>0){
                paint.setPen(QPen(Qt::red,2));
                paint.setBrush(QBrush(Qt::red,Qt::SolidPattern));
        }
        paint.drawEllipse(z[i].x()-5,z[i].y()-5,10,10);
        paint.drawText(z[i].x()+5,z[i].y()-10,QString().setNum(q[i]));
    }

    paint.setPen(QPen(Qt::black,2));
    paint.setBrush(Qt::NoBrush);
    paint.setRenderHint(QPainter::Antialiasing, true);


    int s=0;
    for (int i=0;i<q.size();++i){
        s+=fabs(q[i]);
    }

    for (int x=0;x<pictWidth;++x) {               // рисование линии
        for (int y=0;y<pictHeight;++y){
            if (((potenzial(x,y-1)>poz) && (potenzial(x,y+1)<poz)) || ((potenzial(x,y-1)<poz) && (potenzial(x,y+1)>poz)) || ((potenzial(x-1,y)>poz) && (potenzial(x+1,y)<poz)) || ((potenzial(x-1,y)<poz) && (potenzial(x+1,y)>poz))){
                paint.drawPoint(x,y);
            }

            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
     }

    paint.end();
    ui->label1->clear();
    ui->label1->setPixmap(fieldpov);
    ui->label1->resize(fieldpov.size());
    setCursor(Qt::ArrowCursor);
    w_load->setText("");
    return;
}

double MainWindow::potenzial(int i, int j){             //расчет потенциала
    double f=0;
    for (int a=0;a<z.size();++a){
        double r=sqrt(std::pow(z[a].x()-i,2)+std::pow(z[a].y()-j,2));
        f+=k*(q[a])/r;
    }

    return f;
}


void MainWindow::drawmag(){                              //рисование магнитных линий
    w_load->setText("Загрузка");
    setCursor(Qt::WaitCursor);
    QVector<QPoint> all;
    QPainter paint;
    paint.begin(& fieldmag);
    paint.eraseRect(0,0,pictWidth,pictHeight);

    paint.setPen(QPen(Qt::black,2));
    paint.setBrush(Qt::NoBrush);
    paint.setFont(QFont("Calibri",12,QFont::Normal));
    for (int i=0; i<prov.size();++i){ //рисование проводников
        paint.drawEllipse(prov[i].x()-5,prov[i].y()-5,10,10);
        paint.drawText(prov[i].x()+10,prov[i].y(),QString().setNum(tok[i]));
        if (tok[i]>0){
            paint.drawLine(prov[i].x()-5,prov[i].y(),prov[i].x()+5,prov[i].y());
            paint.drawLine(prov[i].x(),prov[i].y()-5,prov[i].x(),prov[i].y()+5);
        }
        else if (tok[i]<0){
            paint.drawPoint(prov[i]);
        }
    }

    paint.setPen(QPen(Qt::blue,2));
    paint.setRenderHint(QPainter::Antialiasing, true);

    int pa=0;

    for (int z=0;z<prov.size();++z){
        for (int k=0;k<4;++k){
            double x0,y0;
            switch (k){
                case 1:{
                    x0=prov[z].x()-10;
                    y0=prov[z].y();
                }
                case 2:{
                    x0=prov[z].x();
                    y0=prov[z].y()-30;
                }
                case 3:{
                    x0=prov[z].x()+70;
                    y0=prov[z].y();
                }
                case 4:{
                    x0=prov[z].x();
                    y0=prov[z].y()+120;
                }
            }
            double x=x0;
            double x1,y1;
            x1=x0;
            y1=y0;
            double y=y0*k;
            bool otrisovka=1;
            bool boolpath=0;
            bool boolpath1=0;
            QPainterPath path(QPointF(x,y));
            QPainterPath path1(QPointF(x1,y1));
            QVector<QPoint> all1;
            for (int n=0;n<10;++n){
                path.lineTo(x,y);
                all1.push_back(QPoint(x,y));
                B b_i(induction(x,y).x(),induction(x,y).y());
                x+=b_i.x()/b_i.length();
                y+=b_i.y()/b_i.length();
                path1.lineTo(x1,y1);
                all1.push_back(QPoint(x1,y1));
                B b(induction(x1,y1).x(),induction(x1,y1).y());
                x1-=b.x()/b.length();
                y1-=b.y()/b.length();
            }
            do {
                if (!(x<0 && y<0 ) && !(x>pictWidth && y>pictHeight ) && !(x>pictWidth && y<0) && !(x<0 && y>pictHeight)){
                    path.lineTo(x,y);
                    all1.push_back(QPoint(x,y));
                    B b_i(induction(x,y).x(),induction(x,y).y());
                    x+=b_i.x()/b_i.length();
                    y+=b_i.y()/b_i.length();

                }
                else {
                    boolpath=1;
                }
                if (!(x1<0 && y1<0 ) && !(x1>pictWidth && y1>pictHeight ) && !(x1>pictWidth && y1<0) && !(x1<0 && y1>pictHeight)){
                    path1.lineTo(x1,y1);
                    all1.push_back(QPoint(x1,y1));
                    B b(induction(x1,y1).x(),induction(x1,y1).y());
                    x1-=b.x()/b.length();
                    y1-=b.y()/b.length();
                }
                else{
                     boolpath1=1;
                }

                if (boolpath && boolpath1){
                    break;
                }
                if ((x<0 && y<0 && x1<0 && y1<0) || (x>pictWidth && y>pictHeight && x1>pictWidth && y1>pictHeight) || (x>pictWidth && y<0 && x1>pictWidth && y1<0) || (x<0 && y>pictHeight && x1<0 && y1>pictHeight)){
                    break;
                }
                for (int i=0;i<10;++i ){
                    if ((all.contains(QPoint(x,y-5+i)) && !all1.contains(QPoint(x,y-5+i))) || (all.contains(QPoint(x1,y1-5+i)) && !all1.contains(QPoint(x1,y1-5+i)))){
                        otrisovka=0;
                        break;
                    }
                    if (!otrisovka){
                        break;
                    }
                    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
                }
                if (!otrisovka){
                    break;
                }
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            }
            while( QPointF(x1-x,y1-y).manhattanLength()> 4 );
            if (QPointF(x1-x,y1-y).manhattanLength()<=4){
                path.lineTo(path1.currentPosition());
            }
            if (otrisovka){
                paint.drawPath(path);
                paint.drawPath(path1);
                all+=all1;
                pa=pa+1;
            }
        }
    }
    paint.end();
    ui->label1->clear();
    ui->label1->setPixmap(fieldmag);
    setCursor(Qt::ArrowCursor);
    w_load->setText("");
    return;
}




B MainWindow::induction(int x, int y){                   // расчёт магнитной индукции
    QVector<B> ind;
    for (int a=0;a<prov.size();++a){
        double r=sqrt(pow(prov[a].x()-x,2)+pow(prov[a].y()-y,2));
        double m_i=k*tok[a]/r;
        double sina=(x-prov[a].x())/r;
        double cosa=(-y+prov[a].y())/r;
        ind.push_back(B(m_i*cosa,m_i*sina));
    }
    B b;
    for (int i=0;i<ind.size();++i){
        b.getx(ind[i].x()+b.x());
        b.gety(ind[i].y()+b.y());
    }
    return b;
}

void MainWindow::on_Plus_clicked()
{
    if ((!plus && (del || drag)) || (!plus && !del && !drag)){
        plus=true;
        del=false;
        drag=false;
    }
    else{
        plus=false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *pe){ // курсоры, движение мышки
    QPoint pos=pe->pos()-QPoint(0,ui->menuBar->size().height());
    if (povmag && ui->label1->geometry().contains(pos)){
        w_poz->setText("Потенциал: "+ QString().setNum(float ( ceil (10*potenzial((pos-ui->label1->pos()).x(),(pos-ui->label1->pos()).y()))/10)));
    }
    w_x->setText(QString().setNum(pos.x()));
    w_y->setText(QString().setNum(pos.y()));
    if ((plus || drag) && ui->label1->geometry().contains(pos)){
        QPixmap Cursor(12,12);
        QPainter p;
        p.begin(&Cursor);
        p.eraseRect(0,0,12,12);
        p.setPen(QPen(Qt::darkBlue,2));
        p.setBrush(QBrush(Qt::darkBlue,Qt::SolidPattern));
        p.drawEllipse(1,1,10,10);
        p.end();
        setCursor(QCursor(Cursor));
    }
    if (del && ui->label1->geometry().contains(pos)){
        setCursor(Qt::CrossCursor);
    }
    if ((plus || del || drag) && !ui->label1->geometry().contains(pos)) {
            setCursor(Qt::ForbiddenCursor);
    }
    if (!(plus || del || drag)) {
            setCursor(Qt::ArrowCursor);
    }

}
void MainWindow::mousePressEvent(QMouseEvent *pe){          // нажатие кнопок мыши
    QPoint pos=pe->pos()-QPoint(0,ui->menuBar->size().height());

    if (ui->label1->geometry().contains(pos) && !plus && !del && !drag){ // начало перемещения заряда
        if (povmag){
            for (int i=0;i< z.size();++i) {
                QPoint p= z[i]+ui->label1->pos();
                if (QRect(p.x()-5,p.y()-5,10,10).contains(pos)){
                    inddrag=i;
                    drag=1;
                    break;
                }
            }
        }

        else{
            for (int i=0;i< prov.size();++i) {
                QPoint p= prov[i]+ui->label1->pos();
                if (QRect(p.x()-5,p.y()-5,10,10).contains(pos)){
                    inddrag=i;
                    drag=1;
                    break;
                }
            }

        }
    }
    else if (ui->label1->geometry().contains(pos) && !plus && !del && drag){ // конец перемещения заряда
        if (povmag){
            drag=0;
            z[inddrag]=pos-ui->label1->pos();
            setCursor(Qt::WaitCursor);
            drawpov();
        }
        else{
            drag=0;
            prov[inddrag]=pos-ui->label1->pos();
            setCursor(Qt::WaitCursor);
            drawmag();
        }
    }
    bool bok;
    if (plus==true && ui->label1->geometry().contains(pos)){ //добавление
        if (povmag){
            QInputDialog* input=new QInputDialog;
            int charge = input->getInt(0,"Ввод заряда","Введите значение заряда (от -30 до 30):",0,-30,30,1,&bok, Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
            if(bok){
                plus=false;
                q.push_back(charge);
                z.push_back(pos-ui->label1->pos());
                setCursor(Qt::WaitCursor);
                drawpov();
            }
        }
        else{
            QInputDialog* input=new QInputDialog;
            int ttk = input->getInt(0,"Ввод силы тока","Введите значение силы тока(от -30 (направление \"из экрана\") до 30 (направление \"в экран\")):",0,-30,30,1,&bok,  Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
            if(bok){
                plus=false;
                tok.push_back(ttk);
                prov.push_back(pos-ui->label1->pos());
                setCursor(Qt::WaitCursor);
                drawmag();
            }
        }
    }
    if (del==true && ui->label1->geometry().contains(pos)){ // удаление
            if (povmag){
                for (int i=0;i< z.size();++i) {
                    QPoint p= z[i]+ui->label1->pos();
                    if (QRect(p.x()-5,p.y()-5,10,10).contains(pos)){
                        z.remove(i);
                        q.remove(i);
                        del=false;
                        drawpov();
                        break;
                    }
                }
            }
            else{
                for (int i=0;i< prov.size();++i) {
                    QPoint p= prov[i]+ui->label1->pos();
                    if (QRect(p.x()-5,p.y()-5,10,10).contains(pos)){
                        prov.remove(i);
                        tok.remove(i);
                        del=false;
                        drawmag();
                        break;
                    }
                }

            }

    }

}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *pe){
    QPoint pos=pe->pos()-QPoint(0,ui->menuBar->size().height());
    if (ui->label1->geometry().contains(pos) && !plus && !del && !drag){ // начало перемещения заряда
        if (povmag){
            for (int i=0;i< z.size();++i) {
                QPoint p= z[i]+ui->label1->pos();
                if (QRect(p.x()-5,p.y()-5,10,10).contains(pos)){
                    bool bok;
                    QInputDialog* input=new QInputDialog;
                    int charge = input->getInt(0,"Ввод заряда","Введите значение заряда (от -30 до 30):",0,-30,30,1,&bok, Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
                    if(bok){
                        q[i]=charge;
                        setCursor(Qt::WaitCursor);
                        drawpov();
                        break;
                    }
                }
            }
        }

        else{
            for (int i=0;i< prov.size();++i) {
                QPoint p= prov[i]+ui->label1->pos();
                if (QRect(p.x()-5,p.y()-5,10,10).contains(pos)){
                    bool bok;
                    QInputDialog* input=new QInputDialog;
                    int ttk = input->getInt(0,"Ввод силы тока","Введите значение силы тока(от -30 (направление \"из экрана\") до 30 (направление \"в экран\")):",0,-30,30,1,&bok,  Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
                    if(bok){
                        plus=false;
                        tok[i]=ttk;
                        setCursor(Qt::WaitCursor);
                        drawmag();
                        break;
                    }
                }

            }
        }
    }
}
void MainWindow::on_Potenz_returnPressed()                  // ввод потенциала
{
    poz=ui->Potenz->text().toDouble();
    ui->Potenz->setText(QString().setNum(poz));
    drawpov();
}

void MainWindow::on_Del_clicked()
{
    if ((!del && (plus || drag)) || (!plus && !del && !drag)){
        del=true;
        plus=false;
        drag=false;
    }
    else{
        del=false;
    }
}

void MainWindow::on_Clear_clicked()
{
    plus=false;
    del=false;
    drag=false;
    if (povmag){
        QPainter paint;
        paint.begin(& fieldpov);
        paint.eraseRect(0,0,pictWidth,pictHeight);
        paint.end();
        z.clear();
        q.clear();
        drawpov();
    }
    else{
        QPainter paint;
        paint.begin(& fieldmag);
        paint.eraseRect(0,0,pictWidth,pictHeight);
        paint.end();
        tok.clear();
        prov.clear();
        drawmag();
    }
}

void MainWindow::on_Smena_clicked()
{
    povmag=!povmag;
    plus=false;
    del=false;
    drag=false;
    if (povmag){
        ui->Plus->setText("Добавить заряд");
        ui->Del->setText("Удалить заряд");
        ui->Smena->setText("Линии\nмагнитной\nиндукции");
        ui->perem->setText("Для перемещения зарядов \n нажмите и отпустите\n кнопку мыши");
        ui->Char->show();
        ui->Potenz->show();
        setWindowTitle("Эквипотенциальные поверхности");
        ui->condenser->show();
        drawpov();
    }
    else{
        ui->Plus->setText("Добавить\nпроводник");
        ui->Del->setText("Удалить\nпроводник");
        ui->Smena->setText("Эквипотенциальные\nповерхности");
        ui->perem->setText("Для перемещения\n проводников \n нажмите и отпустите\n кнопку мыши");
        ui->Char->hide();
        ui->Potenz->hide();
        setWindowTitle("Линии магнитной индукции");
        w_poz->setText("");
        ui->condenser->hide();
        drawmag();
    }

}

void MainWindow::about(){
    QMessageBox* mb=new QMessageBox;
    mb->setIcon(QMessageBox::Information);
    mb->setWindowFlags(Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    mb->setWindowTitle("О программе");
    mb->setText("<font size=\"5\" face=\"Calibri\"> <p align=\"center\">  <font size=\"8\">П</font>остроение эквипотенциальных поверхностей электростатического поля неподвижных зарядов <br> и <br>  <font size=\"8\">Л</font>иний магнитной индукции вокруг проводников с током  </p> <p align=\"left\"> <b><font size=\"8\">A</font>втор:</b> Зуев Илья <br> <p align=\"right\">Гимназия №1567 2017г.</p></p> </font>");
    mb->exec();
    delete mb;
}

void MainWindow::save(){
    QString format;
    if (povmag){
        QString adress=QFileDialog::getSaveFileName(0,tr("Сохранить картинку"),"Эквипотенциальные поверхности _ потенциал " + QString().setNum(float ( ceil (10*poz)/10)),"*.jpg ;; *.png ;; *.bmp",&format);
        if (!adress.isEmpty()){
            if (format.contains("jpg")){
                fieldpov.save(adress,"JPG",100);
            }
            else if (format.contains("png")){
                fieldpov.save(adress,"PNG",100);
            }
            else {
                fieldpov.save(adress,"BMP",100);
            }
        }
    }
    else{
        QString adress=QFileDialog::getSaveFileName(0,tr("Сохранить картинку"),"Линии магнитной индукции","*.jpg ;; *.png ;; *.bmp",&format);
        if (!adress.isEmpty()){
            if (format.contains("jpg")){
                fieldmag.save(adress,"JPG",100);
            }
            else if (format.contains("png")){
                fieldmag.save(adress,"PNG",100);
            }
            else {
                fieldmag.save(adress,"BMP",100);
            }
        }
    }
}


void MainWindow::on_condenser_clicked()
{
    plus=false;
    del=false;
    drag=false;
    bool bok;
    QInputDialog* input=new QInputDialog;
    int charge = input->getInt(0,"Ввод заряда обкладки","Введите значение заряда на обкладке конденсатора (от 0 до 50):",0,0,50,1,&bok, Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    if(bok){
        z.clear();
        q.clear();
        for (int i=0;i<10;++i){
            q.push_back(charge);
            z.push_back(QPoint (pictWidth/2-135+i*30,pictHeight/2-30));
        }
        for (int i=0;i<10;++i){
            q.push_back(-charge);
            z.push_back(QPoint (pictWidth/2-135+i*30,pictHeight/2+30));
        }
        drawpov();
    }
}

void MainWindow::autozap(){
    plus=false;
    del=false;
    drag=false;
    if (povmag){
        z.clear();
        q.clear();
        z.push_back(QPoint(115,345));
        q.push_back(5);
        z.push_back(QPoint(431,119));
        q.push_back(5);
        z.push_back(QPoint(683,532));
        q.push_back(5);
        drawpov();
    }
    else{
        prov.clear();
        tok.clear();
        prov.push_back(QPoint(119,345));
        tok.push_back(5);
        prov.push_back(QPoint(431,119));
        tok.push_back(5);
        prov.push_back(QPoint(683,532));
        tok.push_back(5);
        drawmag();
    }
}
