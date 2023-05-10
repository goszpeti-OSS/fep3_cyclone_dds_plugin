from conans import ConanFile, CMake

class ConanProduct(ConanFile):
    name = "fep_cyclone_dds_plugin"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "txt", "CMakeDeps"
    default_user = "local"
    default_channel = "testing"
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
    }

    def configure(self):
        return super().configure()

    def build_requirements(self):
        self.tool_requires("cmake/3.25.0")

    def requirements(self):
        self.requires("fep_sdk_participant/3.1.0", private=True)
        self.requires("cyclonedds-cxx/0.10.3", private=True)
        self.requires("gtest/1.10.0", private=True)

    def build(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_BUILD_TYPE"] = str(self.settings.build_type).upper()
        cmake.configure(defs={"FEP_CYCLONE_DDS_VERSION": self.version, 
                              "ENABLE_TESTS": "False",
                              "Boost_USE_STATIC_LIBS": "On",
                             }
                        )
        cmake.build()
        cmake.test()
        cmake.install()
