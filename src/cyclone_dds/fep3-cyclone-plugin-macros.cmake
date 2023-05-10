
macro(fep_cyclone_dds_plugin_install NAME DESTINATION)
    fep3_participant_install(${NAME} ${DESTINATION})
    cyclone_install(${DESTINATION})
    install(
        FILES
            $<TARGET_FILE:fep3_cyclone_dds_plugin>
        DESTINATION ${DESTINATION}
    )
     # overwrite fep_components file
    install(
        FILES
            $<TARGET_FILE_DIR:fep3_cyclone_dds_plugin>/../fep3_participant.fep_components
        DESTINATION ${DESTINATION}
    )
endmacro(fep_cyclone_dds_install DESTINATION)

macro(fep_cyclone_dds_plugin_deploy NAME)
    fep3_participant_deploy(${NAME})
    add_custom_command(TARGET ${NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                $<TARGET_FILE_DIR:${NAME}>/cyclone
    )
    cyclone_deploy(${NAME} "$<TARGET_FILE_DIR:${NAME}>/cyclone")

    # no need to copy in build directory on linux since linker rpath takes care of that
    if (WIN32)
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:fep3_cyclone_dds_plugin>
            $<TARGET_FILE_DIR:${NAME}>/cyclone)
    endif()

    # always need a fep_components file for the native components plugin
    add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE_DIR:fep3_cyclone_dds_plugin>/fep3_participant.fep_components
            $<TARGET_FILE_DIR:${NAME}>
    )

endmacro(fep_cyclone_dds_plugin_deploy NAME)

macro(cyclone_install DESTINATION)
    if(MSVC)
        set(CYCLONEDDS-CXX_FILES_PATH ${CONAN_BIN_DIRS_CYCLONEDDS-CXX})
        set(CYCLONEDDS_FILES_PATH ${CONAN_BIN_DIRS_CYCLONEDDS})
        set(FILE_WILDCARD "*")
    else()
        set(CYCLONEDDS-CXX_FILES_PATH ${CONAN_LIB_DIRS_CYCLONEDDS-CXX})
        set(CYCLONEDDS_FILES_PATH ${CONAN_LIB_DIRS_CYCLONEDDS})
        set(FILE_WILDCARD "*.so")
    endif()

    install(DIRECTORY ${CYCLONEDDS-CXX_FILES_PATH}/ DESTINATION ${DESTINATION}
        FILES_MATCHING PATTERN ${FILE_WILDCARD})
    install(DIRECTORY ${CYCLONEDDS_FILES_PATH}/ DESTINATION ${DESTINATION}
        FILES_MATCHING PATTERN ${FILE_WILDCARD})
    install(FILES USER_QOS_PROFILES.xml DESTINATION ${DESTINATION})


endmacro(cyclone_install DESTINATION)

macro(cyclone_deploy NAME TARGET_FOLDER)
    if(MSVC)
        set(CYCLONEDDS-CXX_FILES_PATH ${CONAN_BIN_DIRS_CYCLONEDDS-CXX})
        set(CYCLONEDDS_FILES_PATH ${CONAN_BIN_DIRS_CYCLONEDDS})
    else()
        set(CYCLONEDDS-CXX_FILES_PATH ${CONAN_LIB_DIRS_CYCLONEDDS-CXX})
        set(CYCLONEDDS_FILES_PATH ${CONAN_LIB_DIRS_CYCLONEDDS})
        set(FILE_WILDCARD "*.so")
    endif()

    if(MSVC)
        add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CYCLONEDDS-CXX_FILES_PATH}/cycloneddsidlcxx$<$<CONFIG:Debug>:d>.dll
                ${CYCLONEDDS-CXX_FILES_PATH}/ddscxx$<$<CONFIG:Debug>:d>.dll
                ${CYCLONEDDS-CXX_FILES_PATH}/concrt140.dll
                ${CYCLONEDDS-CXX_FILES_PATH}/msvcp140.dll
                ${CYCLONEDDS-CXX_FILES_PATH}/msvcp140_1.dll
                ${CYCLONEDDS-CXX_FILES_PATH}/msvcp140_2.dll
                ${CYCLONEDDS-CXX_FILES_PATH}/msvcp140_atomic_wait.dll
                ${CYCLONEDDS-CXX_FILES_PATH}/msvcp140_codecvt_ids.dll
                ${CYCLONEDDS-CXX_FILES_PATH}/vcruntime140.dll
                ${CYCLONEDDS-CXX_FILES_PATH}/vcruntime140_1.dll
                ${CYCLONEDDS_FILES_PATH}/ddsc$<$<CONFIG:Debug>:d>.dll
                ${TARGET_FOLDER})
    else()
    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CYCLONEDDS-CXX_FILES_PATH}/${FILE_WILDCARD}
            ${TARGET_FOLDER})
    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CYCLONEDDS_FILES_PATH}/${FILE_WILDCARD}
            ${TARGET_FOLDER})
    endif()

    add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/USER_QOS_PROFILES.xml
                ${TARGET_FOLDER})
endmacro(cyclone_deploy NAME)

