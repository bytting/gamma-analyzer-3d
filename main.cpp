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
#include <exception>
#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    try
    {
        QApplication a(argc, argv);
        gamman3d w;
        w.show();
        return a.exec();
    }
    catch(const std::exception& e)
    {
        qDebug() << e.what();
        return 1;
    }
}
