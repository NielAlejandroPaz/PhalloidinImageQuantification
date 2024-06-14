QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    phalloidinimagequantification.cpp

HEADERS += \
    phalloidinimagequantification.h

FORMS += \
    phalloidinimagequantification.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


#INCLUDEPATH += C:\opencv\build_mingw\install\include

#LIBS += C:\opencv\build_mingw\install\x64\mingw\bin\libopencv_core4100.dll
#LIBS += C:\opencv\build_mingw\install\x64\mingw\bin\libopencv_highgui4100.dll
#LIBS += D:\opencv-build\bin\libopencv_imgcodecs320.dll
#LIBS += D:\opencv-build\bin\libopencv_imgproc320.dll
#LIBS += D:\opencv-build\bin\libopencv_features2d320.dll
#LIBS += D:\opencv-build\bin\libopencv_calib3d320.dll

LIBS += $$PWD/../../../../opencv/build_mingw/install/x64/mingw/bin/libopencv_core4100.dll
LIBS += $$PWD/../../../../opencv/build_mingw/install/x64/mingw/bin/libopencv_highgui4100.dll
LIBS += $$PWD/../../../../opencv/build_mingw/install/x64/mingw/bin/libopencv_imgcodecs4100.dll
LIBS += $$PWD/../../../../opencv/build_mingw/install/x64/mingw/bin/libopencv_imgproc4100.dll


INCLUDEPATH += $$PWD/../../../../opencv/build_mingw/install/include
DEPENDPATH += $$PWD/../../../../opencv/build_mingw/install/include
