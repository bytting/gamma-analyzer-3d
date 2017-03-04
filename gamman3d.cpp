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
#include "exceptions.h"
#include "geo.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QIcon>
#include <QDebug>

using namespace QtDataVisualization;

gamman3d::gamman3d(QWidget *parent)
    : QMainWindow(parent),
      widgets(new gui::Widgets()),
      session(new gad::Session())
{
    widgets->setup(this);
    setupSignals();
}

gamman3d::~gamman3d()
{
    delete session;
    delete widgets;
}

void gamman3d::setupSignals()
{
    QObject::connect(
                widgets->actionOpenSession,
                &QAction::triggered,
                this,
                &gamman3d::onOpenSession);

    QObject::connect(
                widgets->actionCloseSession,
                &QAction::triggered,
                this,
                &gamman3d::onCloseSession);

    QObject::connect(
                widgets->actionExit,
                &QAction::triggered,
                this,
                &QWidget::close);

    QObject::connect(
                widgets->actionShowScatter,
                &QAction::triggered,
                this,
                &gamman3d::onShowScatter);

    QObject::connect(
                widgets->actionShowSurface,
                &QAction::triggered,
                this,
                &gamman3d::onShowSurface);

    QObject::connect(
                widgets->actionSetScript,
                &QAction::triggered,
                this,
                &gamman3d::onSetScript);

    QObject::connect(
                widgets->comboScatterTheme,
                qOverload<int>(&QComboBox::currentIndexChanged),
                this,
                qOverload<int>(&gamman3d::onChangeSceneTheme));

    QObject::connect(
                widgets->sliderScatterNodeSize,
                &QSlider::valueChanged,
                this,
                &gamman3d::onResizeSceneNode);

    QObject::connect(
                widgets->scatterSeries,
                &QScatter3DSeries::selectedItemChanged,
                this,
                &gamman3d::onSceneNodeSelected);
}

void gamman3d::populateScene()
{
    // Draw scatter
    widgets->scatterData->clear();
    widgets->scatterData->resize(session->spectrumCount());
    QScatterDataItem *p = &widgets->scatterData->first();
    double x, y, z;

    for(const auto& spec : session->getSpectrumList())
    {
        geo::geodeticToCartesianSimplified(
                    spec->latitudeStart(),
                    spec->longitudeStart(),
                    x, y, z);

        p->setPosition(QVector3D(x, spec->altitudeStart(), y));
        p++;
    }

    widgets->scatterSeries->dataProxy()->resetArray(widgets->scatterData);
}

void gamman3d::onOpenSession()
{
    try
    {
        QString sessionDir = QFileDialog::getExistingDirectory(
                    this,
                    tr("Open session directory"),
                    QDir::homePath(),
                    QFileDialog::ShowDirsOnly |
                    QFileDialog::DontResolveSymlinks);
        if(sessionDir.isEmpty())
            return;

        sessionDir = QDir::toNativeSeparators(sessionDir);

        session->loadPath(sessionDir);

        populateScene();

        widgets->scatterSeries->setSelectedItem(-1);
        widgets->labelStatus->setText(
                    QStringLiteral("Session: ") + sessionDir);
    }
    catch(const GammanException& e)
    {
        qDebug() << e.what();
        QMessageBox::warning(this, tr("Warning"), e.what());
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
        QMessageBox::critical(this, tr("Error"), e.what());
    }
}

void gamman3d::onCloseSession()
{
    try
    {
        session->clear();
        widgets->scatterData->clear();
        widgets->scatterSeries->dataProxy()->resetArray(widgets->scatterData);
        widgets->labelStatus->setText("");
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
        widgets->scatterSeries->setItemSize((double)val / 20.0);
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
        widgets->scatter->activeTheme()->setType(
                    Q3DTheme::Theme(theme));
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
            widgets->labelScatterIndex->setText("");
            widgets->labelScatterLatitude->setText("");
            widgets->labelScatterLongitude->setText("");
            widgets->labelScatterAltitude->setText("");
            widgets->labelScatterTime->setText("");
            widgets->labelScatterDoserate->setText("");
            return;
        }

        const gad::Spectrum* spec = session->getSpectrum(idx);

        widgets->labelScatterIndex->setText(
                    QStringLiteral("Index: ") + QString::number(idx));

        widgets->labelScatterLatitude->setText(
                    QStringLiteral("Latitude: ") +
                    QString::number(spec->latitudeStart()));

        widgets->labelScatterLongitude->setText(
                    QStringLiteral("Longitude: ") +
                    QString::number(spec->longitudeStart()));

        widgets->labelScatterAltitude->setText(
                    QStringLiteral("Altitude: ") +
                    QString::number(spec->altitudeStart()));

        widgets->labelScatterTime->setText(
                    QStringLiteral("Time: ") +
                    spec->gpsTimeStart().toLocalTime().toString(
                        "yyyy-MM-dd hh:mm:ss"));

        widgets->labelScatterDoserate->setText(
                    QStringLiteral("Doserate: ") +
                    QString::number(spec->doserate()));
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
        widgets->pages->setCurrentWidget(widgets->splitterScatter);
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
        widgets->pages->setCurrentWidget(widgets->splitterSurface);
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

void gamman3d::onSetScript()
{
    try
    {
        QString scriptFileName = QFileDialog::getOpenFileName(
                    this,
                    tr("Open Lua script"),
                    QDir::homePath(),
                    tr("Lua script (*.lua)"));

        scriptFileName = QDir::toNativeSeparators(scriptFileName);

        if(QFile::exists(scriptFileName))
        {
            session->loadDoserateScript(scriptFileName);
        }
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}
