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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

find_package(VTK REQUIRED)
include( ${VTK_USE_FILE} )

include_directories(include)
include_directories(src)

set(TS_FILES rocos_zh_CN.ts)
set(SRC_FILES
        src/main.cpp
        src/RocosMainWindow.cpp
        src/MouseInteractorChooseActor.cpp
)
set(HRD_FILES
        src/RocosMainWindow.h
        src/MouseInteractorChooseActor.h
)
set(UI_FILES
        src/RocosMainWindow.ui
)

add_executable(rocos
                    ${SRC_FILES}
                    ${HDR_FILES}
                    ${UI_FILES}
                    ${TS_FILES}
)


target_link_libraries(rocos PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${VTK_LIBRARIES})

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
