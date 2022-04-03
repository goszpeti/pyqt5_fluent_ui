import os
import sys
from pathlib import Path

from conans import ConanFile, CMake, tools
# from conan import tools
class PyQt5FluentUiConan(ConanFile):
    name = "pyqt5_fluent_ui"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "txt", "virtualenv", "qt", "cmake_find_package_multi"
    default_options = {"qt:shared": True}
    default_user = "goszpeti"
    default_channel = "testing"

    def configure(self):
        return super().configure()

    def build_requirements(self):
        self.tool_requires("cmake/3.22.0")

    def requirements(self):
        self.requires("qt/5.15.2", private=True)

    def build(self):
        import PyQt5
        import pkg_resources
        bindings_path = Path(pkg_resources.resource_filename('PyQt5', 'bindings'))
        cmake = CMake(self)
        cmake.configure()
        #cmake.build()
        #cmake.install()
        # use pip 21.0
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
                os.system("sip-wheel --verbose")
        os.chdir(self.build_folder)
        
        #os.system(f"{sys.executable} -m pip install {self.source_folder}")
        #os.system("sip-wheel")