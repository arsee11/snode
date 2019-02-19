#-------------------------------------------------
#
# Project created by QtCreator 2018-08-27T23:26:41
#
#-------------------------------------------------

QT       -= core gui

TARGET = snode
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /home/arsee/gits

SOURCES += \
    ../src/sn_udpport.cpp \
    ../src/sn_udptransport.cpp \
    ../src/sn_route_table.cpp \
    ../src/sn_transport_manager.cpp \
    ../src/sn_message.cpp

HEADERS += \
    ../src/sn_port.h \
    ../src/sn_route_table.h \
    ../src/sn_router.h \
    ../src/sn_address.h \
    ../src/sn_message.h \
    ../src/sn_super_node.h \
    ../src/sn_rip_routing.h \
    ../src/sn_routing.h \
    ../src/sn_node.h \
    ../src/sn_udptransport.h \
    ../src/sn_transport_manager.h \
    ../src/sn_transport.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
