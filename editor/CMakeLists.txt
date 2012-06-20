project(RoomEdit)
cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/GUI ${CMAKE_CURRENT_BINARY_DIR})

set(redit_SRCS
        # editor
        main.cpp
        REditor.cpp
        RModel3DS.cpp
        RTexture.cpp
        RResourceDB.cpp
        # general objects
        REditObj.cpp
        RGrid.cpp
        # grid objects
        RGridObj.cpp
        RObjRoom.cpp
        RObjSelection.cpp
        RSceneObj.cpp
        # GUI
        GUI/REditWnd.cpp
        GUI/RLogger.cpp
        GUI/RMainWnd.cpp
        GUI/RSceneObjPicker.cpp
        # other
        RConfig.cpp
)

set(redit_MOC_HDRS
        # editor
        REditor.h
        # GUI
        GUI/REditWnd.h
        GUI/RLogger.h
        GUI/RMainWnd.h
        GUI/RSceneObjPicker.h
)
QT4_WRAP_CPP(redit_MOC_SRCS ${redit_MOC_HDRS})

# prepare resources
set(app_RES editor_resources.qrc)
QT4_ADD_RESOURCES(app_RES_C ${app_RES})

IF(UNIX)
        add_executable(RoomEdit ${redit_SRCS} ${redit_MOC_SRCS} ${app_RES_C})
ELSEIF(WIN32)
        add_executable(RoomEdit WIN32 ${redit_SRCS} ${redit_MOC_SRCS} ${app_RES_C})
ENDIF()

target_link_libraries(RoomEdit ${QT_QTMAIN_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})