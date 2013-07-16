add_subdirectory(icons)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
#   ${QT_QTNETWORK_INCLUDE_DIR}
#   ${QT_QTDBUS_INCLUDE_DIR}
#   ${QT_QTSQL_INCLUDE_DIR}
#   ${QT_QTWEBKIT_INCLUDE_DIR}
  ${KDE4_INCLUDES}
#   ${SOPRANO_INCLUDE_DIR}
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

# Compile UI files
kde4_add_ui_files(KAPTION_SRCS
    snapshotpreview.ui
    settings/generalsettingspage.ui
    settings/ftpuploadsettingspage.ui
)

# Compile KCFG files
kde4_add_kcfg_files(KAPTION_SRCS settings.kcfgc)

# Create the executable
kde4_add_executable(kaption ${KAPTION_SRCS})

# Link against the needed libraries
target_link_libraries(kaption
#   ${QT_QTNETWORK_LIBRARY}
#   ${QT_QTDBUS_LIBRARY}
#   ${QT_QTSQL_LIBRARY}
#   ${QT_QTWEBKIT_LIBRARY}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${KDE4_KFILE_LIBS}
#   ${KDE4_KPARTS_LIBS}
#   ${KDE4_KNEWSTUFF3_LIBS}
#   ${KDE4_PHONON_LIBS}
#   ${KDE4_SOLID_LIBS}
#   ${KDE4_NEPOMUK_LIBS}
#   ${SOPRANO_LIBRARIES}
)

install(TARGETS kaption ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kaption.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES kaption.kcfg DESTINATION ${KCFG_INSTALL_DIR})
install(FILES data/kaption.notifyrc DESTINATION ${DATA_INSTALL_DIR}/kaption)

macro_display_feature_log()


