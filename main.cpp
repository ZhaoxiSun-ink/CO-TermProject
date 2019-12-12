#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <cstring>
#include <algorithm>
#include <iomanip>
using namespace std;


void cyclestage(vector<vector<string>>& cycleStages, int i, int j, int stageCurr) {
	if (stageCurr == 1) {
		int k = 0;
		while (k < j) {
			cycleStages[j].push_back(".");
			k++;
		}
		cycleStages[j].push_back("IF");
		k = j + 1;
		while (k < 16) {
			cycleStages[j].push_back(".");
			k++;
		}
	}
	else if (cycleStages[j][i] == "*") {
		return;
	}
	else {

		switch (stageCurr) {
		case 2:
			cycleStages[j][i] = "ID";
		case 3:
			if (cycleStages[j][i-1] != "*") {
				cycleStages[j][i] = "EX";
			}
			else {
				cycleStages[j][i] = "*";
			}
		case 4:
			if (cycleStages[j][i-1] != "*") {
				cycleStages[j][i] = "MEM";
			}
			else {
				cycleStages[j][i] = "*";
			}
		case 5:
			if (cycleStages[j][i-1] != "*") {
				cycleStages[j][i] = "WB";
			}
			else {
				cycleStages[j][i] = "*";
			}
		}
	}
}



//Mike Yang 
std::string getOperation(string instruction, string& operation) {
	int len = instruction.length();
	if (instruction[len-1] == ':') {
		return "branch";
	}

	if (instruction.compare("nop") == 0) {
		return "nop";
	}

	int i = 0;
	while (true) {
		if (instruction[i] == ' ') {
			break;
		}
		i++;
	}
	operation = instruction.substr(0, i);
	if (operation.compare("bne") == 0 or operation.compare("beq") == 0) {
		return "J";
	}
	else {
		return "I";
	}
}


void readInstruction(const string instruction, string & target, string & reg1, string & reg2, char type) {
	if (type == 'N') {
		int regCount = 0;
		for (int i = 0; i < (int)instruction.length(); i++) {
			if (instruction[i] == '$') {
				if (regCount == 0) {
					regCount++;
					target = instruction.substr(i+1, 2);
				}
				else if (regCount == 1) {
					if (instruction.substr(i+1,4).compare("zero") == 0) {
						reg1 = instruction.substr(i+1, 4);
					}
					else {
						reg1 = instruction.substr(i+1, 2);
					}
					regCount++;
				}
			}
			else if (instruction[i] == ',' && regCount == 2) {
				if (instruction[ +1] == '$') {
					reg2 = instruction.substr(i+2, 2);
				}
				else {
					reg2 = instruction.substr(i+1, instruction.length() - i - 1);
				}
			}
		}
	}
	else if (type == 'J') {
		int regCount = 0;
		for (int i = 0; i < (int)instruction.length(); i++) {
			if (instruction[i] == '$') {
				if (regCount == 0) {
					regCount++;
					reg1 = instruction.substr(i+1, 2);
				}
				else if (regCount == 1) {
					reg2 = instruction.substr(i+1, 2);
				}
			}
			if (regCount == 1 and instruction[i-1] == ',') {
				int len = instruction.length() - i;
				target = instruction.substr(i, len);
			}
		}
	}
}


void updateReg(std::string operater, std::string target, std::string reg1, std::string reg2,
	std::map<std::string, std::pair<int, int> > & registers) {
	int v0, v1, v2;
	v0 = 0;
	if (reg1.compare("zero") == 0) {
		v1 = 0;
	}
	else {
		v1 = registers[reg1].second;
	}
	std::vector<string> operationStr = {"add", "and", "or", "slt"};
	if (std::find(operationStr.begin(), operationStr.end(), operater) != operationStr.end()) {
		v2 = registers[reg2].second;
		if (operater == "add") {
			v0 = v1 + v2;
		}
		else if (operater == "and") {
			v0 = v1 & v2;
		}
		else if (operater == "or") {
			v0 = v1 ^ v2;
		}
		else if (operater == "slt") {
			if (v1 < v2) {
				v0 = 1;
			}
			else {
				v0 = 0;
			}
		}
	}
	else {
		v2 = stoi(reg2);
		if (operater == "addi") {
			v0 = v1 + v2;
		}
		else if (operater == "andi") {
			v0 = v1 & v2;
		}
		else if (operater == "ori") {
			v0 = v1 ^ v2;
		}
		else if (operater == "slti") {
			if (v1 < v2) {
				v0 = 1;
			}
			else {
				v0 = 0;
			}
		}
	}
	registers[target].second = v0;
}


void printOut(std::vector<std::string> const& cycleInstru, std::vector<std::vector<std::string> > const& cycleStages,
	std::map<std::string, std::pair<int, int> > registers)
{
	std::cout << "CPU Cycles ===>     1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16" << std::endl;
	int instruCount = cycleInstru.size();
	int i = 0;
	while (i < instruCount) {
		std::cout << left << setw(20) << cycleInstru[i];
		int j = 0;
		while (j < 16) {
			if (j != 15) {
				std::cout << left << setw(4) << cycleStages[i][j];
			}
			else {
				std::cout << cycleStages[i][j] << "\n";
			}
			j++;
		}
		i++;
	}
	std::cout << "\n";

	cout << "$s0 = " << left << setw(14) << registers["s0"].first;
	cout << "$s1 = " << left << setw(14) << registers["s1"].first;
	cout << "$s2 = " << left << setw(14) << registers["s2"].first;
	cout << "$s3 = " << registers["s3"].first << "\n";
	cout << "$s4 = " << left << setw(14) << registers["s4"].first;
	cout << "$s5 = " << left << setw(14) << registers["s5"].first;
	cout << "$s6 = " << left << setw(14) << registers["s6"].first;
	cout << "$s7 = " << registers["s7"].first << "\n";
	cout << "$t0 = " << left << setw(14) << registers["t0"].first;
	cout << "$t1 = " << left << setw(14) << registers["t1"].first;
	cout << "$t2 = " << left << setw(14) << registers["t2"].first;
	cout << "$t3 = " << registers["t3"].first << "\n";
	cout << "$t4 = " << left << setw(14) << registers["t4"].first;
	cout << "$t5 = " << left << setw(14) << registers["t5"].first;
	cout << "$t6 = " << left << setw(14) << registers["t6"].first;
	cout << "$t7 = " << registers["t7"].first << "\n";
	cout << "$t8 = " << left << setw(14) << registers["t8"].first;
	cout << "$t9 = " << registers["t9"].first << endl;
	cout << "----------------------------------------------------------------------------------\n";
}

//edited
int get_nopNumber(std::string operand1, std::string operand2, map<string, int> destinations) {
	int tmp;
	map<string, int>::iterator it1, it2;
	it1 = destinations.find(operand1);
	it2 = destinations.find(operand2);
	if (it1 != destinations.end() && it2 != destinations.end()) {
		tmp = min(destinations[operand1], destinations[operand2]);
		if (tmp > 6) 	return 0;
		else return 6 - tmp;
	}
	else if (it1 != destinations.end()) {
		tmp = destinations[operand1];
		if (tmp > 6) 	return 0;
		else	return 6 - tmp;
	}
	else if (it2 != destinations.end()) {
		tmp = destinations[operand2];
		if (tmp > 6)	return 0;
		else 	return 6 - tmp;
	}
	else 	return 0;
}



//save return in this function
//stop the whole function
void set_cycleStages_no_forwarding(vector<vector<string> > & cycle_stages, int i, int j, int current_stage, vector<int> & nop_num) {
	if (nop_num[j] > 0) {
		cycle_stages[j][i] = cycle_stages[j][i - 1];
		nop_num[j]--;
		return;
	}
	else if (current_stage == 1) {
		for (int k = 0; k < j; k++) {
			cycle_stages[j].push_back(".");
		}
		cycle_stages[j].push_back("IF");
		for (int k = j + 1; k < 16; k++) {
			cycle_stages[j].push_back(".");
		}
	}
	else if ((cycle_stages[j][i - 1] == "*" && cycle_stages[j][i - 3] == "IF") || (cycle_stages[j][i - 1] == "*" && cycle_stages[j][i - 3] == "ID")) {
		cycle_stages[j][i] = "*";
		return;
	}

	else if (current_stage == 2) {
		cycle_stages[j][i] = "ID";
	}
	else if (current_stage == 3) {
		cycle_stages[j][i] = "EX";
	}
	else if (current_stage == 4) {
		cycle_stages[j][i] = "MEM";
	}
	else if (current_stage == 5 && cycle_stages[j][i - 1] != "*") {
		cycle_stages[j][i] = "WB";
	}
	return;
}


int main(int argc, const char* argv[]) {

	//initalize
	std::vector<std::string> instructions;
	std::map<std::string, int> branches;
	std::map<std::string, int> destinations;
	std::map<std::string, std::pair<int, int> > register_file;
	register_file["s0"] = make_pair(0, 0);
	register_file["s1"] = make_pair(0, 0);
	register_file["s2"] = make_pair(0, 0);
	register_file["s3"] = make_pair(0, 0);
	register_file["s4"] = make_pair(0, 0);
	register_file["s5"] = make_pair(0, 0);
	register_file["s6"] = make_pair(0, 0);
	register_file["s7"] = make_pair(0, 0);
	register_file["t0"] = make_pair(0, 0);
	register_file["t1"] = make_pair(0, 0);
	register_file["t2"] = make_pair(0, 0);
	register_file["t3"] = make_pair(0, 0);
	register_file["t4"] = make_pair(0, 0);
	register_file["t5"] = make_pair(0, 0);
	register_file["t6"] = make_pair(0, 0);
	register_file["t7"] = make_pair(0, 0);
	register_file["t8"] = make_pair(0, 0);
	register_file["t9"] = make_pair(0, 0);


	std::vector<std::string> cycle_instructions;
	std::vector<std::vector<std::string> > cycle_stages;
	vector<string> temp;
	//Hongbo Zhao--whileloop
	int i = 0;
	while (i < 16) {
		cycle_stages.push_back(temp);
		i++;
	}
	//read instructions into vector
	//getInstructions(argv[2], instructions, branches);
	ifstream myfile;
	myfile.open(argv[2]);

	string instruction;
	int line_num = 0;
	while (getline(myfile, instruction)) {
		line_num++;
		instructions.push_back(instruction);
		// Check if this is a branch label by checking the last char
		int len = instruction.length();
		if (instruction[len - 1] == ':') {
			// Get the branch name
			string branch_name = instruction.substr(0, len - 1);
			// The branch starts from the next line
			branches[branch_name] = line_num;
		}
	}
	//Hongbo Zhao
	if (string(argv[1]).compare("F") == 0) {
		cout << "START OF SIMULATION (forwarding)" << endl << "----------------------------------------------------------------------------------" << endl;
		int next_instruction_index = 0;
		int i = 0;
		while (i < 16) {
			int j = 0;
			while (j <= (int)cycle_instructions.size()) {
				std::string current_instruction;
				std::string operation;
				std::string instruction_type;
				if (j == i) {
					if ((int)cycle_instructions.size() != j + 1) {
						if (next_instruction_index >= (int)instructions.size()) {
							break;
						}
						instruction_type = getOperation(instructions[next_instruction_index], operation);
						if (instruction_type.compare("branch") == 0) {
							next_instruction_index++;
						}
						cycle_instructions.push_back(instructions[next_instruction_index]);
						next_instruction_index++;
					}
				}
				if (j == (int)cycle_instructions.size()) {
					break;
				}
				current_instruction = cycle_instructions[j];
				int current_stage;
				current_stage = i - j + 1;
				cyclestage(cycle_stages, i, j, current_stage);

				instruction_type = getOperation(current_instruction, operation);
				std::string destination;
				std::string operand1;
				std::string operand2;

				if (instruction_type.compare("J") != 0) {
					readInstruction(current_instruction, destination, operand1, operand2, 'N');
					if (current_stage == 3) {
						updateReg(operation, destination, operand1, operand2, register_file);
					}

				}
				if (instruction_type.compare("J") == 0) {
					readInstruction(current_instruction, destination, operand1, operand2, 'J');
					if (current_stage == 5) {

						int operand1_value;
						int operand2_value;
						//Qiran Sun
						operand1_value = register_file[operand1].second;
						operand2_value = register_file[operand2].second;
						if ((operation.compare("beq") == 0 && operand1_value == operand2_value) || (operation.compare("bne") == 0 && operand1_value != operand2_value)) {
							int k = 1;
							while (k < 4) {
								cycle_stages[j + k][i] = "*";
								k++;
							}
							next_instruction_index = branches[destination];
							cycle_instructions.push_back(instructions[next_instruction_index]);
							next_instruction_index++;
						}

					}
				}

				if (current_stage == 5 && find(cycle_stages[j].begin(), cycle_stages[j].end(), "*") == cycle_stages[j].end()) {
					register_file[destination].first = register_file[destination].second;
				}
				if (j == (int)cycle_instructions.size() - 1 && current_stage == 5) {
					printOut(cycle_instructions, cycle_stages, register_file);
					cout << "END OF SIMULATION" << endl;
					return 0;
				}
				j++;
			}
			printOut(cycle_instructions, cycle_stages, register_file);
			i++;

		}
	}
	//Hongbo Zhao
	if (string(argv[1]).compare("N") == 0) {
		cout << "START OF SIMULATION (no forwarding)" << endl << "----------------------------------------------------------------------------------" << endl;
		int next_instruction_index = 0;
		vector<int> nops_number;
		int i = 0;
		while (i < 16) {
			nops_number.push_back(0);
			i++;
		}
		i = 0;
		while (i < 16) {
			int j = 0;
			while (j <= (int)cycle_instructions.size()) {
				std::string current_instruction;
				std::string operation;
				std::string instruction_type;
				if (j == i) {
					if ((int)cycle_instructions.size() != j + 1) {
						if (next_instruction_index >= (int)instructions.size()) {
							break;
						}
						instruction_type = getOperation(instructions[next_instruction_index], operation);
						if (instruction_type.compare("branch") == 0) {
							next_instruction_index++;
						}
						cycle_instructions.push_back(instructions[next_instruction_index]);
						next_instruction_index++;
					}
				}
				if (j == (int)cycle_instructions.size()) {
					break;
				}

				current_instruction = cycle_instructions[j];
				int current_stage;
				current_stage = i - j + 1;

				instruction_type = getOperation(current_instruction, operation);
				std::string destination;
				std::string operand1;
				std::string operand2;
				if (instruction_type.compare("nop") == 0) {
					set_cycleStages_no_forwarding(cycle_stages, i, j, current_stage, nops_number);
				}
				else if (instruction_type.compare("J") != 0) {
					readInstruction(current_instruction, destination, operand1, operand2, 'N');
					destinations[destination] = current_stage;
					if (current_stage == 3) {
						int nop = get_nopNumber(operand1, operand2, destinations);
						if (nop) {
							if (nop == 2) {
								cycle_instructions.insert(cycle_instructions.begin() + j, nop, "nop");
								vector<string> temp_stage = cycle_stages[j];
								temp_stage[i] = "*";
								cycle_stages.insert(cycle_stages.begin() + j, nop, temp_stage);
								cycle_stages[j + nop][i] = "ID";

								if (cycle_stages[j + nop + 1].size() != 0) {
									cycle_stages[j + nop + 1][i] = "IF";
								}

								j = j + 2;
								nops_number[j] = 1;
								nops_number[j + 1] = 1;
								break;

							}
							if (nop == 1) {
								cycle_instructions.insert(cycle_instructions.begin() + j, nop, "nop");
								vector<string> temp_stage = cycle_stages[j];
								temp_stage[i] = "*";
								cycle_stages.insert(cycle_stages.begin() + j, nop, temp_stage);
								cycle_stages[j + nop][i] = "ID";

								if (cycle_stages[j + nop + 1].size() != 0) {
									cycle_stages[j + nop + 1][i] = "IF";
								}
								j = j + 1;
								break;
							}

						}
						set_cycleStages_no_forwarding(cycle_stages, i, j, current_stage, nops_number);
					}
					else if (current_stage == 5) {
						updateReg(operation, destination, operand1, operand2, register_file);
						register_file[destination].first = register_file[destination].second;
						set_cycleStages_no_forwarding(cycle_stages, i, j, current_stage, nops_number);
					}
					else {
						set_cycleStages_no_forwarding(cycle_stages, i, j, current_stage, nops_number);
					}

				}
				else if (instruction_type.compare("J") == 0) {
					readInstruction(current_instruction, destination, operand1, operand2, 'J');
					destinations[destination] = current_stage;

					if (current_stage == 3) {
						int nop = get_nopNumber(operand1, operand2, destinations);
						if (nop) {
							if (nop == 2) {
								cycle_instructions.insert(cycle_instructions.begin() + j, nop, "nop");
								vector<string> temp_stage = cycle_stages[j];
								temp_stage[i] = "*";
								cycle_stages.insert(cycle_stages.begin() + j, nop, temp_stage);
								cycle_stages[j + nop][i] = "ID";

								if (cycle_stages[j + nop + 1].size() != 0) {
									cycle_stages[j + nop + 1][i] = "IF";
								}
								j = j + 2;
								nops_number[j] = 1;
								nops_number[j + 1] = 1;

							}
							if (nop == 1) {
								cycle_instructions.insert(cycle_instructions.begin() + j, nop, "nop");
								vector<string> temp_stage = cycle_stages[j];
								temp_stage[i] = "*";
								cycle_stages.insert(cycle_stages.begin() + j, nop, temp_stage);
								cycle_stages[j + nop][i] = "ID";

								if (cycle_stages[j + nop + 1].size() != 0) {
									cycle_stages[j + nop + 1][i] = "IF";
								}
								j = j + 1;
							}

						}
						else {
							set_cycleStages_no_forwarding(cycle_stages, i, j, current_stage, nops_number);
						}
					}
					else if (current_stage == 5) {
						set_cycleStages_no_forwarding(cycle_stages, i, j, current_stage, nops_number);
						int operand1_value;
						int operand2_value;
						operand1_value = register_file[operand1].second;
						operand2_value = register_file[operand2].second;

						if ((operation.compare("beq") == 0 && operand1_value == operand2_value) || (operation.compare("bne") == 0 && operand1_value != operand2_value)) {
							next_instruction_index = branches[destination];
							cycle_instructions.push_back(instructions[next_instruction_index]);
							next_instruction_index++;
							cyclestage(cycle_stages, i, j + 4, 1);
						}
					}
					else if (i != j) {
						set_cycleStages_no_forwarding(cycle_stages, i, j, current_stage, nops_number);
					}
				}

				if (j == (int)cycle_instructions.size() - 1 && current_stage == 5) {
					printOut(cycle_instructions, cycle_stages, register_file);
					cout << "END OF SIMULATION" << endl;
					return 0;
				}
				j++;
			}
			printOut(cycle_instructions, cycle_stages, register_file);
			i++;
		}
	}
	cout << "END OF SIMULATION" << endl;
	return 0;
}
