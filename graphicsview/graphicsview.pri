#/*******************************************************************************
#*                                                                              *
#* Author    :  Bakiev Damir                                                    *
#* Version   :  na                                                              *
#* Date      :  01 February 2020                                                *
#* Website   :  na                                                              *
#* Copyright :  Bakiev Damir 2016-2020                                          *
#*                                                                              *
#* License:                                                                     *
#* Use, modification & distribution is subject to Boost Software License Ver 1. *
#* http://www.boost.org/LICENSE_1_0.txt                                         *
#*                                                                              *
#*******************************************************************************/

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/edid.h \
    $$PWD/qdruler.h \
    $$PWD/graphicsview.h \
    $$PWD/scene.h

SOURCES += \
    $$PWD/edid.cpp \
    $$PWD/qdruler.cpp \
    $$PWD/graphicsview.cpp \
    $$PWD/scene.cpp
