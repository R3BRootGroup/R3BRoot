#! /usr/bin/env python3
import argparse
import os
import subprocess
import sys

# import sys


class Build:
    def __init__(self):
        self.source_dir = os.path.dirname(os.path.abspath(sys.argv[0]))
        self.define_options()
        self.args = self.parser.parse_args()

    def launch(self):
        if not os.path.isdir(self.args.build_dir):
            os.mkdir(self.args.build_dir)
        self.install_conan_packages()
        self.cmake_configure()
        self.build_source()

    def define_options(self):
        self.parser = argparse.ArgumentParser(
            prog="R3BRoot builder",
            description="Building R3BRoot source files with conan package manager",
            epilog="If any issue occurs, please report it to the R3BRoot github webpage.",
        )
        self.parser.add_argument(
            "-j",
            type=int,
            nargs="?",
            default=1,
            const=1,
            dest="build_thread",
            help="number of threads for the building (default: 1)",
        )
        self.parser.add_argument(
            "--fresh",
            dest="is_fresh_configure",
            action="store_true",
            help="fresh build for cmake",
        )
        self.parser.add_argument(
            "-a",
            "--all",
            dest="is_all",
            action="store_true",
            help="run all processes specified with -p, -c, -b",
        )
        self.parser.add_argument(
            "-p",
            "--only-packages",
            dest="is_only_package",
            action="store_true",
            help="only install all conan packages",
        )
        self.parser.add_argument(
            "-c",
            "--only-configure",
            dest="is_only_configure",
            action="store_true",
            help="only run the cmake configuration",
        )
        self.parser.add_argument(
            "-b",
            "--only-build",
            dest="is_only_build",
            action="store_true",
            help="only build the source files",
        )
        self.parser.add_argument(
            "--build-dir",
            default=f"{self.source_dir}/build",
            help='specify the build folder (default: "./build")',
        )
        self.parser.add_argument(
            "--build-type",
            default="RelWithDebInfo",
            help='specify the build type for cmake (default: "RelWithDebInfo")',
        )
        self.parser.add_argument(
            "--cmake-extra-args",
            default="-DBUILD_GEOMETRY=OFF",
            dest="cmake_args",
            help='extra colon separated arguments for cmake configurations (default: "-DBUILD_GEOMETRY=OFF")',
        )
        self.parser.add_argument(
            "--conan-dir",
            default="conan-packages",
            help='specify the folder name for conan packages (default: "conan-packages")',
        )

    def install_conan_packages(self):
        if self.args.is_only_package or self.args.is_all:
            subprocess.run(["conan", "profile", "detect", "--force"], check=True)
            subprocess.run(
                [
                    "conan",
                    "install",
                    self.source_dir,
                    f"--output-folder={self.args.build_dir}/{self.args.conan_dir}",
                    "--build=missing",
                    f"--settings=build_type={self.args.build_type}",
                ],
                check=True,
            )  # nosec

    def cmake_configure(self):
        if self.args.is_only_configure or self.args.is_all:
            cmake_args = ["--fresh"] if self.args.is_fresh_configure else []
            cmake_args.append(f"-S {self.source_dir}")
            cmake_args.append(f"-B {self.args.build_dir}")
            cmake_args.append("-DUSE_DIFFERENT_COMPILER=TRUE")
            cmake_args.append(
                f"-DCMAKE_TOOLCHAIN_FILE={self.args.build_dir}/{self.args.conan_dir}/conan_toolchain.cmake"
            )
            cmake_args.append(f"-DCMAKE_BUILD_TYPE={self.args.build_type}")
            extra_args = self.args.cmake_args.split(";")
            cmake_args.extend(extra_args)
            print(f"build.py: cmake {cmake_args}")
            subprocess.run(["cmake"] + cmake_args, check=True)  # nosec

    def build_source(self):
        if self.args.is_only_build or self.args.is_all:
            subprocess.run(
                ["cmake", "--build", self.args.build_dir, "--", "-j", str(self.args.build_thread)], check=True
            )  # nosec


if __name__ == "__main__":
    builder = Build()
    builder.launch()
