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

#ifndef GAMMAN3D_H
#define GAMMAN3D_H

//#include "gui.h"
#include "session.h"
#include <QMainWindow>
#include <QLabel>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DInput/QInputAspect>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QDiffuseMapMaterial>
#include <Qt3DExtras/QOrbitCameraController>

namespace Ui {
class gamman3d;
}

class gamman3d : public QMainWindow
{
    Q_OBJECT

public:

    explicit gamman3d(QWidget* parent = 0);
    ~gamman3d();

    void setupWidgets();

    void setupSignals();

    void createScene();

    void populateScene();

    void addSceneNode(const QVector3D &vec);

private:

    Ui::gamman3d* ui;
    QLabel* labelStatus;
    gad::Session* session;

    Qt3DExtras::Qt3DWindow* view;
    Qt3DCore::QEntity* scene;
    Qt3DRender::QCamera* camera;
    Qt3DExtras::QSphereMesh* mesh;

private slots:

    void onExitApplication();
    void onOpenSession();
    void onCloseSession();
    void onSceneNodeSelected(int idx);
    void onShowScene();
    void onShowSettings();
    void onSetScript();
};

#endif // GAMMAN3D_H
