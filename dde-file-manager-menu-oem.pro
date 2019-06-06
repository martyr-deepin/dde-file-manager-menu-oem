# Still need the widgets module since MenuInterface need it.
QT       += widgets

CONFIG   += plugins link_pkgconfig
PKGCONFIG += dde-file-manager Qt5Xdg

TARGET = dde-file-manager-menu-oem-plugin
TEMPLATE = lib

DEFINES += DFMMENUOEMPLUGIN_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        dfmoemmenuplugin.cpp

HEADERS += \
        dfmoemmenuplugin.h

unix {
    target.path = $$[QT_INSTALL_LIBS]/dde-file-manager/plugins/menu/
    INSTALLS += target
}
