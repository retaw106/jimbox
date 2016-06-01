//tab: histagram and threshold
#include "jimbox.h"
#include "ui_jimbox.h"

//draw histgram
void jimbox::drawHist()
{
    //data
    QPoint originP(originPx,originPy);
    int maxhist = 1;
    for (int x=0;x<256;x++)
    {
        if (histData.at(x) > maxhist)
            maxhist = histData.at(x);
    }
    //Painter settings.
    QPainter histPainter(histImage);
    histPainter.setPen(QPen(Qt::black,1,Qt::SolidLine));
    //draw the frame.
    histPainter.drawLine(originP,originP + QPoint(xaxisLength,0));
    histPainter.drawLine(originP,originP - QPoint(0,yaxisLength));
    histPainter.drawLine(originP - QPoint(0,yaxisLength),
                         originP + QPoint(xaxisLength,-yaxisLength));
    histPainter.drawLine(originP + QPoint(xaxisLength,0),
                         originP + QPoint(xaxisLength,-yaxisLength));
    for (int i=1;i<6;i++)
    {
        histPainter.drawLine(originP + QPoint(50*i, 0),
                             originP + QPoint(50*i, -5));
        histPainter.drawLine(originP + QPoint(50*i, -yaxisLength),
                             originP + QPoint(50*i, -yaxisLength+5));
        histPainter.drawLine(originP + QPoint(0, -50*i),
                             originP + QPoint(5, -50*i));
        histPainter.drawLine(originP + QPoint(xaxisLength, -50*i),
                             originP + QPoint(xaxisLength-5, -50*i));
        histPainter.drawText(originP + QPoint(50*i-10, 20), QString::number(50*i));
    }
    histPainter.drawText(originP + QPoint(0, 20), QString::number(0));
    histPainter.drawText(originP + QPoint(-25, -yaxisLength), QString::number(maxhist));
    //draw the histgram
    histPainter.setPen(QPen(Qt::blue));
    for (int x=0;x<256;x++)
    {
        histPainter.drawLine(originP + QPoint(x,0),
                             originP+QPointF(x,-double(histData.at(x)*yaxisLength)/maxhist));
    }
}

//threshold
void jimbox::threshold(int threValue)
{
    for (int i = 0; i < imheight; i++)
        for (int j = 0; j < imwidth;j++)
        {
            if (grayMat(i,j)>=threValue)
                binaryMat(i,j) = fg;
            else binaryMat(i,j) = bg;
        }
}

//threshold on histgram
void jimbox::HistwithThre()
{
    int threValue = ui->threSlider->value();
    QPixmap histPixmap = QPixmap::fromImage(*histImage);
    QPainter painter(&histPixmap);
    painter.setPen(QPen(Qt::red,1,Qt::SolidLine));
    painter.drawLine(originPx+threValue,originPy,
                     originPx+threValue,originPy-yaxisLength);
    ui->rtLabel->setPixmap(histPixmap);
}

//threshold interact
void jimbox::on_threSlider_valueChanged(int threValue)
{
    threshold(threValue);
    HistwithThre();
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(binaryMat*255)));
}

//otsu threshold
void jimbox::on_otsuButton_clicked()
{
    double P1[256];
    double m[256],mG,varB[256],maxvarB=0,kssum=0;
    int ostuThre,ksNum=0;
    P1[0]=nhistData[0];
    m[0]=0;
    for (int i=1;i<256;i++)
    {
        P1[i]=P1[i-1]+nhistData[i];
        m[i]=m[i-1]+i*nhistData[i];
    }
    P1[255]=1;
    mG = m[255];
    for (int k=0;k<256;k++)
    {
        varB[k] = pow(mG*P1[k]-m[k],2)/(P1[k]*(1-P1[k]));
        if (varB[k]>maxvarB) maxvarB=varB[k];
    }
    for (int k=0;k<256;k++)
    {
        if (varB[k]==maxvarB)
        {
            kssum=kssum + k;
            ksNum=ksNum + 1;
        }
    }
    ostuThre = kssum/ksNum+0.5;
    ui->threSlider->setValue(ostuThre);
}

//entropy threshold
void jimbox::on_entropyButton_clicked()
{
    double P1[256],H[256],Hb,Hw,maxH=0,kssum=0;
    int entropyThre,ksNum=0;
    int kmin=0,kmax=254;
    P1[0]=nhistData[0];
    if (P1[0]==0) kmin=1;
    else
        if (P1[0]==1) kmax =-1;
    for (int i=1;i<256;i++)
    {
        P1[i]=P1[i-1]+nhistData[i];
        if (P1[i]==0) kmin=i+1;
        else
            if (P1[i]==1 && i<kmax) kmax = i-1;
    }
    for (int k=0;k<256;k++)
    {
        if (k<kmin && k>kmax)
            H[k]=0;
        else
        {
        Hb=0;
        Hw=0;
        for (int i=0;i<=k;i++)
            if (nhistData[i] != 0)
            Hb = Hb - nhistData[i]/P1[k] * log(nhistData[i]/P1[k]);
        for (int i=k+1;i<256;i++)
            if (nhistData[i] != 0)
            Hw = Hw - nhistData[i]/(1-P1[k]) * log(nhistData[i]/(1-P1[k]));
        H[k] = Hb+Hw;
        }
        if (maxH<H[k]) maxH=H[k];
    }
    for (int k=0;k<256;k++)
    {
        if (H[k]==maxH)
        {
            kssum=kssum + k;
            ksNum=ksNum + 1;
        }
    }
    entropyThre = kssum/ksNum+0.5;
    ui->threSlider->setValue(entropyThre);
}

//save images
void jimbox::on_saveButton_clicked()
{
    QString fileName = ui->saveChoice->currentText() + ui->saveFormat->currentText();
    QImage saveImage;
    switch (ui->saveChoice->currentIndex())
    {
    case 0 : saveImage = *sourceImage;break;
    case 1 : saveImage = *histImage;break;
    case 2 : saveImage = mat2im(grayMat);break;
    case 3 : saveImage = mat2im(binaryMat*255);break;
    }
    if (saveImage.save(fileName)) ui->saveTip->setText("Save successfully!");
    else ui->saveTip->setText("Save fail!");
}

//inverse binary
void jimbox::on_inverseCheck_stateChanged(int arg1)
{
    bg = arg1;
    fg = 1-bg;
    if (imwidth!=0)
    {
        threshold(ui->threSlider->value());
        ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(binaryMat*255)));
    }
}
