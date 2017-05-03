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

#ifndef GEO_H
#define GEO_H

#include <QVector3D>
#include <QGeoCoordinate>

namespace Geo
{

template<typename T>
const T PI = 3.14159265358979323846;

template<typename T>
const T EARTH_RADIUS = 6378137.0;

template<typename T>
T degreeToRadian(T degree)
{
    return degree * (PI<T> / static_cast<T>(180));
}

template<typename T>
T radianToDegree(T radian)
{
    return radian * (static_cast<T>(180) / PI<T>);
}

class Coordinate : public QGeoCoordinate
{
public:

    Coordinate();
    Coordinate(double latitude, double longitude);
    Coordinate(double latitude, double longitude, double altitude);
    Coordinate(const Coordinate &rhs);
    Coordinate(const QGeoCoordinate &rhs);
    virtual ~Coordinate() {}

    void setAngles(double latitude, double longitude);
    void setAngles(double latitude, double longitude, double altitude);
    void setAnglesFromCartesian(const QVector3D &position);

    QVector3D toCartesian() const;
};

} // namespace Geo

#endif // GEO_H
