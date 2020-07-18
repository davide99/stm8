import re
from os.path import dirname, normpath, exists, splitext, abspath

replaceExtension = lambda file, ext: splitext(file)[0] + "." + ext

def getUsedFilesFromFile(filename):
    if not exists(filename):
        return set()

    with open(filename, "r") as f:
        content = f.read()

    #remove any /* */ and // block
    content = re.sub(r"(/\*([^*]|[\r\n]|(\*+([^*/]|[\r\n])))*\*+/)|(//.*)", "", content)

    #get file path
    path = dirname(filename) + "/"

    #regex to parse #include "something.h"
    pattern = re.compile(r'#include\s*"\s*([^"]+\.[Hh])\s*"')
    #h = set of used headers
    h = set([normpath(path + item.lower()) for item in re.findall(pattern, content)])

    c = set()

    for f in h:
        c_file = replaceExtension(f, "c")

        if exists(c_file):  #there exists an associated .c file?
            c.add(c_file)
            
    return h | c


def getUsedCFiles(startingFile):
    startingFile = abspath(startingFile)
    currentFile = startingFile
    completed = set()       #analyzed files
    toBeAnalyzed = set()    #files to be analyzed
    
    while True:
        completed.add(currentFile)  #analyze the file
        if currentFile in toBeAnalyzed:
            toBeAnalyzed.remove(currentFile)

        #get used files
        s = getUsedFilesFromFile(currentFile)
    
        #eventually add new items to toBeAnalyzed
        for f in s:
            if f not in completed:
                toBeAnalyzed.add(f)
        
        if len(toBeAnalyzed) == 0: #no more file?
            break
        else:
            currentFile = toBeAnalyzed.pop()

    assert len(toBeAnalyzed) == 0

    #now we can strip all .h files
    return [item for item in completed if item.endswith(".c") and item != startingFile]


def main():
    for f in getUsedCFiles("./examples/mfrc522/main.c"):
        print(f)
    
    #print("h")
    #for h in d["h"]:
    #    print(h, sep=",")
    
    #print("c")
    #for h in d["c"]:
    #    print(h, sep=",")



if __name__ == "__main__":
    main()