//  gamma-viewer-3d - 3d visualization of sessions generated by gamma-analyzer
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

#ifndef GRIDENTITY_H
#define GRIDENTITY_H

#include <QColor>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGeometry>
#include <Qt3DExtras/QPhongMaterial>

class GridEntityXZ : public Qt3DCore::QEntity
{
    Q_OBJECT

public:

    GridEntityXZ(float y,
                 unsigned int count,
                 float interval,
                 const QColor &color,
                 Qt3DCore::QEntity *parent);

    ~GridEntityXZ() override;

private:

    Qt3DRender::QGeometryRenderer *mMesh;
    Qt3DRender::QGeometry *mGeometry;
    Qt3DRender::QBuffer *mDataBuffer;
    Qt3DRender::QAttribute *mPositionAttribute;
    Qt3DExtras::QPhongMaterial *mMaterial;
};

#endif // GRIDENTITY_H
