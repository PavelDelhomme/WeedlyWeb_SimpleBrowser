cmake_minimum_required(VERSION 3.16)
project(simplebrowser LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebEngineWidgets Network Sql)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLite3 REQUIRED sqlite3)


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/browser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/downloads
    ${CMAKE_CURRENT_SOURCE_DIR}/src/favorites
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database
)

set(SOURCES
    src/main.cpp
    src/browser/browser.cpp
    src/browser/browserwindow.cpp
    src/browser/tabwidget.cpp
    src/browser/webview.cpp
    src/browser/webpage.cpp
    src/browser/webpopupwindow.cpp
    src/downloads/downloadmanagerwidget.cpp
    src/downloads/downloadwidget.cpp
    src/favorites/favoritesmanager.cpp
    src/utils/commandwidget.cpp
    src/utils/cveanalyzer.cpp
    src/utils/requestinterceptor.cpp
    src/browser/certificateerrordialog.cpp
    src/browser/passworddialog.cpp
    src/browser/webauthdialog.cpp
    src/database/database.cpp
)

set(HEADERS
    src/browser/browser.h
    src/browser/browserwindow.h
    src/browser/tabwidget.h
    src/browser/webview.h
    src/browser/webpage.h
    src/browser/webpopupwindow.h
    src/downloads/downloadmanagerwidget.h
    src/downloads/downloadwidget.h
    src/favorites/favoritesmanager.h
    src/utils/commandwidget.h
    src/utils/cveanalyzer.h
    src/utils/requestinterceptor.h
    src/browser/certificateerrordialog.h
    src/browser/passworddialog.h
    src/browser/webauthdialog.h
    src/database/database.h
)

qt_add_executable(simplebrowser
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(simplebrowser PRIVATE
    Qt::Core
    Qt::Gui
    Qt::WebEngineWidgets
    Qt::Network
    Qt::Sql
    )


# Resources
set(RESOURCE_FILES
    resources/data/AppLogoColor.png
    resources/data/ninja.png
    resources/data/3rdparty/dialog-error.png
    resources/data/3rdparty/edit-clear.png
    resources/data/3rdparty/go-bottom.png
    resources/data/3rdparty/go-next.png
    resources/data/3rdparty/go-previous.png
    resources/data/3rdparty/process-stop.png
    resources/data/3rdparty/text-html.png
    resources/data/3rdparty/view-refresh.png
    resources/data/3rdparty/star-regular.png
    resources/data/3rdparty/star-solid.png
    resources/data/3rdparty/folder-opened.png
    resources/data/3rdparty/folder-closed.png
    resources/icons/star-filled.png
    resources/icons/star-unfilled.png
    resources/icons/more.png
)

qt_add_resources(simplebrowser "resources"
    PREFIX "/"
    FILES ${RESOURCE_FILES}
)