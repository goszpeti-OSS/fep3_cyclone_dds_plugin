
################################################################################
## \page page_cmake_commands
# <hr>
# <b>cyclone_install(\<destination\>)</b>
#
# This macro installs the cyclone libraries and the 'USER_QOS_PROFILES.xml' to the
#   folder \<destination\>. The macro expects the 'USER_QOS_PROFILES.xml' to lay
#   next the the 'CMakeLists.txt' calling this macro.
#
# Arguments:
# \li \<destination\>:
# The relative path to the install subdirectory
################################################################################
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


################################################################################
## \page page_cmake_commands
# <hr>
# <b>cyclone_deploy(\<name\>)</b>
#
# This macro copies the cyclone libraries to the location next to the target
#   \<name\>. Furthermore it copies the 'USER_QOS_PROFILES.xml' which is
#   expected to lay next to the 'CMakeLists.txt' which called that macro.
#
# Arguments:
# \li \<name\>:
# The name of the target that needs the cyclone libraries.
################################################################################
macro(cyclone_deploy NAME)
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
                $<TARGET_FILE_DIR:${NAME}>)
    else()
    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CYCLONEDDS-CXX_FILES_PATH}/${FILE_WILDCARD}
            $<TARGET_FILE_DIR:${NAME}>)
    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CYCLONEDDS_FILES_PATH}/${FILE_WILDCARD}
            $<TARGET_FILE_DIR:${NAME}>)
    endif()

    add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${CMAKE_CURRENT_SOURCE_DIR}/USER_QOS_PROFILES.xml
                $<TARGET_FILE_DIR:${NAME}>)
endmacro(cyclone_deploy NAME)

