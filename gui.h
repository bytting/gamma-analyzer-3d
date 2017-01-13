//  gamma-analyzer-3d - 3d visualization of sessions generated by gamma-analyzer
//  Copyright (C) 2017  Dag Robole
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef GUI_H
#define GUI_H

#include <QStackedWidget>
#include <QSplitter>
#include <QLabel>
#include <QAction>
#include <QComboBox>
#include <QSlider>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QScatter3DSeries>

class gamman3d;

class Gui
{
public:

    explicit Gui();

    void setup(gamman3d* g);

    QAction* actionOpenSession;
    QAction* actionCloseSession;
    QAction* actionExit;
    QAction* actionShowScatter;
    QAction* actionShowSurface;

    QStackedWidget* pages;
    QSplitter* splitterScatter;
    QSplitter* splitterSurface;
    QLabel* labelStatus;
    QComboBox* comboScatterTheme;
    QSlider* sliderScatterNodeSize;
    QLabel* labelSurfaceLatitude;
    QLabel* labelSurfaceLongitude;

    QtDataVisualization::Q3DScatter* scatter;
    QtDataVisualization::QScatter3DSeries* scatterSeries;
    QtDataVisualization::QScatterDataArray* scatterData;    
};

#endif // GUI_H
