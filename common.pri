#    Call Recorder for SailfishOS
#    Copyright (C) 2014  Dmitriy Purgin <dpurgin@gmail.com>

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

PACKAGE=harbour-callrecorder
VERSION=0.7.4

TARGETDIR = /usr/share/$${PACKAGE}
TRANSLATIONSDIR = $${TARGETDIR}/translations
QMAKE_RPATHDIR += $${TARGETDIR}/lib

DEFINES += \
    TARGETDIR=\\\"$${TARGETDIR}\\\" \
    TRANSLATIONSDIR=\\\"$${TRANSLATIONSDIR}\\\" \
    VERSION=\\\"$${VERSION}\\\"
