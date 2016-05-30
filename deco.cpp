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
    ui->rtLabel->setPixmap(QPixmap::fromImage(*seImage));
}

//draw SE image
void jimbox::drawSE()
{
    //设定图像边距，格子边长,image size
    int bj=10, len=20;
    int h = bj*2+len*seMat.rows();
    int w = bj*2+len*seMat.cols();
    seImage = new QImage(w,h,QImage::Format_Indexed8);
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

//basic morphology operations
//imtype:0binary,1gray.morphotype:0dilation,1erosion,2closing,3opening
void jimbox::morphoOpe(int imtype,int morphotype,MatrixXi input)
{
    if (imtype==0)
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
}

//convert matrix to qimage
QImage jimbox::mat2im(MatrixXi mat)
{
    QImage im(mat.cols(),mat.rows(),QImage::Format_Indexed8);
    unsigned char *imBits;
    imBits = im.bits();
    for (int i=0;i<mat.rows();i++)
        for (int j=0;j<mat.cols();j++)
        {
            *(imBits+i*grayrealwidth+j) = (uchar)(mat(i,j));
        }
    return im;
}

//morphology operation on original image
void jimbox::on_decoButton_1_clicked()
{
    ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(grayMat)));
}
