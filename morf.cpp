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
    {
        ui->ltLabel->setPixmap(QPixmap::fromImage(mat2im(binaryMat*255)));
        ui->dtButton->setEnabled(1);
        ui->skeButton->setEnabled(1);
        ui->skereButton->setEnabled(1);
        ui->edgedetectButton->setEnabled(1);
        ui->gradientButton->setEnabled(0);
        ui->markerCheck->setEnabled(1);
        if (ui->markerCheck->isChecked()) ui->markerSlider->setEnabled(1);
        ui->condilationButton->setEnabled(1);
        ui->obrButton->setEnabled(0);
        ui->cbrButton->setEnabled(0);
    }
    else
    {
        ui->ltLabel->setPixmap(QPixmap::fromImage(mat2im(grayMat)));
        ui->dtButton->setEnabled(0);
        ui->skeButton->setEnabled(0);
        ui->skereButton->setEnabled(0);
        ui->edgedetectButton->setEnabled(0);
        ui->gradientButton->setEnabled(1);
        ui->markerCheck->setEnabled(0);
        ui->markerSlider->setEnabled(0);
        ui->condilationButton->setEnabled(0);
        ui->obrButton->setEnabled(1);
        ui->cbrButton->setEnabled(1);
    }
}

void jimbox::on_dtButton_clicked()
{
    dtMat = distrans(binaryMat);
    ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im(dtMat)));
    ui->rtLabel->clear();
    ui->rbLabel->clear();
    ui->lbText->setText("distance transform");
    ui->rtText->setText("");
    ui->rbText->setText("");
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
    ui->rbLabel->clear();
    ui->lbText->setText("distance transform");
    ui->rtText->setText("skeleton");
    ui->rbText->setText("skeleton restoration");
}

void jimbox::on_skereButton_clicked()
{
    if (ui->rbText->text()=="skeleton restoration")
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
    ui->lbText->setText("standard edge");
    ui->rtText->setText("external edge");
    ui->rbText->setText("internal edge");
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
    ui->lbText->setText("standard gradient");
    ui->rtText->setText("external gradient");
    ui->rbText->setText("internal gradient");
}

//set marker using a slider
void jimbox::on_markerCheck_clicked(bool checked)
{
    if (checked)
    {
        ui->markerSlider->setEnabled(1);
        QImage markerim = mat2im(binaryMat*255).convertToFormat(QImage::Format_RGB32);
        QPainter painter(&markerim);
        painter.setPen(QPen(Qt::red,1,Qt::SolidLine));
        painter.drawLine(ui->markerSlider->value(),0,ui->markerSlider->value(),imheight);
        ui->rtText->setText("marker");
        ui->rtLabel->setPixmap(QPixmap::fromImage(markerim));
        markerMat = ArrayXXi::Zero(binaryMat.rows(),binaryMat.cols());
        markerMat.col(ui->markerSlider->value()).setOnes();
    }
    else
    {
        ui->markerSlider->setEnabled(0);
        ui->rtLabel->clear();
        ui->rtText->setText("");
    }
}

void jimbox::on_markerSlider_valueChanged(int value)
{
    if (ui->tabWidget->currentIndex()==3)
    {
    QImage markerim = mat2im(binaryMat*255).convertToFormat(QImage::Format_RGB32);
    QPainter painter(&markerim);
    painter.setPen(QPen(Qt::red,2,Qt::SolidLine));
    painter.drawLine(ui->markerSlider->value(),0,ui->markerSlider->value(),imheight);
    ui->rtLabel->setPixmap(QPixmap::fromImage(markerim));
    markerMat = ArrayXXi::Zero(binaryMat.rows(),binaryMat.cols());
    markerMat.col(ui->markerSlider->value()).setOnes();
    }
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
    ui->lbLabel->clear();
    ui->lbText->setText("");
    ui->rbText->setText("conditional dilation");
}


void jimbox::on_obrButton_clicked()
{
    int rad1 = ui->ocSpin->value(), rad2 = ui->reconSpin->value();
    ArrayXXi SE=ArrayXXi::Zero(2*rad1+1,2*rad1+1);
    int sO1=rad1,sO2=rad1;
    ArrayXXi obrmat = morphoOpe(1,3,grayMat,SE,sO1,sO2);
    ArrayXXi tmpmat;
    SE=ArrayXXi::Zero(2*rad2+1,1);
    sO1=rad2,sO2=0;
    //SE = Array33i::Ones();
    //sO1=1;sO2=1;
    do {
        tmpmat = obrmat;
        obrmat = morphoOpe(1,0,obrmat,SE,sO1,sO2);
        obrmat = morphoOpe(1,0,obrmat,SE.transpose(),sO2,sO1);
        for (int i=0;i<obrmat.rows();i++)
            for (int j=0;j<obrmat.cols();j++)
            {
                if (obrmat(i,j)>grayMat(i,j))
                    obrmat(i,j)=grayMat(i,j);
            }
        tmpmat = obrmat-tmpmat;
    } while (tmpmat.sum()!=0);
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(obrmat)));
    ui->rtLabel->clear();
    ui->lbLabel->clear();
    ui->rtText->setText("");
    ui->lbText->setText("");
    ui->rbText->setText("OBR result");
}


void jimbox::on_cbrButton_clicked()
{
    int rad1 = ui->ocSpin->value(), rad2 = ui->reconSpin->value();
    ArrayXXi SE=ArrayXXi::Zero(2*rad1+1,2*rad1+1);
    int sO1=rad1,sO2=rad1;
    ArrayXXi cbrmat = morphoOpe(1,2,grayMat,SE,sO1,sO2);
    ArrayXXi tmpmat;
    SE=ArrayXXi::Zero(2*rad2+1,1);
    sO1=rad2,sO2=0;
    //SE = Array33i::Ones();
    //sO1=1;sO2=1;
    do {
        tmpmat = cbrmat;
        cbrmat = morphoOpe(1,0,cbrmat,SE,sO1,sO2);
        cbrmat = morphoOpe(1,0,cbrmat,SE.transpose(),sO2,sO1);
        for (int i=0;i<cbrmat.rows();i++)
            for (int j=0;j<cbrmat.cols();j++)
            {
                if (cbrmat(i,j)>grayMat(i,j))
                    cbrmat(i,j)=grayMat(i,j);
            }
        tmpmat = cbrmat-tmpmat;
    } while (tmpmat.sum()!=0);
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(cbrmat)));
    ui->rtLabel->clear();
    ui->lbLabel->clear();
    ui->rtText->setText("");
    ui->lbText->setText("");
    ui->rbText->setText("CBR result");
}
