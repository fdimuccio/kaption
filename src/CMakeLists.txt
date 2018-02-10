add_subdirectory(icons)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    graphic/
    graphic/items
    configdialog/
)

# Insert your sources here
set(KAPTION_SRCS
    snapshotpreview.cpp
    grabber.cpp
    trayicon.cpp
    main.cpp
    kaptionapplication.cpp
    saveimagedirselectdialog.cpp
    graphicsitemfactory.cpp
    kaptiongraphicstoolkit.cpp
    propertytooleditor.cpp
    colorpropertytooleditor.cpp
    scalepropertytooleditor.cpp
    fontpropertytooleditor.cpp
    numberpropertytooleditor.cpp
    numberselector.cpp
    graphic/snapshotcanvas.cpp
    graphic/items/kaptiongraphicsitem.cpp
    graphic/items/abstractgraphicsrectitem.cpp
    graphic/items/arrowgraphicsitem.cpp
    graphic/items/boxgraphicsitem.cpp
    graphic/items/textgraphicsitem.cpp
    graphic/items/boxtextgraphicsitem.cpp
    graphic/items/ellipsegraphicsitem.cpp
    graphic/items/numbereditem.cpp
    graphic/items/handlegraphicsitem.cpp
    settings/kaptionsettingsdialog.cpp
    settings/ftpuploadsettingspage.cpp
    settings/generalsettingspage.cpp
    uploadprogressdialog.cpp
    scale.cpp
)

## Compile KCFG files
kconfig_add_kcfg_files(KAPTION_SRCS settings.kcfgc)

# Create the executable
add_executable(kaption ${KAPTION_SRCS})

# Link against the needed libraries
target_link_libraries(kaption
    KF5::Notifications
    KF5::CoreAddons
    KF5::KDELibs4Support
    Qt5::Widgets
    Qt5::Gui
    Qt5::Core
)

install(TARGETS kaption DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kaption.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES kaption.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES data/kaption.notifyrc DESTINATION ${DATA_INSTALL_DIR}/kaption)


