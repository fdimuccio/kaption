set(KAPTION_ICONS
    16-actions-toolbox_arrow.png
    16-actions-toolbox_box.png
    16-actions-toolbox_ellipse.png
    16-actions-toolbox_number.png
    16-actions-toolbox_text.png
    16-actions-upload.png
    32-actions-toolbox_arrow.png
    32-actions-toolbox_box.png
    32-actions-toolbox_ellipse.png
    32-actions-toolbox_number.png
    32-actions-toolbox_text.png
    32-actions-upload.png
    48-actions-upload.png
)

ecm_install_icons(
    ICONS ${KAPTION_ICONS}
    DESTINATION ${DATA_INSTALL_DIR}/kaption/icons
)
