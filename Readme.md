# FEP ECLIPSE CYCLONE DDS PLUGIN


## Building the project

The project is set up with Conan using the available public packages and build ign the missing ones
with custom conan files in the thirdparty directory.

    conan create thirdparty/cyclonedds/conanfile.py /_@_ -pr build_profiles/win_142
    conan create thirdparty/cyclonedds-cxx/conanfile.py /_@_ -pr build_profiles/win_142
    conan create thirdparty/dev_essential/conanfile.py /_@_ -pr build_profiles/win_142
    conan create thirdparty/fep3_participant/conanfile.py /_@_ -pr build_profiles/win_142

    conan create . -pr build_profiles/win_142

## Used products and sources

    * Current source is based on rti_dds_plugin of fep3_sdk_participant under MPL-2 license


