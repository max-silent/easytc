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

#include "FormCreateImage.hpp"

#include <QtGui/QFileDialog>
#include <QtGui/QDialogButtonBox>

FormCreateImage::FormCreateImage(QDialog* parent)
: QDialog(parent)
{
    ui.setupUi(this);

    enableDisableButtons();
    
    QObject::connect(ui.commandSelectImageFile, SIGNAL(clicked()), this, SLOT(selectImageFile()));
    QObject::connect(ui.inputImageFile, SIGNAL(textChanged(const QString&)),
                     this, SLOT(enableDisableButtons()));
    QObject::connect(ui.inputPassword, SIGNAL(textChanged(const QString&)),
                     this, SLOT(enableDisableButtons()));
}

void FormCreateImage::selectImageFile()
{
    QString selected = QFileDialog::getSaveFileName(this, "Select Image File");
    
    if(!selected.isNull())
    {
        ui.inputImageFile->setText(selected);
    }
}

void FormCreateImage::enableDisableButtons()
{
    bool imageFileEmpty = getImageFile().length() == 0;
    bool passwordEmpty = getPassword().length() == 0;
    
    ui.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!imageFileEmpty && !passwordEmpty);
}

std::string FormCreateImage::getImageFile()
{
    return ui.inputImageFile->text().trimmed().toStdString();
}

std::string FormCreateImage::getPassword()
{
    return ui.inputPassword->text().toStdString();
}

int FormCreateImage::getImageSize()
{
    return ui.inputImageSize->value();
}
