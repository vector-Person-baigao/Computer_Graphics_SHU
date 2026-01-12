#include "Point.h"

TMatrix::TMatrix() { init(); }
void TMatrix::init(){
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++) {
            val[i][j] = (i == j) ? 1.0 : 0.0;
            ival[i][j] = (i == j) ? 1.0 : 0.0;
        }
}

double TMatrix::det(int d) {
    if(d == 1) {
        return val[0][0] * (val[1][1] * val[2][2] - val[1][2] * val[2][1])
             - val[0][1] * (val[1][0] * val[2][2] - val[1][2] * val[2][0])
             + val[0][2] * (val[1][0] * val[2][1] - val[1][1] * val[2][0]);
    } else {
        return ival[0][0] * (ival[1][1] * ival[2][2] - ival[1][2] * ival[2][1])
             - ival[0][1] * (ival[1][0] * ival[2][2] - ival[1][2] * ival[2][0])
             + ival[0][2] * (ival[1][0] * ival[2][1] - ival[1][1] * ival[2][0]);
    }
}

Point Point::transform(TMatrix m, int d) const {
    if(d == 1) {
        return Point(static_cast<int>(x*m.val[0][0] + y*m.val[0][1] + m.val[0][2]),
                     static_cast<int>(x*m.val[1][0] + y*m.val[1][1] + m.val[1][2]));
    } else {
        return Point(static_cast<int>(x*m.ival[0][0] + y*m.ival[0][1] + m.ival[0][2]),
                     static_cast<int>(x*m.ival[1][0] + y*m.ival[1][1] + m.ival[1][2]));
    }
}

TMatrix Compose(TMatrix m1, TMatrix m2) {   // T2(T1(P)) -> m2*m1
    TMatrix res;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            res.val[i][j] = 0.0;
            res.ival[i][j] = 0.0;
            for(int k = 0; k < 3; k++) {
                res.val[i][j] += m2.val[i][k] * m1.val[k][j];
                res.ival[i][j] += m1.ival[i][k] * m2.ival[k][j];
            }
        }
    }
    return res;
}

TMatrix Compose(const std::vector<TMatrix>& matrices) {
    TMatrix res;
    for(const auto& m : matrices) {
        res = Compose(m, res);
    }
    return res;
}

TMatrix shift(int dx, int dy) {
    TMatrix m;
    m.val[0][2] = dx; m.ival[0][2] = -dx;
    m.val[1][2] = dy; m.ival[1][2] = -dy;
    return m;
}

TMatrix rotate(double theta) {
    TMatrix m;
    double cos_theta = cos(theta);
    double sin_theta = sin(theta);
    m.val[0][0] = cos_theta;  m.val[0][1] = -sin_theta;
    m.val[1][0] = sin_theta;  m.val[1][1] = cos_theta;
    m.ival[0][0] = cos_theta;  m.ival[0][1] = sin_theta;
    m.ival[1][0] = -sin_theta; m.ival[1][1] = cos_theta;
    return m;
}

TMatrix scale(double dx, double dy) {
    TMatrix m;
    m.val[0][0] = dx; m.ival[0][0] = 1.0/dx;
    m.val[1][1] = dy; m.ival[1][1] = 1.0/dy;
    return m;
}

double dis2p(const Point p1, const Point p2) {
    return pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2);
}