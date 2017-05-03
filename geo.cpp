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

#include "geo.h"
#include <cmath>

namespace Geo
{

Coordinate::Coordinate()
{
}

Coordinate::Coordinate(double latitude, double longitude)
    : QGeoCoordinate(latitude, longitude)
{
}

Coordinate::Coordinate(double latitude, double longitude, double altitude)
    : QGeoCoordinate(latitude, longitude, altitude)
{
}

Coordinate::Coordinate(const Coordinate &rhs)
    : QGeoCoordinate(static_cast<QGeoCoordinate>(rhs))
{
}

Coordinate::Coordinate(const QGeoCoordinate &rhs)
    : QGeoCoordinate(rhs)
{
}

void Coordinate::setAngles(double latitude, double longitude)
{
    setLatitude(latitude);
    setLongitude(longitude);
}

void Coordinate::setAngles(double latitude, double longitude, double altitude)
{
    setLatitude(latitude);
    setLongitude(longitude);
    setAltitude(altitude);
}

void Coordinate::setAnglesFromCartesian(const QVector3D &position)
{
    // Simplified conversion from cartesian to geodetic coordinates

    setLatitude(radianToDegree<double>(
                    std::asin(position.z() / EARTH_RADIUS<double>)));

    setLongitude(radianToDegree<double>(
                     std::atan2(position.y(), position.x())));
}

QVector3D Coordinate::toCartesian() const
{
    // Simplified conversion from geodetic to cartesian coordinates

    QVector3D position;

    auto cosLat = std::cos(degreeToRadian<double>(latitude()));
    auto sinLat = std::sin(degreeToRadian<double>(latitude()));
    auto cosLon = std::cos(degreeToRadian<double>(longitude()));
    auto sinLon = std::sin(degreeToRadian<double>(longitude()));

    position.setX(EARTH_RADIUS<double> * cosLat * cosLon);
    position.setY(EARTH_RADIUS<double> * cosLat * sinLon);
    position.setZ(EARTH_RADIUS<double> * sinLat);

    return position;
}

} // namespace Geo
