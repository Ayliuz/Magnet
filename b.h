#ifndef B_H
#define B_H


class B
{
public:
    B();
    B(double xi, double yi);
    double x();
    double y();
    double length();
    void getx(double x);
    void gety(double y);

private:
    double width;
    double height;
    double ang;
};

#endif // B_H
