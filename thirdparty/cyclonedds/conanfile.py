from conans import ConanFile, CMake

class ConanProduct(ConanFile):
    name = "cyclonedds"
    version = "0.10.3"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "txt"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    no_copy_source = True
    scm = {
        "type": "git",
        "url": "https://github.com/eclipse-cyclonedds/cyclonedds.git",
        "revision": "0.10.3"
    }
    
    def configure(self):
        return super().configure()

    def build_requirements(self):
        self.tool_requires("cmake/3.25.0")

    def source(self):
        return super().source()

    def requirements(self):
        self.requires("cunit/2.1-3")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.install()
