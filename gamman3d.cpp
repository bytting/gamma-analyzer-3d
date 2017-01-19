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

#include "gamman3d.h"
#include "geo.h"
#include <stdexcept>
#include <QMessageBox>
#include <QFileDialog>
#include <QIcon>
#include <QDebug>

using namespace QtDataVisualization;

gamman3d::gamman3d(QWidget *parent) :
    QMainWindow(parent),
    gui(new Gui()),
    session(new gamma::Session())
{
    gui->setup(this);
    setupSignals();
}

gamman3d::~gamman3d()
{    
    delete session;
    delete gui;
}

void gamman3d::setupSignals()
{
    QObject::connect(
                gui->actionOpenSession,
                &QAction::triggered,
                this,
                &gamman3d::onOpenSession);

    QObject::connect(
                gui->actionCloseSession,
                &QAction::triggered,
                this,
                &gamman3d::onCloseSession);

    QObject::connect(
                gui->actionExit,
                &QAction::triggered,
                this,
                &QWidget::close);

    QObject::connect(
                gui->actionShowScatter,
                &QAction::triggered,
                this,
                &gamman3d::onShowScatter);

    QObject::connect(
                gui->actionShowSurface,
                &QAction::triggered,
                this,
                &gamman3d::onShowSurface);

    QObject::connect(
                gui->comboScatterTheme,
                SIGNAL(currentIndexChanged(int)),
                this,
                SLOT(onChangeSceneTheme(int)));

    QObject::connect(
                gui->sliderScatterNodeSize,
                &QSlider::valueChanged,
                this,
                &gamman3d::onResizeSceneNode);

    QObject::connect(
                gui->scatterSeries,
                &QScatter3DSeries::selectedItemChanged,
                this,
                &gamman3d::onSceneNodeSelected);
}

void gamman3d::populateScene()
{
    // Draw scatter
    gui->scatterData->clear();
    gui->scatterData->resize(session->SpectrumCount());
    QScatterDataItem *p = &gui->scatterData->first();
    double x, y, z;

    for(const auto& spec : session->getSpectrums())
    {
        geo::geodeticToCartesianSimplified(
                    spec->latitudeStart,
                    spec->longitudeStart,
                    x, y, z);

        p->setPosition(QVector3D(x, spec->altitudeStart, y));
        p++;
    }

    gui->scatterSeries->dataProxy()->resetArray(gui->scatterData);
}

void gamman3d::onOpenSession()
{
    try
    {
        QString dir = QDir::toNativeSeparators(
                    QFileDialog::getExistingDirectory(
                        this,
                        tr("Open session directory"),
                        QDir::homePath(),
                        QFileDialog::ShowDirsOnly |
                        QFileDialog::DontResolveSymlinks));
        if(dir.isEmpty())
            return;

        using namespace gamma;

        switch(session->load(dir))
        {
        case Session::LoadResult::DirDoesNotExist:
            QMessageBox::information(
                        this,
                        QObject::tr("Information"),
                        QStringLiteral("Directory does not exist"));
            return;

        case Session::LoadResult::DirNotASession:
            QMessageBox::information(
                        this,
                        QObject::tr("Information"),
                        QStringLiteral("Directory does not appear to be a valid session"));
            return;

        case Session::LoadResult::InvalidSpectrumFound:
            QMessageBox::information(
                        this,
                        QObject::tr("Information"),
                        QStringLiteral("Session contains invalid spectrums"));
            break;

        default:
            break;
        }

        populateScene();
        gui->scatterSeries->setSelectedItem(-1);
        gui->labelStatus->setText(QStringLiteral("Session: ") + dir);
    }
    catch(std::exception &e)
    {
        QMessageBox::warning(this, tr("Error"), e.what());
        return;
    }
}

void gamman3d::onCloseSession()
{
    try
    {
        session->clear();
        gui->scatterData->clear();
        gui->scatterSeries->dataProxy()->resetArray(gui->scatterData);
        gui->labelStatus->setText("");
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

void gamman3d::onResizeSceneNode(int val)
{
    try
    {
        gui->scatterSeries->setItemSize((double)val / 20.0);
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

void gamman3d::onChangeSceneTheme(int theme)
{
    try
    {
        gui->scatter->activeTheme()->setType(Q3DTheme::Theme(theme));
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

void gamman3d::onSceneNodeSelected(int idx)
{
    try
    {
        if(idx < 0)
        {
            gui->labelScatterIndex->setText("");
            gui->labelScatterLatitude->setText("");
            gui->labelScatterLongitude->setText("");
            gui->labelScatterAltitude->setText("");
            return;
        }

        if((unsigned int)idx >= session->SpectrumCount())
        {
            qDebug() << "gamman3d::onSceneNodeSelected: Index out of bounds";
            return;
        }

        const gamma::Spectrum& spec = session->getSpectrum(idx);

        gui->labelScatterIndex->setText(
                    QStringLiteral("Index: ") +
                    QString::number(idx));

        gui->labelScatterLatitude->setText(
                    QStringLiteral("Latitude: ") +
                    QString::number(spec.latitudeStart));

        gui->labelScatterLongitude->setText(
                    QStringLiteral("Longitude: ") +
                    QString::number(spec.longitudeStart));

        gui->labelScatterAltitude->setText(
                    QStringLiteral("Altitude: ") +
                    QString::number(spec.altitudeStart));
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

void gamman3d::onShowScatter()
{
    try
    {
        gui->pages->setCurrentWidget(gui->splitterScatter);
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

void gamman3d::onShowSurface()
{
    try
    {
        gui->pages->setCurrentWidget(gui->splitterSurface);
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}
