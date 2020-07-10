#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import re
import sys
import glob
import shutil
import subprocess

#As you may have noticed, i'm not a huge makefile fan :)

#v configuration v
main_src = "main.c"
out_dir = "./out"

CC = "/usr/local/bin/sdcc"
CFLAGS = "-lstm8 -mstm8 --std-sdcc11".split(" ")
FLASH = "/usr/local/bin/stm8flash"
FLASHFLAGS = "-c stlinkv2 -p stm8s103?3".split(" ")

#please do NOT modify after this line, unless you know what you are doing
def printUsage(targets):
    print("Available targets are: ", end="")
    print(", ".join(targets))

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
        s = s | extractHeaders(item)

    return s


def mainTarget():
    used_h = extractHeaders(main_src)   #headers used by the user

    #noice, at this point every used .h (even in the stm library)
    #should be in used_h: we need to find if there's a corresponding
    #.c

    rel_list = []

    for h in used_h:
        c_file = h[:-1] + "c"

        if os.path.exists(c_file):
            out_path = out_dir + "/" + extractPath(c_file)
            out_rel = out_dir + "/" + h[:-1] + "rel"

            os.makedirs(out_path, exist_ok=True)

            l=[CC]
            l.extend(CFLAGS)
            l.extend(["-c", c_file, "-o", out_rel])
            ret = subprocess.call(l)

            if ret != 0:
                print("Non-zero returned during compilation of " + c_file + "\nQuitting")
                quit()

            rel_list.append(out_rel)

    #ok, every non main file is compiled, now compile main.c
    main_out = out_dir + "/" + main_src[:-1] + "ihx"
    l = [CC]
    l.extend(CFLAGS)
    l.extend(["--out-fmt-ihx", "-o", main_out, main_src])
    l.extend(rel_list)

    ret = subprocess.call(l)
    if ret != 0:
        print("Non-zero returned during compilation of " + main_src + "\nQuitting")
        quit()

    return
    

def clean(leaveMain=False):
    print("Cleaning...")
    main_out = out_dir + "/" + main_src[:-1] + "ihx"

    if leaveMain:
        os.rename(main_out, "./" + main_src[:-1] + "ihx")

    shutil.rmtree(out_dir, ignore_errors=True)

    if leaveMain:
        os.mkdir(out_dir)
        os.rename("./" + main_src[:-1] + "ihx", main_out)

    return
    

def flashTarget():
    l = [FLASH]
    l.extend(FLASHFLAGS)
    l.extend(["-w", out_dir + "/" + main_src[:-1] + "ihx"])

    ret = subprocess.call(l)
    if ret != 0:
        print("Non-zero returned during flashing\nQuitting")
        quit()

    return


def main():
    targets = {
        "main" : [
            mainTarget, lambda: clean(True)
        ],
        "clean" : [
            clean
        ],
        "flash" : [
            mainTarget, flashTarget, clean
        ]
    }

    if (len(sys.argv) != 2) or (sys.argv[1].lower() not in targets):
        printUsage(targets.keys())
        quit()

    for item in targets[sys.argv[1].lower()]:
        item()

    return


if __name__ == "__main__":
    main()