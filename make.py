#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import re
import sys
import shutil
import subprocess
from os import mkdir, makedirs, rename
from colorama import init as c_init, Fore
from os.path import abspath, normpath, commonpath, basename, dirname, splitext, exists

# <configuration>
CPU_F = "2000000UL"
CC = "/usr/local/bin/sdcc"
CFLAGS = ["-lstm8", "-mstm8", "--std-sdcc11", "-DF_CPU=" + CPU_F]
FLASH = "/usr/local/bin/stm8flash"
FLASHFLAGS = ["-c", "stlinkv2", "-p", "stm8s103?3"]


# </configuration>

# please do NOT modify after this line, unless you know what you are doing

def print_info(string):
    print(Fore.BLUE + string)


def print_verb(string):
    print(Fore.YELLOW + string)


def print_err_and_quit(string):
    print(Fore.RED + string + "\nQuitting")
    quit()


def print_usage_and_quit(targets):
    print("Syntax: ", end="")
    print_info("./make.py [target] [main_src]")
    print("Available targets are: ", end="")
    print_info(", ".join(targets))
    quit()


def replace_extension(file, ext):
    return splitext(file)[0] + "." + ext


def get_used_files_by_file(filename):
    if not exists(filename):
        return set()

    with open(filename, "r") as f:
        content = f.read()

    # remove any /* */ and // block
    content = re.sub(r"(/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*)", "", content)

    # get file path
    path = dirname(filename) + "/"

    # regex to parse #include "something.h"
    pattern = re.compile(r'#include\s*"\s*([^"]+\.[Hh])\s*"')
    # h = set of used headers
    h = set([normpath(path + item.lower()) for item in re.findall(pattern, content)])

    c = set()

    for f in h:
        c_file = replace_extension(f, "c")

        if exists(c_file):  # there exists an associated .c file?
            c.add(c_file)

    return h | c


def get_used_c_files(starting_file):
    print_verb("Generating used files list...")
    starting_file = abspath(starting_file)
    current_file = starting_file
    completed = set()  # analyzed files
    to_be_analyzed = set()  # files to be analyzed

    while True:
        print_verb("Found " + current_file)
        completed.add(current_file)  # analyze the file
        if current_file in to_be_analyzed:
            to_be_analyzed.remove(current_file)

        # get used files
        s = get_used_files_by_file(current_file)

        # eventually add new items to to_be_analyzed
        for f in s:
            if f not in completed:
                to_be_analyzed.add(f)

        if len(to_be_analyzed) == 0:  # no more file?
            break
        else:
            current_file = to_be_analyzed.pop()

    assert len(to_be_analyzed) == 0
    print_verb("Done generating used files list")

    # now we can strip all .h files
    return [item for item in completed if item.endswith(".c") and item != starting_file]


class Maker:
    def __init__(self, main_src="main.c"):
        self.main_src = abspath(main_src)  # starting source file
        self.out_dir = normpath(dirname(self.main_src) + "/out")  # output directory

        # name of the generated final file
        self.main_out = self.out_dir + "/" + replace_extension(basename(main_src), "ihx")

        self.targets = {
            "main": [self._main_target, lambda: self._clean_target(True)],
            "jmain": [self._main_target],
            "clean": [self._clean_target],
            "flash": [self._main_target, self._flash_target, self._clean_target]
        }

    def get_available_targets(self):
        return self.targets.keys()

    def exec_target(self, target_str="main"):
        if target_str.lower() not in self.targets:
            print_err_and_quit("Target not defined")

        for item in self.targets[target_str.lower()]:
            item()
            if item != self.targets[target_str.lower()][-1]:
                print()

    def _main_target(self):
        print_info("Starting compilation...")
        print_verb("Extracting used headers")
        used_c = get_used_c_files(self.main_src)
        # now every used .c (even in the stm library) should be in used_c

        rel_list = []

        for cFile in used_c:
            print_verb("Compiling " + cFile)
            out_rel = normpath(self.out_dir + "/" + cFile[len(commonpath([self.out_dir, cFile])):])
            out_rel = replace_extension(out_rel, "rel")
            out_path = normpath(dirname(out_rel))

            makedirs(out_path, exist_ok=True)

            cmd_line = [CC]
            cmd_line.extend(CFLAGS)
            cmd_line.extend(["-c", cFile, "-o", out_rel])

            if subprocess.call(cmd_line) != 0:
                print_err_and_quit("Non-zero returned during compilation of " + cFile)

            rel_list.append(out_rel)

        # ok, every non main file is compiled, now compile main.c
        cmd_line = [CC]
        cmd_line.extend(CFLAGS)
        cmd_line.extend(["--out-fmt-ihx", "-o", self.main_out, self.main_src])
        cmd_line.extend(rel_list)

        # BUG correction: if we never enter in the for loop above, out_dir
        # doesn't exist. Let's make it
        makedirs(self.out_dir, exist_ok=True)

        if subprocess.call(cmd_line) != 0:
            print_err_and_quit("Non-zero returned during compilation of " + self.main_src)

        print_info("Done compiling")

    def _clean_target(self, leave_main=False):
        print_info("Cleaning...")

        if leave_main:
            print_verb("Preseving " + self.main_out)
            temp_file = self.main_out.split("/")
            temp_file.insert(len(temp_file) - 1, "..")
            temp_file = normpath("/".join(temp_file))
            rename(self.main_out, temp_file)

        print_verb("Removing " + self.out_dir)
        shutil.rmtree(self.out_dir, ignore_errors=True)

        if leave_main:
            mkdir(self.out_dir)
            rename(temp_file, self.main_out)

        print_info("Done cleaning")

    def _flash_target(self):
        print_info("Starting flashing...")
        cmd_line = [FLASH]
        cmd_line.extend(FLASHFLAGS)
        cmd_line.extend(["-w", self.main_out])

        if subprocess.call(cmd_line) != 0:
            print_err_and_quit("Non-zero returned during flash")
        print_info("Done flashing")


def main():
    # Colorama initialization
    c_init(autoreset=True)

    # if 0 parameters are passed, then use all default values
    if len(sys.argv) == 1:
        maker = Maker()
        maker.exec_target()

    # if 1 parameter is passed, we assume that it's a target
    elif len(sys.argv) == 2:
        maker = Maker()

        if sys.argv[1].lower() == "--help":
            print_usage_and_quit(maker.get_available_targets())
        else:
            maker.exec_target(sys.argv[1])

    # if 2 parameters are passed, we assume that the first is
    # the target, the second is the main_src
    elif len(sys.argv) == 3:
        maker = Maker(main_src=sys.argv[2])
        maker.exec_target(sys.argv[1])


if __name__ == "__main__":
    main()
