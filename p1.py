#Fall 2019 CO term-project
#Zhaoxi Sun & Hongbao Zhao & Qiran Sun & Yang Yi

import sys



arguments = sys.argv[1:]
branches = {}
def getInstructions(arguments):#Hongbo Zhao
    global branches
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
            branch_name = line[0: lenOfInst-1]
            branches[branch_name] = line_num
    f.close()


def set_cycleStages(cycle_stages, i, j, current_stage):#Hongbo Zhao
    if current_stage == 1:
        k = 0
        while k < j:
            cycle_stages[j].append(".")
            k += 1
        cycle_stages[j].append("IF");
        k = j + 1
        while k < 16:
            k += 1
            cycle_stages[j].append(".")
    elif cycle_stages[j][i] == "*":
        pass
    elif current_stage == 2:
        cycle_stages[j][i] = "ID"
    elif current_stage == 3:
        if cycle_stages[j][i-1] == "*":
            cycle_stages[j][i] = "*"
        else:
            cycle_stages[j][i] = "EX"
    elif current_stage == 4:
        if cycle_stages[j][i-1] == "*":
            cycle_stages[j][i] = "*"
        else:
            cycle_stages[j][i] = "MEM"
    elif current_stage == 5:
        if cycle_stages[j][i-1] == "*":
            cycle_stages[j][i] = "*"
        else:
            cycle_stages[j][i] = "WB"
            
def getOperation(instruction, operation):#Hongbo Zhao
    lenInst = len(instruction)
    if instruction[len-1] == ':':
        return "branch"
    if instruction == "nop":
        return "nop"
    index = 0
    while instruction[index] != ' ':
        index += 1
    operation = instruction[0:index]
    if operation == "bne" or operation == "beq":
        return "J"
    else:
        return "I"

    
def read_instruction(instruction, tt)：#Zhaoxi Sun
    #gloabl variables to allow us to change the data in the function
    global destination
    global operand1
    global operand2
    #N type
    if tt=="N":
        operand_count = 0
        for i in range(len(instruction)):
            if instruction[i] == "$":
                if operand_count == 0: #count is 0, just destination
                    operand_count += 1
                    destination = instruction[i+1:i+3]
                elif operand_count == 1:
                    if instruction[i+1] == "z":
                        operand1 = instruction[i+1:i+5]
                    else:
                        operand1 = instruction[i+1,i+3]
                    operand_count += 1
            elif instruction[i] == "," and operand_count == 2:
                if instruction[i+1] == "$":
                    operand2 = instruction[i+1:i+3]
                else:
                    operand2 = instruction[i+1,len(instruction)]
    #J type
    elif tt== "J":
        operand_count = 0
        for i in range(len(instruction)):
            if instruction[i] == "$":
                if operand_count == 0:
                    operand_count+=1;
                    operand1 = instruction[i+1:i+3]
                elif operand_count == 1:
                    operand2 = instruction[i+1:i+3]
             elif instruction[i-1] == "," and operand_count == 1:
                destination = instruction[i:len(instruction)]
destinations = {}                
def set_destinationStage(destination, current_stage):#Hongbo Zhao
    global destinations
    destinations[destination] = current_stage

def initialize_register_file():#Mike Yang
    ret = {}
    ret['s0'] = [0,0]
    ret['s1'] = [0,0]
    ret['s2'] = [0,0]
    ret['s3'] = [0,0]
    ret['s4'] = [0,0]
    ret['s5'] = [0,0]
    ret['s6'] = [0,0]
    ret['s7'] = [0,0]
    ret['t0'] = [0,0]
    ret['t1'] = [0,0]
    ret['t2'] = [0,0]
    ret['t3'] = [0,0]
    ret['t4'] = [0,0]
    ret['t5'] = [0,0]
    ret['t6'] = [0,0]
    ret['t7'] = [0,0]
    ret['t8'] = [0,0]
    ret['t9'] = [0,0]
    return ret
    
    
    
    
def update_registerFile(operation, destination, operand1, operand2, register_file):#Mike Yang
    global register_file
    v0 = 0
    if operation1 == "zero":
        v1 = 0
    else:
        v1 = register_file[operand1][1] #    assuming maps are implemented as dictionary of lists
    if operation == "add" or operation == "and" or operation == "or" or operation == "slt":
        v2 = register_file[operand2][1]
        if operation == "add":
            v0 = v1 + v2
        elif operation == "and":
            v0 = v1 & v2
        elif operation == "or":
            v0 = v1^v2
        elif operation == "slt":
            if v1 < v2:
                v0 = 1
            else:
                v0 = 0
    else:
        v2 = int(operand2)
        if operation== 'addi':
            v0 = v1 + v2
        elif operation == 'andi':
            v0 = v1 & v2
        elif operation == 'ori':
            v0 = v1 ^v2
            
        elif operation == 'slti':
            if v1 < v2 :
                v0 = 1
            else:
                v0 = 0
    assert <v0 != -1>
    register_file[destination][1] = v0
    
def put_b_in_a(destination, register_file):#Mike Yang
    global register_file
    register_file[destination][1] = register_file[destination][1]# assumed dictionary of list for register file

def get_value(operand, register_file, operand_value):#Mike Yang
    global operand_value
    operand_value = register_file[operand][1] #assumed as above
    
def get_branchIndex(destination, branches, next_destination_index): #Mike Yang
    global next_instruction_index
    next_instruction_index = branches[destination]

def print_cycle(cycle_instructions, cycle_stages, register_file):#Mike Yang
    print("CPU Cycles ===>\t\t1\t2\t3\t4\t5\t6\t7\t8\t9\t10\t11\t12\t13\t14\t15\t16")
    count = len(cycle_instruction)
    
    for i in range(count):
        print(cycle_instruction[i],'\t')
        if len(cycle_instrucitons[i]) < 16:
            print("\t")
        for j in range(16):
            if i != 15:
                print(cycle_stages[i][j],'\t')
            else
                print(cycle_stages[i][j],'\n')
    print("\n")
    
    print("$s0 =",register_file['s0'][0],'\t\t') # assumed as above
    print("$s1 =",register_file['s1'][0],'\t\t')
    print("$s2 =",register_file['s2'][0],'\t\t')
    print("$s3 =",register_file['s3'][0],'\n')
    print("$s4 =",register_file['s4'][0],'\t\t')
    print("$s5 =",register_file['s5'][0],'\t\t')
    print("$s6 =",register_file['s6'][0],'\t\t')
    print("$s7 =",register_file['s7'][0],'\n')
    print("$t0 =",register_file['t0'][0],'\t\t')
    print("$t1 =",register_file['t1'][0],'\t\t')
    print("$t2 =",register_file['t2'][0],'\t\t')
    print("$t3 =",register_file['t3'][0],'\n')
    print("$t4 =",register_file['t4'][0],'\t\t')
    print("$t5 =",register_file['t5'][0],'\t\t')
    print("$t6 =",register_file['t6'][0],'\t\t')
    print("$t7 =",register_file['t7'][0],'\n')
    print("$t8 =",register_file['t8'][0],'\t\t')
    print("$t9 =",register_file['t9'][0],'\n')
    print("----------------------------------------------------------------------------------\n")
    def add_star (cycle_stages,instruction_index,cycle):#Qiran Sun
    global cycle_stages
    cycle_stages[instruction_index][cycle] = "*"

def get_nopNumber(operand1, operand2, destinations)# Qiran Sun
    global destinations
    it1= destinations.get(operand1);
    it2=destinations.get(operand2);
    if it1!= None and it2!= None:
        tmp = min(destinations[operand1], destinations[operand2])
        if tmp>6:
            return 0
        else
            return 6-temp

    else if it1!= None:
        tmp = destinations[operand1]
        if(tmp > 6)
            return 0;
        
        else 
            return 6 - tmp

    else if(it2 != None)
        tmp = destinations[operand2]
        if(tmp > 6)
            return 0
        
        else 
            return 6 -tmp
        
    
    else
        return 0


def insert_nop(nop_num, cycle_stages, cycle_instructions, j,  i) # Qiran Sun
    global cycle_instructions
    global cycle_stages
    global nop_num
    for i in range (nop_num):
        cycle_instructions.insert(i,"nop")
        i++

    temp_stage = cycle_stages[j]
    temp_stage[i] = "*"
    for i in range (nop_num):
        cycle_stages.insert(i,temp_stage)
        i++

    cycle_stages[  j + nop_num  ][i] = "ID"
    if (cycle_stages[j + nop_num+1].size !=0)
        cycle_stages[j + nop_num+1][i] = "IF";

    return;
    
         
