from conans import ConanFile, CMake

class CpplogConan(ConanFile):
    name = "cpplog"
    version = "0.0.1"
    settings = "os", "compiler", "build_type", "arch"
    exports = "CMakeLists.txt", "cpplog*", "test*"
    requires = "fmt/3.0.0@memsharded/testing", \
               "nlJson/2.1.0@xgzeng/testing"
    generators = "cmake"
    
    def run(self, cmd):
        self.output.info("Running: %s" % cmd)
        ConanFile.run(self, cmd)

    def build(self):
        cmake = CMake(self.settings)
#        self.run('conan install %s --build' % (self.conanfile_directory))
        self.run('cmake %s %s' % (self.conanfile_directory, cmake.command_line))
        self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include")

#    def package_info(self):
#        pass

