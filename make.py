#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re
import sys
import shutil
import subprocess
from colorama import init as c_init, Fore
from os.path import abspath, normpath, commonpath, basename, dirname, splitext

#<configuration>
CPU_F = "2000000UL"
CC = "/usr/local/bin/sdcc"
CFLAGS = ["-lstm8", "-mstm8", "--std-sdcc11", "-DF_CPU=" + CPU_F]
FLASH = "/usr/local/bin/stm8flash"
FLASHFLAGS = ["-c", "stlinkv2", "-p", "stm8s103?3"]
#</configuraion>

#please do NOT modify after this line, unless you know what you are doing

printInfo = lambda str: print(Fore.BLUE + str)
printVerb = lambda str: print(Fore.YELLOW + str)

def printErrAndQuit(str):
    print(Fore.RED + str + "\nQuitting")
    quit()


def printUsageAndQuit(targets):
    print("Syntax: ", end="")
    printInfo("./make.py [target] [main_src]")
    print("Available targets are: ", end="")
    printInfo(", ".join(targets))
    quit()


replaceExtension = lambda file, ext: splitext(file)[0] + "." + ext


def extractHeaders(filename):
    with open(filename, "r") as f:
        content = f.read()

    #remove any /* */ and // block
    content = re.sub(r"(/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*)", "", content)

    #get file path
    path = dirname(filename) + "/"

    #regex to parse #include "something.h"
    pattern = re.compile(r'#include\s*"\s*([^"]+\.[Hh])\s*"')
    s = set([normpath(path + item.lower()) for item in re.findall(pattern, content)])

    for item in s:
        s = s | extractHeaders(item)    #recursion

    return s


class Maker:
    def __init__(self, main_src="main.c"):
        self.main_src = abspath(main_src) #starting source file
        self.out_dir = normpath(dirname(self.main_src) + "/out") #output directory

        #name of the generated final file
        self.main_out = self.out_dir + "/" + replaceExtension(basename(main_src), "ihx")

        self.targets = {
            "main"  : [self._mainTarget, lambda: self._cleanTarget(True)],
            "clean" : [self._cleanTarget],
            "flash" : [self._mainTarget, self._flashTarget, self._cleanTarget]
        }


    def getAvailableTargets(self):
        return self.targets.keys()


    def execTarget(self, targetStr="main"):
        if targetStr.lower() not in self.targets:
            printErrAndQuit("Target not defined")

        for item in self.targets[targetStr.lower()]:
            item()
            if item != self.targets[targetStr.lower()][-1]:
                print()


    def _mainTarget(self):
        printInfo("Starting compilation...")
        printVerb("Extracting used headers")
        used_h = extractHeaders(self.main_src)   #headers used by the user

        #now every used .h (even in the stm library) should be in used_h:
        #we need to find if there's a corresponding .c

        rel_list = []

        for h in used_h:
            c_file = replaceExtension(h, "c")

            if os.path.exists(c_file):
                printVerb("Compiling " + c_file)
                out_rel = normpath(self.out_dir + "/" + c_file[len(commonpath([self.out_dir, c_file])):])
                out_rel = replaceExtension(out_rel, "rel")
                out_path = normpath(dirname(out_rel))

                os.makedirs(out_path, exist_ok=True)

                l=[CC]
                l.extend(CFLAGS)
                l.extend(["-c", c_file, "-o", out_rel])

                if subprocess.call(l) != 0:
                    printErrAndQuit("Non-zero returned during compilation of " + c_file)

                rel_list.append(out_rel)

        #ok, every non main file is compiled, now compile main.c
        l = [CC]
        l.extend(CFLAGS)
        l.extend(["--out-fmt-ihx", "-o", self.main_out, self.main_src])
        l.extend(rel_list)

        #BUG correction: if we never enter in the for loop above, out_dir
        #doesn't exist. Let's make it
        os.makedirs(self.out_dir, exist_ok=True)

        if subprocess.call(l) != 0:
            printErrAndQuit("Non-zero returned during compilation of " + self.main_src)

        printInfo("Done compiling")
    

    def _cleanTarget(self, leaveMain=False):
        printInfo("Cleaning...")

        if leaveMain:
            printVerb("Preseving " + self.main_out)
            tempFile = self.main_out.split("/")
            tempFile.insert(len(tempFile)-1, "..")
            tempFile = normpath("/".join(tempFile))
            os.rename(self.main_out, tempFile)

        printVerb("Removing " + self.out_dir)
        shutil.rmtree(self.out_dir, ignore_errors=True)

        if leaveMain:
            os.mkdir(self.out_dir)
            os.rename(tempFile, self.main_out)

        printInfo("Done cleaning")
        

    def _flashTarget(self):
        printInfo("Starting flashing...")
        l = [FLASH]
        l.extend(FLASHFLAGS)
        l.extend(["-w", self.main_out])

        if subprocess.call(l) != 0:
            printErrAndQuit("Non-zero returned during flash")
        printInfo("Done flashing")


def main():
    #Colorama initialization
    c_init(autoreset=True)

    #if 0 parameteres are passed, then use all deafult vaules
    if len(sys.argv) == 1:
        maker = Maker()
        maker.execTarget()

    #if 1 parameter is passed, we assume that it's a target
    elif len(sys.argv) == 2:
        maker = Maker()

        if sys.argv[1].lower()=="--help":
            printUsageAndQuit(maker.getAvailableTargets())
        else:
            maker.execTarget(sys.argv[1])
    
    #if 2 parameters are passed, we assume that the first is
    #the target, the second is the main_src
    elif len(sys.argv) == 3:
        maker = Maker(main_src=sys.argv[2])
        maker.execTarget(sys.argv[1])


if __name__ == "__main__":
    main()