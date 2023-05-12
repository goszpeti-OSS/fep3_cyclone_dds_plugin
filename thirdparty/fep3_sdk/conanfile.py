from pathlib import Path
from conans import ConanFile, CMake
from conan.tools.files import patch

class ConanProduct(ConanFile):
    name = "fep_sdk"
    version = "3.1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "txt"
    options = {"fPIC": [True, False]}
    default_options = {"fPIC": True, 
                       "boost:without_date_time":False,
                       "boost:without_filesystem":False}
    no_copy_source = True
    short_paths = True
    exports_sources = "patch1.diff"
    no_copy_export_sources = True
    scm = {
        "type": "git",
        "url": "https://github.com/cariad-tech/fep3_sdk.git",
        "revision": "v3.1.0"
    }
    
    def configure(self):
        return super().configure()

    def build_requirements(self):
        if self.settings.arch != "armv8":
            self.tool_requires("cmake/3.25.0")

    def source(self):
        super().source()
        return 

    def requirements(self):
        self.requires("fep_sdk_system/3.1.0")
        self.requires("fep_sdk_participant/3.1.0")
        self.requires("boost/1.81.0")

    def build(self):
        patch_str = (Path(self.source_folder) / "patch1.diff").read_text()
        patch(self, patch_string=patch_str)

        cmake = CMake(self)
        cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = self.options.fPIC
        cmake.configure(defs={"fep3_sdk_cmake_enable_documentation": "OFF", 
                              "fep3_sdk_cmake_enable_tests": "OFF",
                              "fep3_sdk_cmake_enable_functional_tests": "OFF",
                              "fep3_sdk_cmake_enable_private_tests": "OFF",
                              "Boost_USE_STATIC_LIBS": "On",
                             }
                        )
        cmake.build()
        cmake.install()
