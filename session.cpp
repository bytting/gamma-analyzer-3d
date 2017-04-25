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

#include "session.h"
#include "geo.h"
#include <stdexcept>
#include <memory>
#include <cmath>
#include <algorithm>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace Gamma
{

Session::Session()
{
    L = luaL_newstate();
    if(!L)
        throw UnableToCreateLuaState("Session::Session");
    luaL_openlibs(L);
}

Session::~Session()
{
    try
    {
        clear();
        lua_close(L);
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

const Spectrum* Session::getSpectrum(SpecListSize index) const
{
    if(index >= mSpecList.size())
        throw GA::IndexOutOfBounds("Session::getSpectrum");

    return mSpecList[index];
}

const SpecList& Session::getSpectrumList() const
{
    return mSpecList;
}

void Session::loadPath(QString sessionPath)
{
    const QDir sessionDir(sessionPath);
    if(!sessionDir.exists())
        throw GA::DirDoesNotExist(sessionDir.absolutePath());

    const QDir spectrumDir(sessionPath + QDir::separator() +
                     QStringLiteral("json"));
    if (!spectrumDir.exists())
        throw DirIsNotASession(spectrumDir.absolutePath());

    const auto sessionFile = sessionPath + QDir::separator() +
            QStringLiteral("session.json");
    if(!QFile::exists(sessionFile))
        throw DirIsNotASession(sessionDir.absolutePath());

    clear();

    loadSessionFile(sessionFile);

    const auto fileEntries = spectrumDir.entryInfoList(
                QStringList() << "*.json",
                QDir::NoDotAndDotDot | QDir::Files);

    bool first = true;
    for(const auto& fileEntry : fileEntries)
    {
        try
        {
            auto spec = new Spectrum(fileEntry.absoluteFilePath());

            if(mScriptLoaded)
                spec->calculateDoserate(mDetector, L);

            if(first)
            {
                mMinDoserate = spec->doserate();
                mMaxDoserate = spec->doserate();

                mMinX = mMaxX = spec->position.x();
                mMinY = mMaxY = spec->position.y();
                mMinZ = mMaxZ = spec->position.z();

                mMinAltitude = mMaxAltitude = spec->coordinates.altitude();
                mMinLatitude = mMaxLatitude = spec->coordinates.latitude();
                mMinLongitude = mMaxLongitude = spec->coordinates.longitude();

                first = false;
            }
            else
            {
                if(spec->doserate() < mMinDoserate)
                    mMinDoserate = spec->doserate();
                if(spec->doserate() > mMaxDoserate)
                    mMaxDoserate = spec->doserate();

                if(mMinX > spec->position.x())
                    mMinX = spec->position.x();
                if(mMinY > spec->position.y())
                    mMinY = spec->position.y();
                if(mMinZ > spec->position.z())
                    mMinZ = spec->position.z();

                if(mMaxX < spec->position.x())
                    mMaxX = spec->position.x();
                if(mMaxY < spec->position.y())
                    mMaxY = spec->position.y();
                if(mMaxZ < spec->position.z())
                    mMaxZ = spec->position.z();

                if(mMinAltitude > spec->coordinates.altitude())
                    mMinAltitude = spec->coordinates.altitude();
                if(mMaxAltitude < spec->coordinates.altitude())
                    mMaxAltitude = spec->coordinates.altitude();

                if(mMinLatitude > spec->coordinates.latitude())
                    mMinLatitude = spec->coordinates.latitude();
                if(mMaxLatitude < spec->coordinates.latitude())
                    mMaxLatitude = spec->coordinates.latitude();

                if(mMinLongitude > spec->coordinates.longitude())
                    mMinLongitude = spec->coordinates.longitude();
                if(mMaxLongitude < spec->coordinates.longitude())
                    mMaxLongitude = spec->coordinates.longitude();
            }

            mSpecList.push_back(spec);
        }
        catch(const GA::Exception& e)
        {
            qDebug() << e.what();
        }
    }

    auto halfX = (mMaxX - mMinX) / 2.0;
    auto halfZ = (mMaxZ - mMinZ) / 2.0;

    // Calculate center and north positions, transpose to origo and extend by a factor of 18000.
    // Use -3.0 as y-axis to move positions below ground level.
    center = Geo::geodeticToCartesianSimplified(
                mMinLatitude + ((mMaxLatitude - mMinLatitude) / 2.0),
                mMinLongitude + ((mMaxLongitude - mMinLongitude) / 2.0));

    center.setX((center.x() - mMinX - halfX) * 18000.0);
    center.setY(-3.0);
    center.setZ((center.z() - mMinZ - halfZ) * -18000.0);

    north = Geo::geodeticToCartesianSimplified(
                mMinLatitude + ((mMaxLatitude - mMinLatitude) / 2.0) + 0.00025,
                mMinLongitude + ((mMaxLongitude - mMinLongitude) / 2.0));

    north.setX((north.x() - mMinX - halfX) * 18000.0);
    north.setY(-3.0);
    north.setZ((north.z() - mMinZ - halfZ) * -18000.0);

    // Transpose spectrum positions to origo and extend by a factor of 18000.
    // Use delta altitude as y-axis to "flatten" the spectrums right above ground level.

    for(Spectrum *spec : mSpecList)
    {
        spec->position.setX((spec->position.x() - mMinX - halfX) * 18000.0);
        spec->position.setY(spec->coordinates.altitude() - mMinAltitude);
        spec->position.setZ((spec->position.z() - mMinZ - halfZ) * -18000.0);
    }

    // Recalculate min/max positions for session
    auto p = std::minmax_element(mSpecList.begin(), mSpecList.end(),
        [&](Spectrum* s1, Spectrum *s2) { return s1->position.x() < s2->position.x(); });
    mMinX = (*p.first)->position.x();
    mMaxX = (*p.second)->position.x();

    p = std::minmax_element(mSpecList.begin(), mSpecList.end(),
        [&](Spectrum* s1, Spectrum *s2) { return s1->position.y() < s2->position.y(); });
    mMinY = (*p.first)->position.y();
    mMaxY = (*p.second)->position.y();

    p = std::minmax_element(mSpecList.begin(), mSpecList.end(),
        [&](Spectrum* s1, Spectrum *s2) { return s1->position.z() < s2->position.z(); });
    mMinZ = (*p.first)->position.z();
    mMaxZ = (*p.second)->position.z();
}

void Session::loadSessionFile(QString sessionFile)
{
    QFile jsonFile(sessionFile);
    if(!jsonFile.open(QFile::ReadOnly))
        throw GA::UnableToLoadFile(sessionFile);

    auto doc = QJsonDocument().fromJson(jsonFile.readAll());
    if(!doc.isObject())
        throw InvalidSessionFile(sessionFile);

    auto root = doc.object();

    if(!root.contains("Name"))
        throw GA::MissingJsonValue("Session:Name");
    mName = root.value("Name").toString();

    if(!root.contains("Comment"))
        throw GA::MissingJsonValue("Session:Comment");
    mComment = root.value("Comment").toString();

    if(!root.contains("Livetime"))
        throw GA::MissingJsonValue("Session:Livetime");
    mLivetime = root.value("Livetime").toInt();

    if(!root.contains("Iterations"))
        throw GA::MissingJsonValue("Session:Iterations");
    mIterations = root.value("Iterations").toInt();

    if(!root.contains("DetectorType"))
        throw GA::MissingJsonValue("Session:DetectorType");
    mDetectorType.loadJson(root.value("DetectorType").toObject());

    if(!root.contains("Detector"))
        throw GA::MissingJsonValue("Session:Detector");
    mDetector.loadJson(root.value("Detector").toObject());
}

void Session::loadDoserateScript(QString scriptFileName)
{
    if(luaL_dofile(L, scriptFileName.toStdString().c_str()))
        qDebug() << "luaL_dofile failed";
    else mScriptLoaded = true;
}

void Session::clear()
{
    for(auto spec : mSpecList)
        delete spec;

    mSpecList.clear();

    mMinDoserate = mMaxDoserate = 0.0;
    mMinX = mMinY = mMinZ = 0.0;
    mMaxX = mMaxY = mMaxZ = 0.0;
    mMinAltitude = mMaxAltitude = 0.0;
}

} // namespace Gamma
