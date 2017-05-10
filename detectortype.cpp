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

#include "detectortype.h"
#include "exceptions.h"

namespace Gamma
{

DetectorType::DetectorType(const QJsonObject &obj)
    :
      mMaxNumChannels(0),
      mMinHV(0),
      mMaxHV(0)
{
    loadJson(obj);
}

void DetectorType::loadJson(const QJsonObject &obj)
{
    if(!obj.contains("Name"))
        throw MissingJsonValue("DetectorType:Name");
    mName = obj.value("Name").toString();

    if(!obj.contains("MaxNumChannels"))
        throw MissingJsonValue("DetectorType:MaxNumChannels");
    mMaxNumChannels = obj.value("MaxNumChannels").toInt();

    if(!obj.contains("MinHV"))
        throw MissingJsonValue("DetectorType:MinHV");
    mMinHV = obj.value("MinHV").toInt();

    if(!obj.contains("MaxHV"))
        throw MissingJsonValue("DetectorType:MaxHV");
    mMaxHV = obj.value("MaxHV").toInt();

    if(!obj.contains("GEScript"))
        throw MissingJsonValue("DetectorType:GEScript");
    mGEScript = obj.value("GEScript").toString();
}

} // namespace Gamma
