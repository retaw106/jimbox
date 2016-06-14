//tab: dilation,erosion,closing,opening
#include "jimbox.h"
#include "ui_jimbox.h"

void jimbox::on_SErow_valueChanged(int arg1)
{
    ui->SEEdit->setRowCount(arg1);
    ui->SEEdit->resizeColumnsToContents();
}

void jimbox::on_SEcolumn_valueChanged(int arg1)
{
    ui->SEEdit->setColumnCount(arg1);
    ui->SEEdit->resizeColumnsToContents();
}

//get SE to a matrix
void jimbox::on_getseButton_clicked()
{
    //get seMat
    seMat.resize(ui->SErow->value(),ui->SEcolumn->value());
    sexMat.resize(ui->SErow->value(),ui->SEcolumn->value());
    bool ok; double d;
    for (int i=0;i<seMat.rows();i++)
        for (int j=0;j<seMat.cols();j++)
        {
            if ( ui->SEEdit->item(i,j)==NULL )
            {
                seMat(i,j)=0;
                sexMat(i,j)=1;
            }
            else
            {
                d = ui->SEEdit->item(i,j)->text().toDouble(&ok);
                if (ok == false)
                {
                    seMat(i,j)=0;
                    sexMat(i,j)=1;
                }
                else
                {
                    seMat(i,j)=d;
                    sexMat(i,j)=0;
                }
            }
        }
    //draw SE image
    drawSE();
    drawSEorigin(ui->SEEdit->currentRow(),ui->SEEdit->currentColumn());
    ui->rtLabel->setPixmap(QPixmap::fromImage(*seOImage));
}

//draw SE image
void jimbox::drawSE()
{
    //设定图像边距，格子边长,image size
    int bj=10, len=20;
    int h = bj*2+len*seMat.rows();
    int w = bj*2+len*seMat.cols();
    seImage = new QImage(w,h,QImage::Format_Grayscale8);
    seImage->fill(Qt::white);
    QPainter Painter(seImage);
    Painter.setPen(QPen(Qt::black,1,Qt::SolidLine));
    Painter.setFont(QFont("Arial",10));
    QRect gezi(0,0,len,len);
    for (int i=0;i<seMat.rows();i++)
        for (int j=0;j<seMat.cols();j++)
        {
            gezi.moveTo(bj+len*j,bj+len*i);
            Painter.drawRect(gezi);
            if (sexMat(i,j)==1)
                Painter.drawText(gezi,Qt::AlignCenter,"*");
            else
                Painter.drawText(gezi,Qt::AlignCenter,
                             QString::number(seMat(i,j)));
        }
}

//draw a circle on SE origin
void jimbox::drawSEorigin(int i, int j)
{
    int bj2 = 2;
    int bj=10+bj2, len=20;
    seOImage = new QImage(*seImage);
    QPainter painter(seOImage);
    painter.setPen(QPen(Qt::black,1,Qt::SolidLine));
    painter.drawEllipse(bj+len*j,bj+len*i,len-bj2*2,len-bj2*2);
}

//basic morphology operations
//imtype:0binary,1gray.morphotype:0dilation,1erosion,2closing,3opening
ArrayXXi jimbox::morphoOpe(int imtype,int morphotype,ArrayXXi inputmat,ArrayXXi SE,int sO1,int sO2)
{
    ArrayXXi outputmat;
    ArrayXXi tmpmat;
    ArrayXXi output1mat;
    int ir=inputmat.rows(), ic=inputmat.cols();
    int sr=SE.rows(), sc=SE.cols();
    //int sO1=ui->SEEdit->currentRow(),sO2=ui->SEEdit->currentColumn();
    if (imtype==0)
    {
        switch (morphotype) {
        case 0:
            outputmat = ArrayXXi::Zero(ir,ic);
            tmpmat = ArrayXXi::Zero(ir+sr-1,ic+sc-1);
            tmpmat.block(sr-1-sO1,sc-1-sO2,ir,ic)=inputmat;
            for (int i=0;i<sr;i++)
                for (int j=0;j<sc;j++)
                {
                    if (SE(i,j)>0)
                    {
                        outputmat += tmpmat.block(sr-1-i,sc-1-j,ir,ic);
                    }
                }
            for (int i=0;i<ir;i++)
                for (int j=0;j<ic;j++)
                {
                    outputmat(i,j) = (outputmat(i,j)>0);
                }
            break;
        case 1:
            outputmat = ArrayXXi::Ones(ir,ic);
            tmpmat = ArrayXXi::Zero(ir+sr-1,ic+sc-1);
            tmpmat.block(sO1,sO2,ir,ic)=inputmat;
            for (int i=0;i<sr;i++)
                for (int j=0;j<sc;j++)
                {
                    if (SE(i,j)>0)
                    {
                        outputmat *= tmpmat.block(i,j,ir,ic);
                    }
                }
           break;
        case 2:
            tmpmat = morphoOpe(0,0,inputmat,SE,sO1,sO2);
            outputmat = morphoOpe(0,1,tmpmat,SE,sO1,sO2);
            break;
        case 3:
            tmpmat = morphoOpe(0,1,inputmat,SE,sO1,sO2);
            outputmat = morphoOpe(0,0,tmpmat,SE,sO1,sO2);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (morphotype) {
        case 0:
            outputmat = ArrayXXi::Zero(ir,ic);
            tmpmat = ArrayXXi::Constant(ir+sr-1,ic+sc-1,-9999);
            tmpmat.block(sr-1-sO1,sc-1-sO2,ir,ic)=inputmat;
            for (int i=0;i<ir;i++)
                for (int j=0;j<ic;j++)
                {
                    outputmat(i,j)=(tmpmat.block(i,j,sr,sc)+SE.reverse()).maxCoeff();
                }
            break;
        case 1:
            outputmat = ArrayXXi::Constant(ir,ic,255);
            tmpmat = ArrayXXi::Constant(ir+sr-1,ic+sc-1,-9999);
            tmpmat.block(sr-1-sO1,sc-1-sO2,ir,ic)=inputmat;
            for (int i=0;i<ir;i++)
                for (int j=0;j<ic;j++)
                {
                    outputmat(i,j)=(tmpmat.block(i,j,sr,sc)-SE.reverse()).minCoeff();
                }
            break;
        case 2:
            tmpmat = morphoOpe(1,0,inputmat,SE,sO1,sO2);
            outputmat = morphoOpe(1,1,tmpmat,SE,sO1,sO2);
            break;
        case 3:
            tmpmat = morphoOpe(1,1,inputmat,SE,sO1,sO2);
            outputmat = morphoOpe(1,0,tmpmat,SE,sO1,sO2);
            break;
        default:
            break;
        }
    }
    return outputmat;
}

ArrayXXi jimbox::morphoOpe(int morphotype,ArrayXXi inputmat,ArrayXXi SE, ArrayXXi SEx,int sO1,int sO2)
{
    ArrayXXi outputmat;
    ArrayXXi tmpmat;
    int ir=inputmat.rows(), ic=inputmat.cols();
    int sr=SE.rows(), sc=SE.cols();
    for (int i=0;i<sr;i++)
        for (int j=0;j<sc;j++)
        {
            if (SEx(i,j)==1) SE(i,j)=-9999;
        }
    //int sO1=ui->SEEdit->currentRow(),sO2=ui->SEEdit->currentColumn();
    switch (morphotype) {
    case 0:
        outputmat = ArrayXXi::Zero(ir,ic);
        tmpmat = ArrayXXi::Zero(ir+sr-1,ic+sc-1);
        tmpmat.block(sr-1-sO1,sc-1-sO2,ir,ic)=inputmat;
        for (int i=0;i<ir;i++)
            for (int j=0;j<ic;j++)
            {
                outputmat(i,j)=(tmpmat.block(i,j,sr,sc)+SE.reverse()).maxCoeff();
            }
        break;
    case 1:
        outputmat = ArrayXXi::Constant(ir,ic,255);
        tmpmat = ArrayXXi::Zero(ir+sr-1,ic+sc-1);
        tmpmat.block(sr-1-sO1,sc-1-sO2,ir,ic)=inputmat;
        for (int i=0;i<ir;i++)
            for (int j=0;j<ic;j++)
            {
                outputmat(i,j)=(tmpmat.block(i,j,sr,sc)-SE.reverse()).minCoeff();
            }
        break;
    case 2:
        tmpmat = morphoOpe(0,inputmat,SE,SEx,sO1,sO2);
        outputmat = morphoOpe(1,tmpmat,SE,SEx,sO1,sO2);
        break;
    case 3:
        tmpmat = morphoOpe(1,inputmat,SE,SEx,sO1,sO2);
        outputmat = morphoOpe(0,tmpmat,SE,SEx,sO1,sO2);
        break;
    default:
        break;
    }
    return outputmat;
}

//morphology operation on original image
void jimbox::on_decoButton_1_clicked()
{
    on_getseButton_clicked();
    if (ui->DimType->currentIndex()==0)
    {
        decoresultMat = morphoOpe(0,decoGroup->checkedId(),binaryMat,seMat,
                                  ui->SEEdit->currentRow(),ui->SEEdit->currentColumn());
        ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(decoresultMat*255)));
    }
    else
    {
        decoresultMat = morphoOpe(decoGroup->checkedId(),grayMat,seMat,sexMat,
                                  ui->SEEdit->currentRow(),ui->SEEdit->currentColumn());
        ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(decoresultMat)));
    }
}

//morphology operation on current result image
void jimbox::on_decoButton_2_clicked()
{
    on_getseButton_clicked();
    if (ui->DimType->currentIndex()==0)
    {
        decoresultMat = morphoOpe(0,decoGroup->checkedId(),decoresultMat,seMat,
                                  ui->SEEdit->currentRow(),ui->SEEdit->currentColumn());
        ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(decoresultMat*255)));
    }
    else
    {
        decoresultMat = morphoOpe(decoGroup->checkedId(),decoresultMat,seMat,sexMat,
                                  ui->SEEdit->currentRow(),ui->SEEdit->currentColumn());
        ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(decoresultMat)));
    }
}

void jimbox::on_SEEdit_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    ui->seO1->setValue(currentRow+1);
    ui->seO2->setValue(currentColumn+1);
    if (imwidth!=0)
    if (!ui->rtLabel->pixmap()) on_getseButton_clicked();
    else
    {
        drawSEorigin(currentRow,currentColumn);
        ui->rtLabel->setPixmap(QPixmap::fromImage(*seOImage));
    }
}

void jimbox::on_seO1_valueChanged(int arg1)
{
    ui->SEEdit->setCurrentCell(arg1-1,ui->seO2->value()-1);
}

void jimbox::on_seO2_valueChanged(int arg1)
{
    ui->SEEdit->setCurrentCell(ui->seO1->value()-1,arg1-1);
}

void jimbox::on_DimType_currentIndexChanged(int index)
{
    if (imwidth!=0)
    {
    if (index==0)
    {
        threshold(ui->threSlider->value());
        ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im(binaryMat*255)));
    }
    else ui->lbLabel->setPixmap((QPixmap::fromImage(mat2im(grayMat))));
    }
}
