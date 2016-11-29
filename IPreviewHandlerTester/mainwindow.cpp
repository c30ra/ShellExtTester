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

//Qt
#include <QFileDialog>
#include <QMessageBox>
#include <QtWin>
#include <QDebug>
#include <QUUID>

//system
#include <windows.h>
#include "objbase.h"
#include "shlwapi.h"
#include "shobjidl.h"
//#include <GdiPlus.h>

//STL
#include <memory>

const CLSID CLSID_PreviewHandler =
    //{ 0x78A573CA, 0x297E, 0x4D9F, { 0xA5, 0xFC, 0x7F, 0x6E, 0x5E, 0xEA, 0x6F, 0xC9 } };
    // {F0485B13-4C06-44C1-8E4C-37ABEE9B7054}
{ 0xf0485b13, 0x4c06, 0x44c1, { 0x8e, 0x4c, 0x37, 0xab, 0xee, 0x9b, 0x70, 0x54 } };

const GUID APPID_PreviewHandler =
    //{ 0x2992DE27, 0x3526, 0x48C5, { 0xB7, 0x65, 0xE5, 0x52, 0x78, 0xEC, 0xBE, 0x9D } };
    // {81DFCDD9-D422-4FC7-B038-B17172A2E55A}
{ 0x81dfcdd9, 0xd422, 0x4fc7, { 0xb0, 0x38, 0xb1, 0x71, 0x72, 0xa2, 0xe5, 0x5a } };

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  pPreviewHandler{nullptr}
{
  ui->setupUi(this);

  connect(ui->actionOpen, SIGNAL(triggered()) , this, SLOT(openImage()));
}

MainWindow::~MainWindow()
{
  cleanCOM();
  delete ui;
}

void MainWindow::openImage()
{
  cleanCOM();
  auto file = QFileDialog::getOpenFileName(this);
  if(!file.isEmpty())
    {
      auto hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
      if (SUCCEEDED(hr))
        {
          //          auto filePath = std::make_shared<wchar_t>();
          //          wchar_t *filePath = new wchar_t[file.size()];
          file.replace('/','\\');
          auto filePath = file.toStdWString();
          auto wFilePath = filePath.c_str();

          auto index = file.indexOf(QRegExp(".[a-zA-Z0-9]+"),0);
          auto extension = file.right(index).toStdWString();
          auto wExtension = extension.c_str();

          wchar_t csClsid[64];
          LPDWORD pSize = (LPDWORD)malloc(64);

          HRESULT hr_regRead;
          hr_regRead = HRESULT_FROM_WIN32(RegGetValueW(HKEY_CLASSES_ROOT,wExtension ,
                                                       L"",RRF_RT_REG_SZ,NULL,csClsid,pSize));
          //          auto pwcExtn = file.section();
          //          CLSIDFromString(csClsid);
          //          hr = SHCreateItemFromParsingName(wFilePath, NULL, IID_PPV_ARGS(&psi));
          QUuid uuid(QString("F0485B13-4C06-44C1-8E4C-37ABEE9B7054"));

          hr = CoCreateInstance(uuid, NULL, CLSCTX_INPROC_SERVER |
                                CLSCTX_LOCAL_SERVER,
                                IID_IPreviewHandler,(LPVOID*)&pPreviewHandler);

          if (SUCCEEDED(hr))
            {
              pPreviewHandler->QueryInterface(IID_IInitializeWithFile,(LPVOID*)&m_pIFile);
              hr = m_pIFile->Initialize(wFilePath,STGM_READ);
//              hr = psi->BindToHandler(NULL, APPID_PreviewHandler, IID_PPV_ARGS(&pPreviewHandler));
              if (SUCCEEDED(hr))
                {
                  auto hWnd = reinterpret_cast<HWND> (ui->hostwidget->winId());
                  const RECT rect
                  {
                    this->rect().left(),
                        this->rect().top(),
                        this->rect().right(),
                        this->rect().bottom()
                  };

                  hr = pPreviewHandler->SetWindow(hWnd, &rect);

                  if(SUCCEEDED(hr))
                    {
                      pPreviewHandler->DoPreview();
                    }
                }

              else
                psi->Release();
            }
        }
      else
        CoUninitialize();
    }
}

void MainWindow::cleanCOM()
{
  if(pPreviewHandler)
    {
      pPreviewHandler->Unload();
      pPreviewHandler->Release();
    }
  if(psi)
    psi->Release();
  if(m_pIFile)
    m_pIFile->Release();
  CoUninitialize();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  const RECT rect
  {
    this->rect().left(),
        this->rect().top(),
        this->rect().right(),
        this->rect().bottom()
  };
  auto hr = S_OK;
  if(pPreviewHandler)
    hr = pPreviewHandler->SetRect(&rect);

  if(!SUCCEEDED(hr))
    qWarning() << "resizing failed";

  QWidget::resizeEvent(event);
}
