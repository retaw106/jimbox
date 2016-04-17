#ifndef JIMBOX_H
#define JIMBOX_H

#include <QMainWindow>
#include "jkernel.h"

#include <QImage>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QPainter>
#include <QVector>
#include <QPixmap>
#include <QResizeEvent>
#include <QButtonGroup>

namespace Ui {
class jimbox;
}
class JKernel;
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

private:
    Ui::jimbox *ui;

    //image info
    QImage *sourceImage;
    QImage *grayImage;
    QImage *binaryImage;
    QImage *histImage;
    QVector<int> histData;
    QVector<double> nhistData;
    int imwidth=0, imheight;
    int rgbrealwidth, grayrealwidth;
    unsigned char *sourceBits, *grayBits, *binaryBits;

    //buttongroup
    QButtonGroup *fbGroup;

    //histogram settings
    int bg = 0;
    int fg = 255;
    int originPx = 30;
    int originPy = 270;
    int xaxisLength = 256;
    int yaxisLength = 250;

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
};

#endif // JIMBOX_H
