#include "b.h"
#include <cmath>

B::B(){
    width=0;
    height=0;
}

B::B(double xi, double yi)
{
    width=xi;
    height=yi;

}
void B::getx(double x){
    width=x;
}
void B::gety(double y){
    height=y;
}

double B::x(){
    return width;
}
double B::y(){
    return height;
}

double B::length(){
    return sqrt(pow(width,2)+pow(height,2));
}
