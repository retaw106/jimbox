#include "jimbox.h"
#include "ui_jimbox.h"

jimbox::jimbox(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::jimbox)
{
    ui->setupUi(this);
    fbGroup = new QButtonGroup;
    fbGroup->addButton(ui->manualfilButton,0);
    fbGroup->addButton(ui->robertsxButton,10);
    fbGroup->addButton(ui->robertsyButton,11);
    fbGroup->addButton(ui->prewittxButton,20);
    fbGroup->addButton(ui->prewittyButton,21);
    fbGroup->addButton(ui->sobelxButton,30);
    fbGroup->addButton(ui->sobelyButton,31);
    fbGroup->addButton(ui->gaussianButton,4);
    fbGroup->addButton(ui->medianButton,5);
    connect(fbGroup,SIGNAL(buttonClicked(int)),this,SLOT(changefilter(int)));
    QDoubleValidator sigmaVali(0.001,INFINITY,3,this);
    ui->sigmaEdit->setValidator(&sigmaVali);
    sigmaVali.setBottom(-INFINITY);
    ui->kvalueEdit->setValidator(&sigmaVali);
}

jimbox::~jimbox()
{
    delete ui;
}

//open file, initial data, display images.
void jimbox::on_action_Open_triggered()
{
    //read image.
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
            tr("Image Files (*.bmp *.jpg *.tif *.gif *.png)"));
    QImage readFile;
    if (!fileName.isEmpty()){
        if (!readFile.load(fileName)){
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
    }
    else return;
    //display source image, get the size information.
    sourceImage = new QImage;
    *sourceImage = readFile.convertToFormat(QImage::Format_RGB32);
    initialIminfo();
    //initial display
    initialDis();
    ui->thregroupBox->setEnabled(1);
    ui->savegroupBox->setEnabled(1);
    ui->filterButton->setEnabled(1);
}

//initialize display
void jimbox::initialDis()
{
    resizeimLabel();
    ui->ltLabel->setPixmap(QPixmap::fromImage(*sourceImage));
    ui->lbLabel->setPixmap(QPixmap::fromImage(*grayImage));
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        HistwithThre();
        binaryImage = new QImage(imwidth,imheight,QImage::Format_Indexed8);
        binaryBits = binaryImage->bits();
        threshold(ui->threSlider->value());
        ui->rtText->setText("histogram");
        ui->rbText->setText("binary image");
        ui->rbLabel->setPixmap(QPixmap::fromImage(*binaryImage));
        break;
    case 1:
        ui->rtText->setText("filter");
        ui->rbText->setText("filtered image");
        ui->kernelLabel->setPixmap(QPixmap::fromImage(imKernel.getkimage()));
        ui->rtLabel->setPixmap(QPixmap::fromImage(imKernel.getopeimage(ui->filterChoice->currentIndex())));
        ui->rbLabel->clear();
        break;
    default:
        break;
    }
}

//threshold
void jimbox::threshold(int threValue)
{
    for (int i = 0; i < imheight; i++)
        for (int j = 0; j < imwidth;j++)
        {
            if (*(grayBits+i*grayrealwidth+j)>=threValue)
                *(binaryBits+i*grayrealwidth+j) = fg;
            else *(binaryBits+i*grayrealwidth+j) = bg;
        }
}

//threshold histgram
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

//initialize image info
void jimbox::initialIminfo()
{
    imwidth = sourceImage->width();
    imheight = sourceImage->height();
    rgbrealwidth = sourceImage->bytesPerLine();
    sourceBits = sourceImage->bits();
    grayImage = new QImage(imwidth, imheight, QImage::Format_Indexed8);
    grayBits = grayImage->bits();
    grayrealwidth = grayImage->bytesPerLine();
    histData.fill(0,256);
    nhistData.fill(0,256);
    getGray();
    histImage = new QImage(300,300,QImage::Format_RGB32);
    histImage->fill(Qt::white);
    drawHist();
}

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

//get gray information
void jimbox::getGray()
{
    unsigned char R, G, B;
    for (int i = 0; i < imheight; i++)
        for (int j = 0; j < imwidth;j++)
        {
            R = *(sourceBits + rgbrealwidth*i + 4 * j + 2);
            G = *(sourceBits + rgbrealwidth*i + 4 * j + 1);
            B = *(sourceBits + rgbrealwidth*i + 4 * j);
            *(grayBits+i*grayrealwidth+j) = (uchar)(R*0.299 + G*0.587 + B * 0.114);
            int gray = *(grayBits+i*grayrealwidth+j);
            histData[gray]=histData[gray]+1;
        }
    for (int i=0;i<256;i++)
        nhistData[i]=double(histData[i])/(imwidth*imheight);
}

//resize function
void jimbox::resizeimLabel()
{
    int frameW = ui->ltWidget->width();
    int frameH = ui->ltWidget->height();
    ui->rtLabel->resize(frameW,frameH);
    if (frameW*imheight/imwidth<=frameH)
    {
        ui->ltLabel->setGeometry(0,(frameH-frameW*imheight/imwidth)/2,
                                 frameW,frameW*imheight/imwidth);
        ui->lbLabel->setGeometry(0,(frameH-frameW*imheight/imwidth)/2,
                                 frameW,frameW*imheight/imwidth);
        ui->rbLabel->setGeometry(0,(frameH-frameW*imheight/imwidth)/2,
                                 frameW,frameW*imheight/imwidth);
    }
    else
    {
        ui->ltLabel->setGeometry((frameW-frameH*imwidth/imheight)/2,0,
                                 frameH*imwidth/imheight,frameH);
        ui->lbLabel->setGeometry((frameW-frameH*imwidth/imheight)/2,0,
                                 frameH*imwidth/imheight,frameH);
        ui->rbLabel->setGeometry((frameW-frameH*imwidth/imheight)/2,0,
                                 frameH*imwidth/imheight,frameH);
    }
}

void jimbox::on_threSlider_valueChanged(int threValue)
{
    threshold(threValue);
    HistwithThre();
    ui->rbLabel->setPixmap(QPixmap::fromImage(*binaryImage));
}

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

void jimbox::on_saveButton_clicked()
{
    QString fileName = ui->saveChoice->currentText() + ui->saveFormat->currentText();
    QImage saveImage;
    switch (ui->saveChoice->currentIndex())
    {
    case 0 : saveImage = *sourceImage;break;
    case 1 : saveImage = *histImage;break;
    case 2 : saveImage = grayImage->convertToFormat(QImage::Format_RGB888);break;
    case 3 : saveImage = binaryImage->convertToFormat(QImage::Format_RGB888);break;
    }
    if (saveImage.save(fileName)) ui->saveTip->setText("Save successfully!");
    else ui->saveTip->setText("Save fail!");
}

void jimbox::resizeEvent(QResizeEvent *event)
{
    if (imwidth != 0) resizeimLabel();
}

void jimbox::on_inverseCheck_stateChanged(int arg1)
{
    bg = arg1*255;
    fg = 255-bg;
    if (imwidth!=0)
    {
        threshold(ui->threSlider->value());
        ui->rbLabel->setPixmap(QPixmap::fromImage(*binaryImage));
    }
}

void jimbox::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0:
        if (imwidth!=0)
            initialDis();
        else
        {
            ui->rtText->setText("histogram");
            ui->rbText->setText("binary image");
        }
        break;
    case 1:
        if (imwidth!=0)
            initialDis();
        else
        {
            ui->rtText->setText("histogram");
            ui->rbText->setText("binary image");
        }
        break;
    default:
        break;
    }
}

void jimbox::changefilter(int id)
{
    imKernel.settype(id);
    ui->kernelLabel->setPixmap(QPixmap::fromImage(imKernel.getkimage()));
    ui->rtLabel->setPixmap(QPixmap::fromImage(imKernel.getopeimage(ui->filterChoice->currentIndex())));
}

void jimbox::on_filterButton_clicked()
{
    int index = ui->filterChoice->currentIndex();
    ui->rbLabel->setPixmap(QPixmap::fromImage(imKernel.getresultim(*grayImage,index)));
}

void jimbox::on_sigmaEdit_editingFinished()
{
    imKernel.sigma = ui->sigmaEdit->text().toDouble();
    if (ui->gaussianButton->isChecked())
    {
        imKernel.settype(4);
        ui->kernelLabel->setPixmap((QPixmap::fromImage(imKernel.getkimage())));
        ui->rtLabel->setPixmap(QPixmap::fromImage(imKernel.getopeimage(ui->filterChoice->currentIndex())));
    }
}

void jimbox::on_kvalueEdit_editingFinished()
{
    double val = ui->kvalueEdit->text().toDouble();
    if (ui->manualfilButton->isChecked())
    {
        imKernel.settype(0);
        imKernel.setvalue(ui->kindexBox->currentIndex(),val);
        ui->kernelLabel->setPixmap((QPixmap::fromImage(imKernel.getkimage())));
        ui->rtLabel->setPixmap(QPixmap::fromImage(imKernel.getopeimage(ui->filterChoice->currentIndex())));
    }
}


void jimbox::on_filterChoice_currentIndexChanged(int index)
{
    ui->rtLabel->setPixmap(QPixmap::fromImage(imKernel.getopeimage(index)));
}
