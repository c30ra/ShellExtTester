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

#ifndef THUMBNAILRENDER_H
#define THUMBNAILRENDER_H

#include <QWidget>

class ThumbnailRender : public QWidget
{
  Q_OBJECT
public:
  explicit ThumbnailRender(QWidget *parent = 0);
  ~ThumbnailRender();

  QPixmap pixmap() const;
  void setPixmap(const QPixmap &pixmap);

signals:

public slots:

  // QWidget interface
protected:
  void paintEvent(QPaintEvent *);

private:
  QPixmap m_pixmap;
};

#endif // THUMBNAILRENDER_H
