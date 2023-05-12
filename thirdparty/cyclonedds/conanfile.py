from pathlib import Path
from conans import ConanFile, CMake
from conan.tools.files import patch

class ConanProduct(ConanFile):
    name = "cyclonedds"
    version = "0.10.3"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "txt", "CMakeDeps"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    exports_sources = "patch1.diff"
    no_copy_source = True
    scm = {
        "type": "git",
        "url": "https://github.com/eclipse-cyclonedds/cyclonedds.git",
        "revision": "0.10.3"
    }
    
    def configure(self):
        return super().configure()

    def build_requirements(self):
        if self.settings.arch != "armv8":
            self.tool_requires("cmake/3.25.0")

    def requirements(self):
        self.requires("cunit/2.1-3")
        # TODO: Enable for linux
        # self.requires("iceoryx/2.0.2")

    def build(self):
        patch_str = (Path(self.source_folder) / "patch1.diff").read_text()
        patch(self, patch_string=patch_str)
        cmake = CMake(self)
        cmake.configure(defs={"ENABLE_SSL": "NO",
                              "ENABLE_SHM": "NO", # TODO: Enable for linux
                              "ENABLE_SECURITY": "NO",                 
                              } )
        cmake.build()
        cmake.install()
