//tab: dilation,erosion,closing,opening
#include "jimbox.h"
#include "ui_jimbox.h"

void jimbox::on_SErow_valueChanged(int arg1)
{
    ui->SEEdit->setRowCount(arg1);
}

void jimbox::on_SEcolumn_valueChanged(int arg1)
{
    ui->SEEdit->setColumnCount(arg1);
}

//get SE to a matrix
void jimbox::on_getseButton_clicked()
{
    seMat.resize(ui->SErow->value(),ui->SEcolumn->value());
    for (int i=0;i<seMat.rows();i++)
        for (int j=0;j<seMat.cols();j++)
        {
            seMat(i,j)=ui->SEEdit->item(i,j)->text()
                    .toInt();
        }
}

//draw SE image
void jimbox::drawSE()
{
    //设定图像边长，边距，格子边长
    int klen = 400, bj=50, klen2=100;
    QImage kimage(klen,klen,QImage::Format_RGB32);
    kimage.fill(Qt::white);
    QPainter kPainter(&kimage);
    kPainter.setPen(QPen(Qt::black,4,Qt::SolidLine));
    for (int i=0;i<4;i++)
    {
    kPainter.drawLine(bj+i*klen2,bj,bj+i*klen2,klen-bj);
    kPainter.drawLine(bj,bj+i*klen2,klen-bj,bj+i*klen2);
    }
    switch (type) {
    case 0:
    case 4:
        kPainter.setFont(QFont("Arial",15));
        for (int i=0;i<9;i++)
        kPainter.drawText(bj+(i*7-i/3*3*7+1)*klen2/7,bj+(i/3*3+2)*klen2/3,
                          QString::number(num[i],'e',1));
        break;
    case 5:
        break;
    default:
        kPainter.setFont(QFont("Arial",40));
        for (int i=0;i<9;i++)
        kPainter.drawText(bj+(i*3-i/3*3*3+1)*klen2/3,bj+(i/3*3+2)*klen2/3,
                          QString::number(num[i]));
        break;
    }
}
