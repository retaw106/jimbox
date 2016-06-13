//morphological functions
#include "jimbox.h"
#include "ui_jimbox.h"

//distance transform
ArrayXXi jimbox::distrans(ArrayXXi inputmat)
{
    //ArrayXXi SE = Array33i::Ones();
    ArrayXXi SE1=Array3i::Ones();
    ArrayXXi SE2=SE1.transpose();
    int sO1=1,sO2=0;
    ArrayXXi eromat;
    ArrayXXi outputmat = ArrayXXi::Zero(inputmat.rows(),inputmat.cols());
    int i=1;
    while (inputmat.sum()) {
        //eromat = morphoOpe(0,1,inputmat,SE,sO1,sO2);
        eromat = morphoOpe(0,1,morphoOpe(0,1,inputmat,SE1,sO1,sO2),SE2,sO2,sO1);
        outputmat += (inputmat-eromat)*i;
        i=i+1;
        inputmat = eromat;
    }
    return outputmat;
}


void jimbox::on_mimType_currentIndexChanged(int index)
{
    if (index==0)
        ui->ltLabel->setPixmap(QPixmap::fromImage(mat2im(binaryMat*255)));
    else ui->ltLabel->setPixmap(QPixmap::fromImage(mat2im(grayMat)));
}

void jimbox::on_dtButton_clicked()
{
    dtMat = distrans(binaryMat);
    ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im(dtMat)));
}

void jimbox::on_skeButton_clicked()
{
    if (!ui->lbLabel->pixmap())
    {
        dtMat = distrans(binaryMat);
        ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im(dtMat)));
    }
    ArrayXXi tmpmat = ArrayXXi::Zero(dtMat.rows()+2,dtMat.cols()+2);
    tmpmat.block(1,1,dtMat.rows(),dtMat.cols()) = dtMat;
    rkMat = dtMat;
    skeMat = ArrayXXi::Zero(dtMat.rows(),dtMat.cols());
    for (int i=0;i<dtMat.rows();i++)
        for (int j=0;j<dtMat.cols();j++)
        {
            if (tmpmat(i+1,j+1)>0)
            {
            if (tmpmat(i+1,j+1)<tmpmat.block(i,j,3,3).maxCoeff())
                rkMat(i,j)=0;
            else skeMat(i,j)=1;
            }
        }
    ui->rtLabel->setPixmap(QPixmap::fromImage(mat2im(skeMat*255)));
}

void jimbox::on_skereButton_clicked()
{
    ArrayXXi skereMat = ArrayXXi::Zero(rkMat.rows(),rkMat.cols());
    for (int i=0;i<rkMat.rows();i++)
        for (int j=0;j<rkMat.cols();j++)
        {
            int s = rkMat(i,j);
            if (s>0)
            {
                skereMat.block(i-s+1,j-s+1,2*s-1,2*s-1).setOnes();
            }
        }
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(skereMat*255)));
}

//morphological edge detection
void jimbox::on_edgedetectButton_clicked()
{
    ArrayXXi SE1=Array3i::Ones();
    ArrayXXi SE2=SE1.transpose();
    int sO1=1,sO2=0;
    ArrayXXi dilmat,eromat;
    dilmat = morphoOpe(0,0,morphoOpe(0,0,binaryMat,SE1,sO1,sO2),SE2,sO2,sO1);
    eromat = morphoOpe(0,1,morphoOpe(0,1,binaryMat,SE1,sO1,sO2),SE2,sO2,sO1);
    ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im((dilmat-eromat)*255)));
    ui->rtLabel->setPixmap(QPixmap::fromImage(mat2im((dilmat-binaryMat)*255)));
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im((binaryMat-eromat)*255)));
}

//morphological gradient
void jimbox::on_gradientButton_clicked()
{
    ArrayXXi SE1=Array3i::Zero();
    ArrayXXi SE2=SE1.transpose();
    int sO1=1,sO2=0;
    ArrayXXi dilmat,eromat;
    dilmat = morphoOpe(1,0,morphoOpe(1,0,grayMat,SE1,sO1,sO2),SE2,sO2,sO1);
    eromat = morphoOpe(1,1,morphoOpe(1,1,grayMat,SE1,sO1,sO2),SE2,sO2,sO1);
    ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im((dilmat-eromat)/2)));
    ui->rtLabel->setPixmap(QPixmap::fromImage(mat2im((dilmat-grayMat)/2)));
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im((grayMat-eromat)/2)));
}

//set marker using a slider
void jimbox::on_markerCheck_clicked(bool checked)
{
    if (checked)
    {
        ui->markerSlider->setEnabled(1);
        ui->markerSlider->setMaximum(imwidth-1);
        ui->markerSlider->setValue(imwidth/2-1);
        QImage markerim = mat2im(binaryMat*255).convertToFormat(QImage::Format_RGB32);
        QPainter painter(&markerim);
        painter.setPen(QPen(Qt::red,1,Qt::SolidLine));
        painter.drawLine(ui->markerSlider->value(),0,ui->markerSlider->value(),imheight);
        ui->rtLabel->setPixmap(QPixmap::fromImage(markerim));
        markerMat = ArrayXXi::Zero(binaryMat.rows(),binaryMat.cols());
        markerMat.col(ui->markerSlider->value()).setOnes();
    }
    else
    {
        ui->markerSlider->setEnabled(0);
        ui->rtLabel->clear();
    }
}

void jimbox::on_markerSlider_valueChanged(int value)
{
    QImage markerim = mat2im(binaryMat*255).convertToFormat(QImage::Format_RGB32);
    QPainter painter(&markerim);
    painter.setPen(QPen(Qt::red,1,Qt::SolidLine));
    painter.drawLine(ui->markerSlider->value(),0,ui->markerSlider->value(),imheight);
    ui->rtLabel->setPixmap(QPixmap::fromImage(markerim));
    markerMat = ArrayXXi::Zero(binaryMat.rows(),binaryMat.cols());
    markerMat.col(ui->markerSlider->value()).setOnes();
}


void jimbox::on_condilationButton_clicked()
{
    ArrayXXi SE1=Array3i::Ones();
    ArrayXXi SE2=SE1.transpose();
    int sO1=1,sO2=0;
    ArrayXXi cdmat,tmpmat;
    if (ui->markerCheck->isChecked())
        cdmat = markerMat*binaryMat;
    else
    {
        cdmat = morphoOpe(0,1,morphoOpe(0,1,binaryMat,SE1,sO1,sO2),SE2,sO2,sO1);
        cdmat = morphoOpe(0,1,morphoOpe(0,1,cdmat,SE1,sO1,sO2),SE2,sO2,sO1);
    }
    do {
        tmpmat = cdmat;
        cdmat = morphoOpe(0,0,morphoOpe(0,0,cdmat,SE1,sO1,sO2),SE2,sO2,sO1);
        cdmat *= binaryMat;
    } while (cdmat.sum()!=tmpmat.sum());
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(cdmat*255)));
}


void jimbox::on_obrButton_clicked()
{
    ArrayXXi SE=ArrayXXi::Zero(5,5);
    int sO1=2,sO2=2;
    ArrayXXi obrmat = morphoOpe(1,3,grayMat,SE,sO1,sO2);
    ArrayXXi tmpmat;
    //SE = Array33i::Ones();
    //sO1=1;sO2=1;
    do {
        tmpmat = obrmat;
        obrmat = morphoOpe(1,0,obrmat,SE,sO1,sO2);
        for (int i=0;i<obrmat.rows();i++)
            for (int j=0;j<obrmat.cols();j++)
            {
                if (obrmat(i,j)>grayMat(i,j))
                    obrmat(i,j)=grayMat(i,j);
            }
        tmpmat = obrmat-tmpmat;
    } while (tmpmat.sum()!=0);
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(obrmat)));
}
