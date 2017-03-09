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
#include "ui_gamman3d.h"
#include "exceptions.h"
//#include "planeentity.h"
#include "spectrumentity.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QIcon>
#include <QDebug>
#include <cmath>

gamman3d::gamman3d(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::gamman3d),
      session(new gad::Session())
{
    ui->setupUi(this);
    setupWidgets();
    setupSignals();
    setupScene();
}

gamman3d::~gamman3d()
{
    delete grid;
    grid = nullptr;
    delete cameraController;
    cameraController = nullptr;
    delete sceneEntity;
    sceneEntity = nullptr;
    delete fwdRenderer;
    fwdRenderer = nullptr;
    delete sortPolicy;
    sortPolicy = nullptr;
    delete view;
    delete session;
    delete ui;
}

void gamman3d::setupWidgets()
{
    labelStatus = new QLabel(ui->status);
    statusBar()->addWidget(labelStatus);
}

void gamman3d::setupSignals()
{
    QObject::connect(
                ui->actionOpenSession,
                &QAction::triggered,
                this,
                &gamman3d::onOpenSession);

    QObject::connect(
                ui->actionCloseSession,
                &QAction::triggered,
                this,
                &gamman3d::onCloseSession);

    QObject::connect(
                ui->actionExit,
                &QAction::triggered,
                this,
                &gamman3d::onExitApplication);

    QObject::connect(
                ui->actionShowScene,
                &QAction::triggered,
                this,
                &gamman3d::onShowScene);

    QObject::connect(
                ui->actionShowSettings,
                &QAction::triggered,
                this,
                &gamman3d::onShowSettings);

    QObject::connect(
                ui->actionLoadDoserateScript,
                &QAction::triggered,
                this,
                &gamman3d::onSetScript);

    /*QObject::connect(
                widgets->scatterSeries,
                &QScatter3DSeries::selectedItemChanged,
                this,
                &gamman3d::onSceneNodeSelected);*/
}

void gamman3d::setupScene()
{
    view = new Qt3DExtras::Qt3DWindow();
    auto containerScene = QWidget::createWindowContainer(view);
    ui->layoutScene->addWidget(containerScene);

    camera = view->camera();
    camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera->setAspectRatio(view->width() / view->height());
    //camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    camera->setPosition(QVector3D(0.0f, 20.0f, 40.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    camera->setNearPlane(0.01f);
    camera->setFarPlane(100000.0f);

    sortPolicy = new Qt3DRender::QSortPolicy();
    sortPolicy->setSortTypes(QVector<Qt3DRender::QSortPolicy::SortType>(
        {Qt3DRender::QSortPolicy::BackToFront}
    ));

    fwdRenderer = new Qt3DExtras::QForwardRenderer(sortPolicy);
    fwdRenderer->setCamera(camera);
    fwdRenderer->setSurface(view);
    //fwdRenderer->setClearColor(ui->pageScene->palette().color(QWidget::backgroundRole()));
    fwdRenderer->setClearColor(Qt::black);

    view->setActiveFrameGraph(sortPolicy);

    sceneEntity = new Qt3DCore::QEntity();

    cameraController = new Qt3DExtras::QOrbitCameraController(sceneEntity);
    cameraController->setLinearSpeed(50.0f);
    cameraController->setLookSpeed(180.0f);
    cameraController->setCamera(camera);

    grid = new GridEntity(sceneEntity, 10, 10.0f);

    view->setRootEntity(sceneEntity);
    view->show();
}

void gamman3d::populateScene()
{
    // FIXME: clear scene
    // FIXME: problem with negative coordinates?

    double halfX = (session->maxX() - session->minX()) / 2.0;
    double halfY = (session->maxY() - session->minY()) / 2.0;
    double halfZ = (session->maxZ() - session->minZ()) / 2.0;

    for(const auto& spec : session->getSpectrumList())
    {
        double x = spec->x1() - session->minX() - halfX;
        double y = spec->y1() - session->minY() - halfY;
        double z = spec->z1() - session->minZ() - halfZ;
        double alt = spec->altitudeStart() - session->minAltitude();

        x *= 10000.0;
        y *= 10000.0;
        z *= 10000.0;
        //alt /= 10.0;

        auto *entity = new SpectrumEntity(
                    sceneEntity,
                    QVector3D(x, alt, -y),
                    session->minDoserate(),
                    session->maxDoserate(),
                    spec->doserate());
    }

    camera->setPosition(QVector3D(0.0f, 20.0f, 80.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
}

void gamman3d::onExitApplication()
{
    QWidget::close();
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

        labelStatus->setText(QStringLiteral("Session: ") + sessionDir);
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
        //session->clear(); // FIXME
        labelStatus->setText("");
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

void gamman3d::onSceneNodeSelected(int idx)
{
    /*try
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
    }*/
}

void gamman3d::onShowScene()
{
    try
    {
        ui->pages->setCurrentWidget(ui->pageScene);
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

void gamman3d::onShowSettings()
{
    try
    {
        ui->pages->setCurrentWidget(ui->pageSettings);
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
                    tr("Load doserate script"),
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
