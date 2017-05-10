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

#include "detector.h"
#include "exceptions.h"
#include <QJsonArray>
#include <cmath>

namespace Gamma
{

Detector::Detector(const QJsonObject &obj)
{
    loadJson(obj);
}

void Detector::loadJson(const QJsonObject &obj)
{
    if(!obj.contains("TypeName"))
        throw MissingJsonValue("Detector:TypeName");
    mTypeName = obj.value("TypeName").toString();

    if(!obj.contains("CurrentHV"))
        throw MissingJsonValue("Detector:CurrentHV");
    mHV = obj.value("CurrentHV").toInt();

    if(!obj.contains("CurrentNumChannels"))
        throw MissingJsonValue("Detector:CurrentNumChannels");
    mNumChannels = obj.value("CurrentNumChannels").toInt();

    if(!obj.contains("Serialnumber"))
        throw MissingJsonValue("Detector:Serialnumber");
    mSerialnumber = obj.value("Serialnumber").toString();

    if(!obj.contains("CurrentCoarseGain"))
        throw MissingJsonValue("Detector:CurrentCoarseGain");
    mCoarseGain = obj.value("CurrentCoarseGain").toDouble();

    if(!obj.contains("CurrentFineGain"))
        throw MissingJsonValue("Detector:CurrentFineGain");
    mFineGain = obj.value("CurrentFineGain").toDouble();

    if(!obj.contains("CurrentLivetime"))
        throw MissingJsonValue("Detector:CurrentLivetime");
    mLivetime = obj.value("CurrentLivetime").toInt();

    if(!obj.contains("CurrentLLD"))
        throw MissingJsonValue("Detector:CurrentLLD");
    mLLD = obj.value("CurrentLLD").toInt();

    if(!obj.contains("CurrentULD"))
        throw MissingJsonValue("Detector:CurrentULD");
    mULD = obj.value("CurrentULD").toInt();

    if(!obj.contains("EnergyCurveCoefficients"))
        throw MissingJsonValue("Detector:EnergyCurveCoefficients");
    QJsonArray coeffs = obj.value("EnergyCurveCoefficients").toArray();

    mEnergyCurveCoefficients.clear();
    for(auto c : coeffs)
        mEnergyCurveCoefficients.emplace_back(c.toDouble());
}

double Detector::getEnergy(int index) const
{
    if (mEnergyCurveCoefficients.size() < 2
            || mEnergyCurveCoefficients.size() > 5
            || index < 0)
        return 0.0;

    double dx = (double)index;
    double e = 0.0;

    for(CoefficientListSize i = 0; i < mEnergyCurveCoefficients.size(); i++)
        e += mEnergyCurveCoefficients[i] * std::pow(dx, (double)i);

    return e;
}

} // namespace Gamma
