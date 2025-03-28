QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

QMAKE_CXXFLAGS += -O0
QMAKE_CFLAGS += -O0

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mesh.cpp \
    model.cpp \
    openglshowwidget.cpp \
    openglwidget.cpp \
    showview.cpp

HEADERS += \
    mesh.h \
    model.h \
    openglshowwidget.h \
    openglwidget.h \
    showview.h

FORMS += \
    openglshowwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

LIBS += -LE:/EdgeDownload/assimp-5.3.0/build-assimp-5.3.0-Desktop_Qt_5_14_0_MinGW_64_bit-Debug/bin/ -lassimp-5d

INCLUDEPATH += E:/EdgeDownload/assimp-5.3.0/assimp-5.3.0/include
DEPENDPATH += E:/EdgeDownload/assimp-5.3.0/assimp-5.3.0/include
