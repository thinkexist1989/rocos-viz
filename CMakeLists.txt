cmake_minimum_required(VERSION 3.10)

project(rocos LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

# Qt
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

# VTK
find_package(VTK 8.2 REQUIRED )
include( ${VTK_USE_FILE} )

# Eigen3
find_package(Eigen3 REQUIRED)

# Yaml-cpp
find_package(yaml-cpp REQUIRED)

# orocos_kdl
find_package(orocos_kdl REQUIRED)

include_directories(include)
include_directories(src)

set(TS_FILES rocos_zh_CN.ts)
set(SRC_FILES
        src/main.cpp
        src/RocosMainWindow.cpp
        src/MouseInteractorChooseActor.cpp
        src/SceneWidget.cpp
        src/Model.cpp
        src/Link.cpp
        src/PositionBar.cpp
        src/JointPositionWidget.cpp
        src/CartesianPositionWidget.cpp
)
set(HRD_FILES
        src/RocosMainWindow.h
        src/MouseInteractorChooseActor.h
        src/SceneWidget.h
        src/Model.h
        src/Link.h
        src/PositionBar.h
        src/JointPositionWidget.h
        src/CartesianPositionWidget.h
)
set(UI_FILES
        src/RocosMainWindow.ui
        src/JointPositionWidget.ui
        src/CartesianPositionWidget.ui
)

set(RC_FILES
        res.qrc)

add_executable(rocos
                    ${SRC_FILES}
                    ${HDR_FILES}
                    ${UI_FILES}
                    ${RC_FILES}
                    ${TS_FILES}
)


target_link_libraries(rocos
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${VTK_LIBRARIES} Eigen3::Eigen yaml-cpp orocos-kdl)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

#configure_file(${MODELS} ./ COPYONLY)
file(COPY models/ DESTINATION models/)
