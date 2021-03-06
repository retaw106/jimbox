//interface operation, basic image action
#include "jimbox.h"
#include "ui_jimbox.h"

//some qt widgets' property
jimbox::jimbox(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::jimbox)
{
    ui->setupUi(this);
    //caf
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
    decoGroup = new QButtonGroup;
    decoGroup->addButton(ui->decoRadio_1,0);
    decoGroup->addButton(ui->decoRadio_2,1);
    decoGroup->addButton(ui->decoRadio_3,2);
    decoGroup->addButton(ui->decoRadio_4,3);
    QDoubleValidator sigmaVali(0.001,INFINITY,3,this);
    ui->sigmaEdit->setValidator(&sigmaVali);
    sigmaVali.setBottom(-INFINITY);
    ui->kvalueEdit->setValidator(&sigmaVali);
    //deco
    ui->SEEdit->resizeColumnsToContents();
    ui->SEEdit->setCurrentCell(ui->seO1->value()-1,ui->seO2->value()-1);
}

jimbox::~jimbox()
{
    delete ui;
}

//when tab changes
void jimbox::on_tabWidget_currentChanged(int index)
{
    if (imwidth!=0) //when image exists
        initialDis();
    else    //only change image label when image not loaded
    {
    switch (index) {
    case 0: //histagram and threshold
        ui->ltText->setText("source image");
        ui->lbText->setText("gray image");
        ui->rtText->setText("histogram");
        ui->rbText->setText("binary image");
        break;
    case 1: //convolution and filters
        ui->ltText->setText("source image");
        ui->lbText->setText("gray image");
        ui->rtText->setText("filter");
        ui->rbText->setText("filtered image");
        break;
    case 2: //dilation,erosion,closing,opening
        ui->ltText->setText("source image");
        ui->lbText->setText("gray/binary image");
        ui->rtText->setText("SE");
        ui->rbText->setText("current result");
        break;
    case 3: //morphological functions
        ui->ltText->setText("binary/gray image");
        ui->lbText->setText("");
        ui->rtText->setText("");
        ui->rbText->setText("");
        break;
    default:
        break;
    }
    }
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
    ui->decoButton_1->setEnabled(1);
    ui->decoButton_2->setEnabled(1);
    if (ui->mimType->currentIndex()==0)
    {
        ui->dtButton->setEnabled(1);
        ui->skeButton->setEnabled(1);
        ui->skereButton->setEnabled(1);
        ui->edgedetectButton->setEnabled(1);
        ui->markerCheck->setEnabled(1);
        if (ui->markerCheck->isChecked()) ui->markerSlider->setEnabled(1);
        ui->condilationButton->setEnabled(1);
    }
    else
    {
        ui->gradientButton->setEnabled(1);
        ui->obrButton->setEnabled(1);
        ui->cbrButton->setEnabled(1);
    }
}

//initialize display
void jimbox::initialDis()
{
    resizeimLabel();
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        HistwithThre();
        threshold(ui->threSlider->value());
        ui->ltText->setText("source image");
        ui->lbText->setText("gray image");
        ui->rtText->setText("histogram");
        ui->rbText->setText("binary image");
        ui->ltLabel->setPixmap(QPixmap::fromImage(*sourceImage));
        ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im(grayMat)));
        ui->rbLabel->setPixmap(QPixmap::fromImage(mat2im(binaryMat*255)));
        break;
    case 1:
        ui->ltText->setText("source image");
        ui->lbText->setText("gray image");
        ui->rtText->setText("filter");
        ui->rbText->setText("filtered image");
        ui->kernelLabel->setPixmap(QPixmap::fromImage(imKernel.getkimage()));
        ui->ltLabel->setPixmap(QPixmap::fromImage(*sourceImage));
        ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im(grayMat)));
        ui->rtLabel->setPixmap(QPixmap::fromImage(imKernel.getopeimage(ui->filterChoice->currentIndex())));
        ui->rbLabel->clear();
        break;
    case 2: //dilation,erosion,closing,opening
        ui->ltText->setText("source image");
        ui->lbText->setText("gray/binary image");
        ui->rtText->setText("SE");
        ui->rbText->setText("current result");
        ui->ltLabel->setPixmap(QPixmap::fromImage(*sourceImage));
        ui->rtLabel->clear();
        ui->rbLabel->clear();
        if (ui->DimType->currentIndex()==0)
        {
            threshold(ui->threSlider->value());
            ui->lbLabel->setPixmap(QPixmap::fromImage(mat2im(binaryMat*255)));
        }
        else ui->lbLabel->setPixmap((QPixmap::fromImage(mat2im(grayMat))));
        break;
    case 3: //morphological functions
        ui->ltText->setText("binary/gray image");
        ui->lbText->setText("");
        ui->rtText->setText("");
        ui->rbText->setText("");
        threshold(ui->threSlider->value());
        if (ui->mimType->currentIndex()==0)
            ui->ltLabel->setPixmap(QPixmap::fromImage(mat2im(binaryMat*255)));
        else ui->ltLabel->setPixmap(QPixmap::fromImage(mat2im(grayMat)));
        ui->lbLabel->clear();
        ui->rtLabel->clear();
        ui->rbLabel->clear();
        break;
    default:
        break;
    }
}

//initialize image info
void jimbox::initialIminfo()
{
    //image width and height
    imwidth = sourceImage->width();
    imheight = sourceImage->height();
    ui->markerSlider->setMaximum(imwidth-1);
    rgbrealwidth = sourceImage->bytesPerLine();
    sourceBits = sourceImage->bits();
    //initialize gray image
    /*grayImage = new QImage(imwidth, imheight, QImage::Format_Grayscale8);
    grayBits = grayImage->bits();
    grayrealwidth = grayImage->bytesPerLine();*/
    grayMat.resize(imheight,imwidth);
    histData.fill(0,256);
    nhistData.fill(0,256);
    getGray();
    //initailize histagram
    histImage = new QImage(300,300,QImage::Format_RGB32);
    histImage->fill(Qt::white);
    drawHist();
    binaryMat.resize(imheight,imwidth);
    //initailize binary image
    //binaryImage = new QImage(imwidth,imheight,QImage::Format_Grayscale8);
    //binaryBits = binaryImage->bits();
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
            int gray = (int)(R*0.299 + G*0.587 + B * 0.114);
            histData[gray]=histData[gray]+1;
            grayMat(i,j) = gray;
        }
    for (int i=0;i<256;i++)
        nhistData[i]=double(histData[i])/(imwidth*imheight);
}

//resize image when window size changes
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
        ui->rtLabel->setGeometry(0,(frameH-frameW*imheight/imwidth)/2,
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
        ui->rtLabel->setGeometry((frameW-frameH*imwidth/imheight)/2,0,
                                 frameH*imwidth/imheight,frameH);
        ui->rbLabel->setGeometry((frameW-frameH*imwidth/imheight)/2,0,
                                 frameH*imwidth/imheight,frameH);
    }
}

void jimbox::resizeEvent(QResizeEvent *event)
{
    if (imwidth != 0) resizeimLabel();
}

//convert matrix to qimage
QImage jimbox::mat2im(ArrayXXi mat)
{
    QImage im(mat.cols(),mat.rows(),QImage::Format_Grayscale8);
    unsigned char *imBits;
    int imrealwidth;
    imBits = im.bits();
    imrealwidth = im.bytesPerLine();
    for (int i=0;i<mat.rows();i++)
        for (int j=0;j<mat.cols();j++)
        {
            if (mat(i,j)>255) *(imBits+i*imrealwidth+j) = 255;
            else if (mat(i,j)<0) *(imBits+i*imrealwidth+j) = 0;
            else *(imBits+i*imrealwidth+j) = mat(i,j);
        }
    return im;
}

//convert qimage to matrix
ArrayXXi jimbox::im2mat(QImage im)
{
    ArrayXXi mat(im.height(),im.width());
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
