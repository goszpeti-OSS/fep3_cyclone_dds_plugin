# FEP3 Eclipse Cyclone DDS Simulation Bus Plugin

This project aim to add an open source simulation bus to the FEP3 SDK for the purpose of showcasing its capabilities. It does not aim to be a fully featured and fast implementation.

The current source is based on rti_dds_plugin of fep3_sdk_participant under MPL-2 license.

## Features and known limitations

What is working?

* Participant with system name isolation through DDS domain tags
* Metadata as user info on bultin participant topic
* Reading and writing DDS topics, though not very performant

Dynamic stream metatype change is currently not working.
Possible issues with timing where not analized yet. 

# Qos

Only via the CYCLONEDDS_URI environment varaible. See the USER_QOS_PROFILES.xml.
This is not set per default, so there is no differentiation between small and large or other kinds of sample types.

## Using the Adapter

### For simulations

A *fep3_participant.fep_components* file is provided in the lib folder, so you only need to copy it and the *cyclone_dds'* folder beside your 
fep3_particpant shared library and you're ready to go!

### Developing participants - CMake macros

There are analogous macros to FEP3 deploy and install:

    fep_cyclone_dds_plugin_deploy(TARGET)
    fep_cyclone_dds_plugin_install(TARGET DESTINATION)

They will copy the FEP3 participant, the Cyclone DDS plugin and its dependecies and
a modified fep_components file.

## Building the project

The project is set up with Conan using the available public packages and building the missing ones with custom conan files in the thirdparty directory.

    conan create thirdparty/cyclonedds/conanfile.py /_@_ -pr build_profiles/win_142
    conan create thirdparty/cyclonedds-cxx/conanfile.py /_@_ -pr build_profiles/win_142
    conan create thirdparty/dev_essential/conanfile.py /_@_ -pr build_profiles/win_142
    conan create thirdparty/fep3_participant/conanfile.py /_@_ -pr build_profiles/win_142
    conan create thirdparty/fep3_system/conanfile.py /_@_ -pr build_profiles/win_142
    conan create thirdparty/fep3_base_utilities/conanfile.py /_@_ -pr build_profiles/win_142
    conan create thirdparty/fep3_sdk/conanfile.py /_@_ -pr build_profiles/win_142

    conan create . -pr build_profiles/win_142

## Used products and sources

Only first level dependencies are listed.

* cyclonedds and cyclonedds-cxx by Eclipse under EPL 2.0
* dev_essential 1.20, fep_sdk 3.1.0 by CARIAD under MPL-2.0
* boost 1.73 under BSL-1.0
* gtest/1.10.0 under BSD-3-Clause

## Tested compilers

* Visual Studio MSVC 142
* gcc TODO


