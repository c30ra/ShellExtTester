/*******************************************************************************
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2015  Luca Carella

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thumbnailrender.h"

//Qt
#include <QFileDialog>
#include <QMessageBox>
#include <QtWin>

//system
#include <windows.h>
#include <shlobj.h>
#include <thumbcache.h>
//#include <GdiPlus.h>

//STL
#include <memory>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->actionOpen, SIGNAL(triggered()) , this, SLOT(openImage()));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::openImage()
{
  auto file = QFileDialog::getOpenFileName(this);
  if(!file.isEmpty())
    {
      HBITMAP g_hThumbnail;
      auto size = ui->thumbSizeSpinBox->value();
      auto hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
      if (SUCCEEDED(hr))
        {
          IShellItem *psi;
//          auto filePath = std::make_shared<wchar_t>();
//          wchar_t *filePath = new wchar_t[file.size()];
          file.replace('/','\\');
          auto filePath = file.toStdWString();
          auto wFilePath = filePath.c_str();
          hr = SHCreateItemFromParsingName(wFilePath, NULL, IID_PPV_ARGS(&psi));
          if (SUCCEEDED(hr))
            {
              IThumbnailProvider *pThumbProvider;
              hr = psi->BindToHandler(NULL, BHID_ThumbnailHandler, IID_PPV_ARGS(&pThumbProvider));
              if (SUCCEEDED(hr))
                {
                  WTS_ALPHATYPE wtsAlpha;
                  hr = pThumbProvider->GetThumbnail(size, &g_hThumbnail, &wtsAlpha);
                  if (SUCCEEDED(hr))
                    {
                      ui->thumbnailRender->setPixmap(
                            QtWin::fromHBITMAP(g_hThumbnail, QtWin::HBitmapNoAlpha));
                    }
                  pThumbProvider->Release();
                }

              else  QMessageBox::warning(this,"Warning",
                                        "Thumbnail handler not "
                                        "avaible for this file format!");
              psi->Release();
            }
//          delete[] filePath;
          CoUninitialize();
        }
    }
}
