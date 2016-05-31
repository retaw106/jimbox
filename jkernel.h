#ifndef JKERNEL_H
#define JKERNEL_H

#include <QImage>
#include <QPainter>
#include <Eigen/Dense>

using namespace Eigen;

class JKernel
{
public:
    //default is invariant kernel, t is type of kernel
    JKernel();
    //draw the image of the kernel
    QImage getkimage();
    //calculate the convolution of input image
    //ope: 0Correlation,1Convolution
    MatrixXi getresultim(MatrixXi inputmat, int ope);
    QImage getopeimage(int ope);
    //set type
    void settype(int t);
    void setvalue(int index, double value);
    double sigma=1;
private:
    //kernel numbers
    double num[9];
    //type of kernel:0Manual,1Roberts,2Prewitt,3Sobel
    //4Gaussian,5Median
    int type;
};

#endif // JKERNEL_H
