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
#include "geo.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QIcon>
#include <QDebug>

gamman3d::gamman3d(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::gamman3d),
      session(new gad::Session())
{
    ui->setupUi(this);
    setupWidgets();
    setupSignals();
    createScene();
}

gamman3d::~gamman3d()
{
    //delete scene;
    //delete view;
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

void gamman3d::createScene()
{
    view = new Qt3DExtras::Qt3DWindow();
    auto containerScene = QWidget::createWindowContainer(view);
    ui->layoutScene->addWidget(containerScene);

    view->defaultFrameGraph()->setClearColor(
                ui->pageScene->palette().color(QWidget::backgroundRole()));
    scene = new Qt3DCore::QEntity();

    camera = view->camera();
    camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera->setAspectRatio(view->width() / view->height());
    //camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    camera->setPosition(QVector3D(0.0f, 0.0f, 40.0f));
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    camera->setNearPlane(0.01f);
    camera->setFarPlane(100000.0f);

    mesh = new Qt3DExtras::QSphereMesh(scene);
    mesh->setRadius(0.05f);

    Qt3DExtras::QOrbitCameraController* camCtrl = new Qt3DExtras::QOrbitCameraController(scene);
    camCtrl->setLinearSpeed(50.0f);
    camCtrl->setLookSpeed(180.0f);
    camCtrl->setCamera(camera);

    view->setRootEntity(scene);
    view->show();
}

void gamman3d::populateScene()
{
    // FIXME: clear scene

    double x, y, z;
    double minX=0, minY=0, minZ=0, maxX=0, maxY=0, maxZ=0;
    double minAlt = 0, maxAlt = 0;
    bool first = true;

    for(const auto& spec : session->getSpectrumList())
    {
        geo::geodeticToCartesianSimplified(
                    spec->latitudeStart(),
                    spec->longitudeStart(),
                    x, y, z);

        if(first)
        {
            minX = maxX = x;
            minY = maxY = y;
            minZ = maxZ = z;
            minAlt = spec->altitudeStart();
            maxAlt = spec->altitudeStart();
            first = false;
        }
        else
        {
            if(x < minX)
                minX = x;
            if(y < minY)
                minY = y;
            if(z < minZ)
                minZ = z;

            if(x > maxX)
                maxX = x;
            if(y > maxY)
                maxY = y;
            if(z > maxZ)
                maxZ = z;

            if(minAlt > spec->altitudeStart())
                minAlt = spec->altitudeStart();
            if(maxAlt < spec->altitudeStart())
                maxAlt = spec->altitudeStart();
        }
    }

    double deltaX = (maxX - minX) / 2.0;
    double deltaY = (maxY - minY) / 2.0;
    double deltaZ = (maxZ - minZ) / 2.0;

    for(const auto& spec : session->getSpectrumList())
    {
        geo::geodeticToCartesianSimplified(
                    spec->latitudeStart(),
                    spec->longitudeStart(),
                    x, y, z);

        x -= minX + deltaX;
        y -= minY + deltaY;
        z -= minZ + deltaZ;
        double alt = spec->altitudeStart() - minAlt;

        x *= 1000.0;
        y *= 1000.0;
        z *= 1000.0;
        alt /= 10.0;

        addSceneNode(QVector3D(x, alt, y));
    }
}

void gamman3d::addSceneNode(const QVector3D &vec)
{
    Qt3DCore::QEntity* entity = new Qt3DCore::QEntity(scene);
    entity->addComponent(mesh);

    Qt3DCore::QTransform* transform = new Qt3DCore::QTransform(scene);
    transform->setTranslation(vec);
    entity->addComponent(transform);

    Qt3DExtras::QPhongMaterial* mat = new Qt3DExtras::QPhongMaterial(scene);
    mat->setDiffuse(QColor(100, 50, 50));
    mat->setSpecular(QColor(255, 0, 0));
    mat->setAmbient(QColor(200, 20, 20));
    mat->setShininess(6.0f);
    entity->addComponent(mat);
}

void gamman3d::onExitApplication()
{
    //view->disconnect();
    //view->close();
    this->close();
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
        scene->childNodes().clear();
        session->clear();
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
