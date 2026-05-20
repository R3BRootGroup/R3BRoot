from conan import ConanFile
from conan.tools.cmake import CMakeToolchain

class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"

    def requirements(self):
        self.requires("ms-gsl/4.0.0")  # type: ignore
        self.requires("range-v3/0.12.0")  # type: ignore
        self.requires("gtest/cci.20210126")  # type: ignore
        self.requires("nlohmann_json/3.12.0")  # type: ignore
        self.requires("magic_enum/0.9.7")  # type: ignore
        self.requires("cli11/2.5.0")  # type: ignore
        self.requires("re2/20240702")  # type: ignore
        # self.requires("glaze/6.4.0") # type: ignore

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False  # type: ignore
        tc.generate()
