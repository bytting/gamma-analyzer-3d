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

#include "gui.h"
#include "gamman3d.h"
#include <QObject>
#include <QWidget>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBox>
#include <QLabel>

Gui::Gui()
{
}

void Gui::setup(gamman3d* g)
{
    using namespace QtDataVisualization;

    // === MAIN WINDOW ===
    g->setMinimumSize(640, 480);
    g->setWindowIcon(QIcon(QStringLiteral(":/res/images/crash.ico")));

    // === MAIN MENU ===
    QMenuBar* menu = new QMenuBar(g);
    g->setMenuBar(menu);
    QMenu* fileMenu = menu->addMenu(QObject::tr("&File"));

    actionOpenSession = fileMenu->addAction(
                QIcon(QStringLiteral(":/res/images/open-32.png")),
                QObject::tr("&Open session"));

    actionCloseSession = fileMenu->addAction(
                QIcon(QStringLiteral(":/res/images/close-32.png")),
                QObject::tr("&Close session"));

    fileMenu->addSeparator();

    actionExit = fileMenu->addAction(
                QIcon(QStringLiteral(":/res/images/exit-32.png")),
                QObject::tr("E&xit"));

    QMenu* viewMenu = menu->addMenu(QObject::tr("&View"));

    actionShowScatter = viewMenu->addAction(
                QIcon(QStringLiteral(":/res/images/scatter-32.png")),
                QObject::tr("Show scatter"));

    actionShowSurface = viewMenu->addAction(
                QIcon(QStringLiteral(":/res/images/chart-32.png")),
                QObject::tr("Show surface"));

    // === TOOL BAR ===
    QToolBar* tools = new QToolBar(g);
    g->addToolBar(tools);
    tools->addAction(actionOpenSession);
    tools->addAction(actionCloseSession);
    tools->addSeparator();
    tools->addAction(actionShowScatter);
    tools->addAction(actionShowSurface);
    tools->addSeparator();

    QLabel* labelScatterTheme = new QLabel(QObject::tr(" Theme "));
    tools->addWidget(labelScatterTheme);

    comboScatterTheme = new QComboBox(tools);
    comboScatterTheme->addItem(QStringLiteral("Qt"));
    comboScatterTheme->addItem(QStringLiteral("Primary Colors"));
    comboScatterTheme->addItem(QStringLiteral("Digia"));
    comboScatterTheme->addItem(QStringLiteral("Stone Moss"));
    comboScatterTheme->addItem(QStringLiteral("Army Blue"));
    comboScatterTheme->addItem(QStringLiteral("Retro"));
    comboScatterTheme->addItem(QStringLiteral("Ebony"));
    comboScatterTheme->addItem(QStringLiteral("Isabelle"));
    comboScatterTheme->setCurrentIndex(0);
    tools->addWidget(comboScatterTheme);

    QLabel* labelScatterNodeSize = new QLabel(QObject::tr(" Node size "));
    tools->addWidget(labelScatterNodeSize);

    sliderScatterNodeSize = new QSlider(tools);
    sliderScatterNodeSize->setMaximumWidth(200);
    sliderScatterNodeSize->setMinimum(1);
    sliderScatterNodeSize->setMaximum(20);
    sliderScatterNodeSize->setValue(2);
    sliderScatterNodeSize->setOrientation(Qt::Horizontal);
    tools->addWidget(sliderScatterNodeSize);

    // === STATUS BAR ===
    QStatusBar* status = new QStatusBar(g);
    g->setStatusBar(status);

    labelStatus = new QLabel(status);
    status->addWidget(labelStatus);

    // === CENTRAL WIDGET ===
    QWidget* widget = new QWidget();
    g->setCentralWidget(widget);

    QHBoxLayout* widgetLayout = new QHBoxLayout(widget);
    widgetLayout->setContentsMargins(0, 0, 0, 0);

    // === STACKED WIDGET ===
    pages = new QStackedWidget(g);
    widgetLayout->addWidget(pages);    

    splitterScatter = new QSplitter(Qt::Horizontal, g);
    splitterScatter->setStretchFactor(1, 1);
    pages->addWidget(splitterScatter);

    splitterSurface = new QSplitter(Qt::Horizontal, g);
    splitterSurface->setStretchFactor(1, 1);
    pages->addWidget(splitterSurface);

    pages->setCurrentWidget(splitterScatter);

    // === SCATTER ===
    scatter = new Q3DScatter();
    QWidget* widgetScatter = QWidget::createWindowContainer(scatter);
    widgetScatter->setContentsMargins(0, 0, 0, 0);
    widgetScatter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitterScatter->addWidget(widgetScatter);

    scatter->scene()->activeCamera()->setCameraPreset(
                Q3DCamera::CameraPresetFront);
    scatter->setShadowQuality(
                QAbstract3DGraph::ShadowQualityNone);
    scatter->activeTheme()->setType(Q3DTheme::ThemeQt);
    scatter->show();

    QScatterDataProxy *scatterProxy = new QScatterDataProxy();
    scatterSeries = new QScatter3DSeries(scatterProxy);
    scatterSeries->setItemSize(0.1f);
    //scatterSeries->setMesh(QAbstract3DSeries::MeshUserDefined);
    //scatterSeries->setUserDefinedMesh(QStringLiteral(":/res/mesh/arrow.obj"));
    scatterSeries->setMeshSmooth(true);
    scatter->addSeries(scatterSeries);
    scatterData = new QScatterDataArray();    

    // === SPECTRUM INFO ===
    QWidget* widSpectrumInfo = new QWidget(splitterScatter);
    widSpectrumInfo->setLayout(new QVBoxLayout());
    widSpectrumInfo->layout()->setAlignment(Qt::AlignTop);
    splitterScatter->addWidget(widSpectrumInfo);

    labelSurfaceLatitude = new QLabel();
    widSpectrumInfo->layout()->addWidget(labelSurfaceLatitude);
    labelSurfaceLongitude = new QLabel();
    widSpectrumInfo->layout()->addWidget(labelSurfaceLongitude);
}
