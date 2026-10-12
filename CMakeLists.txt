# SPDX-FileCopyrightText: None
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.20)
project(kwin-keypulse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core DBus Gui Widgets)

# Prefer the official KWin CMake package when ECM is available.
find_package(ECM QUIET NO_MODULE)
set(KEYPULSE_HAVE_KWIN_PACKAGE FALSE)
if(ECM_FOUND)
    list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
    find_package(KWin QUIET)
    if(KWin_FOUND)
        set(KEYPULSE_HAVE_KWIN_PACKAGE TRUE)
    endif()
endif()

add_library(keypulse MODULE
    src/main.cpp
    src/keypulse_plugin.cpp
    src/keypulse_plugin.h
    src/metadata.json
)

target_link_libraries(keypulse PRIVATE
    Qt6::Core
    Qt6::DBus
    Qt6::Gui
    Qt6::Widgets
)

if(KEYPULSE_HAVE_KWIN_PACKAGE)
    target_link_libraries(keypulse PRIVATE KWin::kwin)
else()
    message(STATUS
        "ECM/KWin CMake package not found; using direct libkwin link fallback "
        "(optional later: extra-cmake-modules).")

    find_package(KF6Config REQUIRED)
    find_package(KF6CoreAddons REQUIRED)
    find_package(KF6WindowSystem REQUIRED)

    # Matches KWin::kwin INTERFACE_INCLUDE_DIRECTORIES on typical Plasma packages.
    target_include_directories(keypulse PRIVATE /usr/include/kwin)
    target_link_libraries(keypulse PRIVATE
        KF6::ConfigCore
        KF6::CoreAddons
        KF6::WindowSystem
        kwin
    )
endif()

# Default prefix ~/.local so the plugin can be installed without root.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local" CACHE PATH "Install prefix" FORCE)
endif()

include(GNUInstallDirs)
set(KEYPULSE_KWIN_PLUGIN_DIR "${CMAKE_INSTALL_PREFIX}/lib/qt6/plugins/kwin/plugins")
install(TARGETS keypulse LIBRARY DESTINATION "${KEYPULSE_KWIN_PLUGIN_DIR}")
install(FILES dbus/local.keypulse.Activity1.xml
        DESTINATION "${CMAKE_INSTALL_DATADIR}/dbus-1/interfaces")

message(STATUS "kwin-keypulse install dir: ${KEYPULSE_KWIN_PLUGIN_DIR}")
