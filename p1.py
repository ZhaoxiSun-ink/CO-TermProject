#Fall 2019 CO term-project
#Zhaoxi Sun & Hongbao Zhao & Qiran Sun & Yang Yi

import sys


program_name = sys.argv[0]
arguments = sys.argv[1:]

def getInstructions(arguments):#Hongbo Zhao
    branches = {}
    mode = arguments[0]
    file = arguments[1]
    instruction = ""
    line_num = 0
    f= open(file)
    for line in f.readlines():
        line = line.strip()
        instruction += line
        line_num += 1
        lenOfInst = len(line)
        if line[lenOfInst-1] == ":":
            branch_name = line[0,lenOfInst-1]
            branches[branch_name] = line_num
    f.close()

