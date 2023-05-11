from conans import ConanFile, CMake

class ConanProduct(ConanFile):
    name = "dev_essential"
    version = "1.2.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "txt" 
    options = {"fPIC": [True, False]}
    default_options = {"fPIC": True}
    scm = {
        "type": "git",
        "url": "https://github.com/cariad-tech/dev_essential.git",
        "revision": "v1.2.0"
    }

    def build_requirements(self):
        self.tool_requires("cmake/3.25.0")

    def build(self):
        cmake = CMake(self)
        cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = self.options.fPIC
        cmake.configure()
        cmake.build()
        cmake.install()
