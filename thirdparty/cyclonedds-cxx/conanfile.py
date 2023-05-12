from conans import ConanFile, CMake

class ConanProduct(ConanFile):
    name = "cyclonedds-cxx"
    version = "0.10.3"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "txt"
    options = {"shared": [True, False]}
    default_options = {"shared": True}
    no_copy_source = True
    scm = {
        "type": "git",
        "url": "https://github.com/eclipse-cyclonedds/cyclonedds-cxx.git",
        "revision": "0.10.3"
    }
    
    def configure(self):
        return super().configure()

    def build_requirements(self):
        self.tool_requires("cmake/3.25.0")

    def requirements(self):
        self.requires("boost/1.81.0", private=True)
        self.requires("gtest/1.12.1", private=True)
        self.requires("cyclonedds/0.10.3")

    def build(self):
        cmake = CMake(self)
        cmake.configure(defs={"BUILD_IDLLIB": "ON",
                               "BUILD_EXAMPLES": "ON",
                               "ENABLE_SHM": "NO" #TODO
                               })
        cmake.build()
        cmake.install()
