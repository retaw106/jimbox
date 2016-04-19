#include "jkernel.h"

//default kernel
JKernel::JKernel()
{
    type = 0;
    num[0]=0;num[1]=0;num[2]=0;
    num[3]=0;num[4]=0;num[5]=0;
    num[6]=0;num[7]=0;num[8]=0;
}

//set kernel types
//0Manual,1Roberts,2Prewitt,3Sobel,4Gaussian,5Median
void JKernel::settype(int t)
{
    type = t;
    double sigma2,n0,n1,n2;
    switch (type) {
    case 0:
        break;
    case 11:
        num[0]=-1;num[1]=0;num[2]=0;
        num[3]=0;num[4]=1;num[5]=0;
        num[6]=0;num[7]=0;num[8]=0;
        break;
    case 10:
        num[0]=0;num[1]=-1;num[2]=0;
        num[3]=1;num[4]=0;num[5]=0;
        num[6]=0;num[7]=0;num[8]=0;
        break;
    case 21:
        num[0]=-1;num[1]=0;num[2]=1;
        num[3]=-1;num[4]=0;num[5]=1;
        num[6]=-1;num[7]=0;num[8]=1;
        break;
    case 20:
        num[0]=-1;num[1]=-1;num[2]=-1;
        num[3]=0;num[4]=0;num[5]=0;
        num[6]=1;num[7]=1;num[8]=1;
        break;
    case 31:
        num[0]=-1;num[1]=0;num[2]=1;
        num[3]=-2;num[4]=0;num[5]=2;
        num[6]=-1;num[7]=0;num[8]=1;
        break;
    case 30:
        num[0]=-1;num[1]=-2;num[2]=-1;
        num[3]=0;num[4]=0;num[5]=0;
        num[6]=1;num[7]=2;num[8]=1;
        break;
    case 4:
        if (sigma!=0)
        {
        sigma2=sigma*sigma;
        n0 = 1/(2*3.1415926*sigma2);
        n1 = n0*exp(-0.5/sigma2);
        n2 = n0*exp(-1/sigma2);
        num[0]=n2;num[1]=n1;num[2]=n2;
        num[3]=n1;num[4]=n0;num[5]=n1;
        num[6]=n2;num[7]=n1;num[8]=n2;
        }
        break;
    case 5:
        break;
    default:
        break;
    }
}

//manual set kernel
void JKernel::setvalue(int index, double value)
{
    num[index] = value;
}

//draw the image of the kernel
QImage JKernel::getkimage()
{
    //设定图像变长，边距，格子边长
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
    return kimage;
}

//draw the image of the operation filter
QImage JKernel::getopeimage(int ope)
{
    double num1[9];
    if (ope==0)
        for (int i=0;i<9;i++) num1[i]=num[i];
    else
    {
        if (type==10||type==11)
        {
            num1[0]=num[4];num1[1]=num[3];num1[2]=num[2];
            num1[3]=num[1];num1[4]=num[0];num1[5]=num[5];
            num1[6]=num[6];num1[7]=num[7];num1[8]=num[8];
        }
        else
        for (int i=0;i<9;i++) num1[i]=num[8-i];
    }
    //设定图像变长，边距，格子边长
    int klen = 400, bj=50, klen2=100;
    QImage opeimage(klen,klen,QImage::Format_RGB32);
    opeimage.fill(Qt::white);
    QPainter kPainter(&opeimage);
    kPainter.setPen(QPen(Qt::black,4,Qt::SolidLine));
    for (int i=0;i<4;i++)
    {
    kPainter.drawLine(bj+i*klen2,bj,bj+i*klen2,klen-bj);
    kPainter.drawLine(bj,bj+i*klen2,klen-bj,bj+i*klen2);
    }
    QColor color;
    if (type!=5)
    {
        for (int i=0;i<9;i++)
        {
            if (num1[i]>=0)
                color.setHsv(0,atan(num1[i])*510/3.1416,255);
            else
                color.setHsv(240,-atan(num1[i])*510/3.1416,255);
            kPainter.fillRect(bj+(i-i/3*3)*klen2,bj+i/3*klen2,klen2,klen2,color);
        }
    }
    switch (type) {
    case 0:
    case 4:
        kPainter.setFont(QFont("Arial",15));
        for (int i=0;i<9;i++)
        kPainter.drawText(bj+(i*7-i/3*3*7+1)*klen2/7,bj+(i/3*3+2)*klen2/3,
                          QString::number(num1[i],'e',1));
        break;
    case 5:
        break;
    default:
        kPainter.setFont(QFont("Arial",40));
        for (int i=0;i<9;i++)
        kPainter.drawText(bj+(i*3-i/3*3*3+1)*klen2/3,bj+(i/3*3+2)*klen2/3,
                          QString::number(num1[i]));
        break;
    }
    return opeimage;
}

//calculate the convolution of input image
QImage JKernel::getresultim(QImage originim,int ope)
{
    double num1[9];
    if (ope==0)
        for (int i=0;i<9;i++) num1[i]=num[i];
    else
    {
        if (type==10||type==11)
        {
            num1[0]=num[4];num1[1]=num[3];num1[2]=num[2];
            num1[3]=num[1];num1[4]=num[0];num1[5]=num[5];
            num1[6]=num[6];num1[7]=num[7];num1[8]=num[8];
        }
        else
        for (int i=0;i<9;i++) num1[i]=num[8-i];
    }
    QImage resultim;
    resultim = originim;
    //originBits,resultBits,realwidth,width,height
    unsigned char *oB, *rB;
    int rw, w, h, pix;
    oB = originim.bits();
    rB = resultim.bits();
    rw = originim.bytesPerLine();
    w = originim.width();
    h = originim.height();
    double temp;
    if (type!=5)
    {
    for (int i=1;i<h-1;i++)
        for (int j=1;j<w-1;j++)
        {
            pix=double(*(oB+(i-1)*rw+j-1))*num1[0]+
                    double(*(oB+(i-1)*rw+j))*num1[1]+
                    double(*(oB+(i-1)*rw+j+1))*num1[2]+
                    double(*(oB+(i)*rw+j-1))*num1[3]+
                    double(*(oB+(i)*rw+j))*num1[4]+
                    double(*(oB+(i)*rw+j+1))*num1[5]+
                    double(*(oB+(i+1)*rw+j-1))*num1[6]+
                    double(*(oB+(i+1)*rw+j))*num1[7]+
                    double(*(oB+(i+1)*rw+j+1))*num1[8];
            if (pix>255) pix=255;
            else if (pix<0) pix=0;
            *(rB+i*rw+j)=pix;
        }
    }
    else
    {
        for (int i=1;i<h-1;i++)
            for (int j=1;j<w-1;j++)
            {
                double num2[9]={double(*(oB+(i-1)*rw+j-1)),
                        double(*(oB+(i-1)*rw+j)),
                        double(*(oB+(i-1)*rw+j+1)),
                        double(*(oB+(i)*rw+j-1)),
                        double(*(oB+(i)*rw+j)),
                        double(*(oB+(i)*rw+j+1)),
                        double(*(oB+(i+1)*rw+j-1)),
                        double(*(oB+(i+1)*rw+j)),
                        double(*(oB+(i+1)*rw+j+1))};
                for (int k1=0;k1<5;k1++)
                {
                    for (int k2=0;k2<7-k1;k2++)
                    {
                        if (num2[k2]>num2[k2+1])
                        {
                            temp = num2[k2];
                            num2[k2] = num2[k2+1];
                            num2[k2+1]=temp;
                        }
                    }
                }
                *(rB+i*rw+j)=num2[4];
            }
    }
    return resultim;
}
