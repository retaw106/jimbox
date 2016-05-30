//tab: convolution and filters
#include "jimbox.h"
#include "ui_jimbox.h"

//slot: change filter (interact)
void jimbox::changefilter(int id)
{
    imKernel.settype(id);
    ui->kernelLabel->setPixmap(QPixmap::fromImage(imKernel.getkimage()));
    ui->rtLabel->setPixmap(QPixmap::fromImage(imKernel.getopeimage(ui->filterChoice->currentIndex())));
}

//do filter
void jimbox::on_filterButton_clicked()
{
    int index = ui->filterChoice->currentIndex();
    ui->rbLabel->setPixmap(QPixmap::fromImage(imKernel.getresultim(*grayImage,index)));
}

//edit sigma in Gaussian model
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

//get kernel values
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

//filterchoice: correlation or convolution
void jimbox::on_filterChoice_currentIndexChanged(int index)
{
    ui->rtLabel->setPixmap(QPixmap::fromImage(imKernel.getopeimage(index)));
}
