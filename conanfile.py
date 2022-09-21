import os
import sys
from pathlib import Path

from conans import ConanFile, CMake, tools
# from conan import tools
class PyQt5FluentUiConan(ConanFile):
    name = "pyqt5_fluent_ui"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "txt", "qt", "CMakeDeps" #"cmake_multi", 
    default_options = {"qt:shared": True}
    default_user = "goszpeti"
    default_channel = "testing"

    def configure(self):
        return super().configure()

    def build_requirements(self):
        self.tool_requires("cmake/3.22.0")

    def requirements(self):
        self.requires("qt/5.15.5", private=True)
        # TODO pip install PyQt-builder

    def build(self):
        cmake = CMake(self)
        cmake.definitions["CMAKE_BUILD_TYPE"] = str(self.settings.build_type).upper()
        # --warn-uninitialized
        cmake.configure()
        cmake.build()
        cmake.install()
        self._build_python_bindings()

    def _build_python_bindings(self):
        # use pip 21.0?
        import pkg_resources
        bindings_path = Path(pkg_resources.resource_filename('PyQt5', 'bindings'))
        os.environ["PATH"] +=  ";" + self.deps_cpp_info["qt"].bin_paths[0]
        with open(self.source_folder + "/pyproject.toml.in", "r") as sfd:
            pyproject_content = sfd.read()
            pyproject_content = pyproject_content.replace("${SIP_INCLUDE_DIR}", bindings_path.as_posix())
            with open(self.source_folder + "/pyproject.toml", "w") as dfd:
                dfd.write(pyproject_content)

        # tools.replace_in_file(self.source_folder + "/pyproject.toml",, output=self.build_folder + "/pyproject.toml")
        #os.chdir(self.source_folder)
        #os.removedirs(self.source_folder + "/build_sip")
        with tools.chdir(self.source_folder):
            with tools.vcvars(self):
                os.system("pip install . --verbose")
                #os.system("sip-wheel --verbose")
        os.chdir(self.build_folder)
        
        #os.system(f"{sys.executable} -m pip install {self.source_folder}")
        #os.system("sip-wheel")