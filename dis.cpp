//DIsdance transform and Skeleton
#include "jimbox.h"
#include "ui_jimbox.h"

//distance transform
ArrayXXi jimbox::distrans(ArrayXXi inputmat)
{
    ArrayXXi SE = ArrayXXi::Ones(3,3);
    int sO1=1,sO2=1;
    ArrayXXi eromat;
    ArrayXXi outputmat = ArrayXXi::Zero(inputmat.rows(),inputmat.cols());
    int i=1;
    while (!inputmat.isZero()) {
        eromat = morphoOpe(0,1,inputmat,SE,sO1,sO2);
        outputmat += (inputmat-eromat)*i;
        i=i+1;
        inputmat = eromat;
    }
    return outputmat;
}

void jimbox::on_dtButton_clicked()
{
    ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im(distrans(binaryMat))));
}
