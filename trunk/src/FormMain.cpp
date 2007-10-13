/**
 * Copyright (c) 2007, Emir Uner
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "FormMain.hpp"
#include "MountInfo.hpp"
#include "TrueCrypt.hpp"
#include "FormMountImage.hpp"
#include "FormCreateImage.hpp"

#include <QtGui/QHeaderView>
#include <QtGui/QMessageBox>
#include <QtCore/QThread>

#include <stdexcept>

namespace
{

QTableWidgetItem* createTableItem(std::string str)
{
    QTableWidgetItem* item = new QTableWidgetItem(str.c_str());
    item->setFlags(Qt::ItemIsEnabled);
            
    return item;
}

class CreateImageFileThread : public QThread
{
    std::string imageFile_;
    std::string password_;
    int size_;
    int result;
    std::string errorMessage;
    
public:
    enum Result
    {
        Success,
        Failure
    };
    
    CreateImageFileThread(std::string imageFile, std::string password, int size)
    : imageFile_(imageFile), password_(password), size_(size)
    {
    }
    
    int getResult()
    {
        return result;
    }
    
    std::string getErrorMessage()
    {
        return errorMessage;
    }
    
    void run()
    {
        try
        {
            ::createImage(imageFile_, password_, size_);
            result = Success;
        }
        catch(std::runtime_error ex)
        {
            result = Failure;
            errorMessage = ex.what();
        }
    }
};
    
} // namespace <unnamed>

FormMain::FormMain(QMainWindow* parent)
: QMainWindow(parent), formPleaseWait(0), imageCreationThread(0)
{
    ui.setupUi(this);
    
    ui.tableMounts->setHorizontalHeaderLabels(QStringList() << "Image File" << "Mount Point");
    ui.tableMounts->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    updateTableMounts();

    if(ui.tableMounts->rowCount() > 1)
    {
        ui.tableMounts->selectRow(1);
    }

    enableDisableButtons();
    
    QObject::connect(ui.tableMounts, SIGNAL(itemSelectionChanged()), this, SLOT(enableDisableButtons()));
    QObject::connect(ui.pushButtonUnmount, SIGNAL(clicked()), this, SLOT(unmount()));
    QObject::connect(ui.pushButtonUnmountAll, SIGNAL(clicked()), this, SLOT(unmountAll()));
    QObject::connect(ui.pushButtonMountImage, SIGNAL(clicked()), this, SLOT(mountImage()));
    QObject::connect(ui.pushButtonCreateImage, SIGNAL(clicked()), this, SLOT(createImage()));
}

void FormMain::updateTableMounts()
{    
    for(int i = 1; i < ui.tableMounts->rowCount(); ++i)
    {
        ui.tableMounts->removeRow(1);
    }
    ui.tableMounts->setRowCount(0);
  
    try
    {
        MountInfoVec miVec = getMountInfo();
        
        for(MountInfoVec::const_iterator it = miVec.begin(); it != miVec.end(); ++it)
        {
            const int row = ui.tableMounts->rowCount();
            ui.tableMounts->insertRow(row);
            
            ui.tableMounts->setItem(row, 0, createTableItem(it->imageFile));
            ui.tableMounts->setItem(row, 1, createTableItem(it->mountPoint));
        }
    }
    catch(std::runtime_error ex)
    { 
        if(std::string(ex.what()).find("No volumes mapped") == std::string::npos)
        {
            QMessageBox::critical(0, "Error!", ex.what());
        }
    }
}

void FormMain::enableDisableButtons()
{
    ui.pushButtonUnmount->setEnabled(ui.tableMounts->currentRow() != -1);
    ui.pushButtonUnmountAll->setEnabled(ui.tableMounts->rowCount() > 0);
}

void FormMain::unmount()
{
    try
    {
        ::unmount(ui.tableMounts->item(ui.tableMounts->currentRow(), 0)->text().toStdString().c_str());
        updateTableMounts();
    }
    catch(std::runtime_error ex)
    { 
        QMessageBox::critical(0, "Error!", ex.what());
    }
}

void FormMain::unmountAll()
{
    try 
    {
        ::unmountAll();
        updateTableMounts();
    }
    catch(std::runtime_error ex)
    { 
        QMessageBox::critical(0, "Error!", ex.what());
    }
}

void FormMain::mountImage()
{
    FormMountImage* formMountImage = new FormMountImage();

    if(formMountImage->exec() == QDialog::Accepted)
    {
        try 
        {
            ::mount(formMountImage->getImageFile(), formMountImage->getMountPoint(), formMountImage->getPassword());
            updateTableMounts();            
        }
        catch(std::runtime_error ex)
        { 
            QMessageBox::critical(0, "Error!", ex.what());
        }
    }
}

void FormMain::createImage()
{
    FormCreateImage* form = new FormCreateImage();

    if(form->exec() == QDialog::Accepted)
    {
        CreateImageFileThread* thread = new CreateImageFileThread(form->getImageFile(), form->getPassword(),
                                                                  form->getImageSize());
        QObject::connect(thread, SIGNAL(finished()), this, SLOT(imageCreated()));

        imageCreationThread = thread;
        thread->start();

        formPleaseWait = new FormPleaseWait();
        formPleaseWait->exec();
    }
}

void FormMain::imageCreated()
{
    if(formPleaseWait != 0)
    {
        CreateImageFileThread* thread = (CreateImageFileThread*) imageCreationThread;

        if(thread->getResult() == CreateImageFileThread::Success)
        {
            formPleaseWait->setMessageAndEnableOkButton("Created the image file.");
        }
        else
        {
            formPleaseWait->setMessageAndEnableOkButton(thread->getErrorMessage());
        }
    }
}
