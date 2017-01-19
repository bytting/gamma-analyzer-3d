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
#include <stdexcept>
#include <memory>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

namespace gamma
{

Session::Session()
{
}

Session::~Session()
{
    try
    {
        clear();
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
    }
}

const Spectrum& Session::getSpectrum(unsigned int index) const
{
    if(index >= mSpecList.size())
        throw std::runtime_error(
                "Session::getSpectrum: Index out of bounds");

    return *mSpecList[index];
}

const SpecList& Session::getSpectrums() const
{
    return mSpecList;
}

Session::LoadResult Session::load(QString sessionPath)
{    
    auto res = LoadResult::Success;

    clear();

    QDir dir(sessionPath + QStringLiteral("/json"));

    if (!dir.exists())
        return LoadResult::DirNotASession;

    if(!QFile::exists(sessionPath + QStringLiteral("/session.json")))
        return LoadResult::DirNotASession;

    const auto entryInfoList = dir.entryInfoList(
                QStringList() << "*.json",
                QDir::NoDotAndDotDot | QDir::Files);

    for(const auto& info : entryInfoList)
    {        
        try
        {
            auto spec = new Spectrum(info.absoluteFilePath());
            mSpecList.push_back(spec);
        }
        catch(const std::exception& e)
        {
            qDebug() << e.what();
            res = LoadResult::InvalidSpectrumFound;
        }
    }

    return res;
}

void Session::clear()
{
    for(auto& spec : mSpecList)
        delete spec;

    mSpecList.clear();
}

} // namespace gamma
