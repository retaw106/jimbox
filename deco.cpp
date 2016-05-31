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
    for (int i=0;i<seMat.rows();i++)
        for (int j=0;j<seMat.cols();j++)
        {
            if ( ui->SEEdit->item(i,j)==NULL ) seMat(i,j)=0;
            else seMat(i,j)=ui->SEEdit->item(i,j)->text().toInt();
        }
    //draw SE image
    drawSE();
    sO1 = ui->seO1->value()-1;
    sO2 = ui->seO2->value()-1;
    drawSEorigin(sO1,sO2);
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
MatrixXi jimbox::morphoOpe(int imtype,int morphotype,MatrixXi inputmat)
{
    MatrixXi outputmat;
    if (imtype==0)
    {
        switch (morphotype) {
        case 0:
            outputmat.resize(inputmat.rows()+seMat.rows()-1,
                             inputmat.cols()+seMat.cols()-1);

            break;
        case 1:
           break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
        }
    }
    else
    {
        switch (morphotype) {
        case 0:

            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
        }
    }
    return outputmat;
}

//convert matrix to qimage
QImage jimbox::mat2im(MatrixXi mat)
{
    QImage im(mat.cols(),mat.rows(),QImage::Format_Grayscale8);
    unsigned char *imBits;
    int imrealwidth;
    imBits = im.bits();
    imrealwidth = im.bytesPerLine();
    for (int i=0;i<mat.rows();i++)
        for (int j=0;j<mat.cols();j++)
        {
            *(imBits+i*imrealwidth+j) = mat(i,j);
        }
    return im;
}

//convert qimage to matrix
MatrixXi jimbox::im2mat(QImage im)
{
    MatrixXi mat;
    unsigned char *imBits;
    int imrealwidth;
    imBits = im.bits();
    imrealwidth = im.bytesPerLine();
    for (int i=0;i<mat.rows();i++)
        for (int j=0;j<mat.cols();j++)
        {
            mat(i,j) = *(imBits+i*imrealwidth+j);
        }
    return mat;
}

//morphology operation on original image
void jimbox::on_decoButton_1_clicked()
{
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(grayMat)));
}

void jimbox::on_SEEdit_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    ui->seO1->setValue(currentRow+1);
    ui->seO2->setValue(currentColumn+1);
}

void jimbox::on_seO1_valueChanged(int arg1)
{
    ui->SEEdit->setCurrentCell(arg1-1,ui->seO2->value()-1);
}

void jimbox::on_seO2_valueChanged(int arg1)
{
    ui->SEEdit->setCurrentCell(ui->seO1->value()-1,arg1-1);
}
