TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lmysqlclient -lpthread -lhiredis
SOURCES += \
        Mysql.cpp \
        TCPKernel.cpp \
        Thread_pool.cpp \
        block_epoll_net.cpp \
        clogic.cpp \
        err_str.cpp \
        main.cpp \
        mysqlconnectionpool.cpp

HEADERS += \
    Mysql.h \
    TCPKernel.h \
    Thread_pool.h \
    block_epoll_net.h \
    clogic.h \
    err_str.h \
    mysqlconnectionpool.h \
    packdef.h


