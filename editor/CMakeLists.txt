project(RoomEdit)
cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/GUI ${CMAKE_CURRENT_BINARY_DIR})

set(redit_SRCS
        # editor
        main.cpp
        REditor.cpp
        # GUI
        GUI/REditorWnd.cpp
        GUI/RLogger.cpp
        GUI/RMainWnd.cpp
)

set(redit_MOC_HDRS
        # editor
        REditor.h
        # GUI
        GUI/REditorWnd.h
        GUI/RLogger.h
        GUI/RMainWnd.h
)
QT4_WRAP_CPP(redit_MOC_SRCS ${redit_MOC_HDRS})

IF(UNIX)
        add_executable(RoomEdit ${redit_SRCS} ${redit_MOC_SRCS})
ELSEIF(WIN32)
        add_executable(RoomEdit WIN32 ${redit_SRCS} ${redit_MOC_SRCS})
ENDIF()

target_link_libraries(RoomEdit ${QT_QTMAIN_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI} ${QT_QTOPENGL_LIBRARY})