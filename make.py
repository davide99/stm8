#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re
import sys
import glob
import shutil
import subprocess
from colorama import init as c_init, Fore, Back, Style

#<configuration>
main_src = "main.c"
out_dir = "./out"
cpu_f = "2000000UL"

CC = "/usr/local/bin/sdcc"
CFLAGS = ["-lstm8", "-mstm8", "--std-sdcc11", "-DF_CPU=" + cpu_f]
FLASH = "/usr/local/bin/stm8flash"
FLASHFLAGS = ["-c", "stlinkv2", "-p", "stm8s103?3"]
#</configuraion>

#please do NOT modify after this line, unless you know what you are doing

#a useful var
main_out = out_dir + "/" + main_src[:-1] + "ihx"

printInfo = lambda str: print(Fore.BLUE + str)
printVerb = lambda str: print(Fore.YELLOW + str)

def printErrAndQuit(str):
    print(Fore.RED + str + "\nQuitting")
    quit()


def printUsageAndQuit(targets):
    print("Available targets are: ", end="")
    printInfo(", ".join(targets))
    quit()


def extractPath(file):
    path = "/".join(file.split("/")[:-1]) + "/"
    if path == "/":
        path = ""

    return path


def extractHeaders(filename):
    #read whole file
    f = open(filename, "r")
    content = f.read()
    f.close()

    #remove any /* */ and // block
    content = re.sub(r"(/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*)", "", content)

    #get file path
    path = extractPath(filename)

    #regex to parse #include "something.h"
    pattern = re.compile(r'#include\s*"\s*([^"]+\.[Hh])\s*"')
    s = set([path + item.lower() for item in re.findall(pattern, content)])

    for item in s:
        s = s | extractHeaders(item)    #recursion

    return s


def mainTarget():
    printInfo("Starting compilation...")
    printVerb("Extracting used headers")
    used_h = extractHeaders(main_src)   #headers used by the user

    #now every used .h (even in the stm library) should be in used_h:
    #we need to find if there's a corresponding .c

    rel_list = []

    for h in used_h:
        c_file = h[:-1] + "c"

        if os.path.exists(c_file):
            printVerb("Compiling " + c_file)
            out_path = out_dir + "/" + extractPath(c_file)
            out_rel = out_dir + "/" + h[:-1] + "rel"

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
    l.extend(["--out-fmt-ihx", "-o", main_out, main_src])
    l.extend(rel_list)

    if subprocess.call(l) != 0:
        printErrAndQuit("Non-zero returned during compilation of " + main_src)

    printInfo("Done compiling\n")
    

def cleanTarget(leaveMain=False):
    printInfo("Cleaning...")
    printVerb("Preseving " + main_out)

    if leaveMain:
        os.rename(main_out, "./123" + main_src[:-1] + "ihx")

    printVerb("Removing " + out_dir)
    shutil.rmtree(out_dir, ignore_errors=True)

    if leaveMain:
        os.mkdir(out_dir)
        os.rename("./123" + main_src[:-1] + "ihx", main_out)

    printInfo("Done cleaning\n")
    

def flashTarget():
    printInfo("Starting flashing...")
    l = [FLASH]
    l.extend(FLASHFLAGS)
    l.extend(["-w", main_out])

    if subprocess.call(l) != 0:
        printErrAndQuit("Non-zero returned during flash")
    printInfo("Done flashing\n")


def main():
    #Colorama initialization
    c_init(autoreset=True)

    targets = {
        "main"  : [mainTarget, lambda: cleanTarget(True)],
        "clean" : [cleanTarget],
        "flash" : [mainTarget, flashTarget, cleanTarget]
    }

    if len(sys.argv) == 1:  #go with the default target
        sys.argv.append("main")

    if (len(sys.argv) != 2) or (sys.argv[1].lower() not in targets):
        printUsageAndQuit(targets.keys())

    for item in targets[sys.argv[1].lower()]:
        item()


if __name__ == "__main__":
    main()