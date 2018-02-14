project(Kaption)

cmake_minimum_required(VERSION 2.6.2)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(FeatureSummary)
include(ECMInstallIcons)

find_package(Qt5 REQUIRED Core Gui Widgets)
find_package(KF5Notifications REQUIRED)
find_package(KF5CoreAddons REQUIRED)
find_package(KF5DBusAddons REQUIRED)
find_package(KF5GlobalAccel REQUIRED)
find_package(KF5KDELibs4Support REQUIRED)

set(CMAKE_AUTOUIC ON)

add_subdirectory(src)
add_subdirectory(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
