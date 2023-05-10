set(CMAKE_BUILD_TYPE ${CONAN_SETTINGS_BUILD_TYPE})  #
#####################################################

# Execute (now hacked) find package script

include(fep3-cyclone-plugin-macros.cmake)

set(PLUGIN_NAME fep3_cyclone_dds_plugin)
IDLCXX_GENERATE(TARGET ${PLUGIN_NAME}_types FILES "types/stream_types.idl") # WARNINGS no-implicit-extensibility)
add_library(${PLUGIN_NAME} SHARED
            simulation_bus/topic_intf.h
            simulation_bus/converter.h
            simulation_bus/converter.cpp
            simulation_bus/vector_raw_memory.hpp

            simulation_bus/internal_topic/internal_topic.h
            simulation_bus/internal_topic/internal_topic.cpp

            simulation_bus/stream_item_topic/stream_item_topic.h
            simulation_bus/stream_item_topic/stream_item_topic.cpp

            simulation_bus/stream_item_topic/stream_item_writer.h
            simulation_bus/stream_item_topic/stream_item_writer.cpp

            simulation_bus/stream_item_topic/stream_item_reader.h
            simulation_bus/stream_item_topic/stream_item_reader.cpp

            simulation_bus/reader_item_queue.h
            simulation_bus/reader_item_queue.cpp

            simulation_bus/fep_cyclone_dds_simulation_bus.cpp
            simulation_bus/fep_cyclone_dds_simulation_bus.h

            fep_cyclone_dds_plugin.cpp

            simulation_bus/bus_info/bus_info.h
            simulation_bus/bus_info/bus_info.cpp
            )

set_target_properties(${PLUGIN_NAME} PROPERTIES FOLDER "plugins/cpp")
target_compile_definitions(${PLUGIN_NAME} PRIVATE BUILD_SIMBUS)

target_link_libraries(${PLUGIN_NAME} PRIVATE
            CycloneDDS-CXX::ddscxx
            fep3_participant_cpp_plugin
            dev_essential::pkg_rpc
            dev_essential::filesystem
            dev_essential::system
            ${PLUGIN_NAME}_types
            )

if(MSVC)
    # warning coming from dds headers
    target_compile_options(${PLUGIN_NAME} PRIVATE /wd4245)
    target_link_libraries(${PLUGIN_NAME} PRIVATE "Iphlpapi.lib")
    install(FILES $<TARGET_PDB_FILE:${PLUGIN_NAME}> DESTINATION lib/cyclone OPTIONAL)
endif()

add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/fep3_participant.fep_components
        $<TARGET_FILE_DIR:${PLUGIN_NAME}>
)

cyclone_deploy(${PLUGIN_NAME} $<TARGET_FILE_DIR:${PLUGIN_NAME}>)
cyclone_install(lib/cyclone_dds)

install(TARGETS ${PLUGIN_NAME}
        EXPORT ${PLUGIN_NAME}_targets
        LIBRARY NAMELINK_SKIP DESTINATION lib/cyclone_dds
        RUNTIME DESTINATION lib/cyclone_dds)

install(EXPORT ${PLUGIN_NAME}_targets DESTINATION lib/cmake)

install(FILES 
        fep3_participant.fep_components
        fep3-cyclone-plugin-macros.cmake
        DESTINATION lib
)
