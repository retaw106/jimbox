#ifndef JIMBOX_H
#define JIMBOX_H

#include <QMainWindow>
#include "jkernel.h"
#include <Eigen/Dense>

#include <QImage>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QPainter>
#include <QVector>
#include <QPixmap>
#include <QResizeEvent>
#include <QButtonGroup>
//#include <QTableWidgetItem>

using namespace Eigen;

namespace Ui {
class jimbox;
}

class jimbox : public QMainWindow
{
    Q_OBJECT
public:
    explicit jimbox(QWidget *parent = 0);
    ~jimbox();

private slots:
    void on_action_Open_triggered();

    void on_threSlider_valueChanged(int value);

    void on_otsuButton_clicked();

    void on_entropyButton_clicked();

    void on_saveButton_clicked();

    void on_inverseCheck_stateChanged(int arg1);

    void on_tabWidget_currentChanged(int index);

    void changefilter(int id);

    void on_filterButton_clicked();

    void on_sigmaEdit_editingFinished();

    void on_kvalueEdit_editingFinished();

    void on_filterChoice_currentIndexChanged(int index);

    void on_SErow_valueChanged(int arg1);

    void on_SEcolumn_valueChanged(int arg1);

    void on_getseButton_clicked();

    void on_decoButton_1_clicked();

    void on_SEEdit_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_seO1_valueChanged(int arg1);

    void on_seO2_valueChanged(int arg1);

    void on_DimType_currentIndexChanged(int index);

    void on_decoButton_2_clicked();

    void on_dtButton_clicked();

private:
    Ui::jimbox *ui;

    //image info
    QImage *sourceImage;
    //QImage *grayImage;
    //QImage *binaryImage;
    QImage *histImage;
    QVector<int> histData;
    QVector<double> nhistData;
    ArrayXXi grayMat,binaryMat,histMat;
    int imwidth=0, imheight;
    int rgbrealwidth;//, grayrealwidth;
    unsigned char *sourceBits;//, *grayBits, *binaryBits;

    //buttongroup
    QButtonGroup *fbGroup;
    QButtonGroup *decoGroup;

    //histogram settings
    int bg = 0;
    int fg = 1;
    int originPx = 30;
    int originPy = 270;
    int xaxisLength = 256;
    int yaxisLength = 250;

    //convert matrix to qimage
    QImage mat2im(ArrayXXi mat);
    //convert qimage to matrix
    ArrayXXi im2mat(QImage im);

    //caf
    JKernel imKernel;

    //resize function
    void resizeimLabel();
    void resizeEvent(QResizeEvent* event);
    //initialize image info
    void initialIminfo();
    //get gray information
    void getGray();
    //draw histgram
    void drawHist();
    //initialize display
    void initialDis();
    //threshold histgram
    void HistwithThre();
    //threshold
    void threshold(int value);

    //deco
    //draw SE to an image
    QImage *seImage;
    QImage *seOImage;
    ArrayXXi seMat;
    ArrayXXi decoresultMat;
    void drawSE();
    //draw a circle on SE origin
    void drawSEorigin(int i, int j);
    //basic morphology operations
    //imtype:0binary,1gray.morphotype:0dilation,1erosion,2closing,3opening
    ArrayXXi morphoOpe(int imtype, int morphotype, ArrayXXi inputmat, ArrayXXi SE, int sO1, int sO2);

    //DiS
    //DIsdance transform and Skeleton
    ArrayXXi dtMat;
    //distance transform
    ArrayXXi distrans(ArrayXXi inputmat);
};

#endif // JIMBOX_H
