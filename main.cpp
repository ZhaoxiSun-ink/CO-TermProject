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
	else if (cycleStages[j][i].compare("*") == 0) {
		return;
	}
	else {

		switch (stageCurr) {
		case 2:
			cycleStages[j][i] = "ID";
		case 3:
			if (cycleStages[j][i - 1] != "*") {
				cycleStages[j][i] = "EX";
			}
			else {
				cycleStages[j][i] = "*";
			}
		case 4:
			if (cycleStages[j][i - 1] != "*") {
				cycleStages[j][i] = "MEM";
			}
			else {
				cycleStages[j][i] = "*";
			}
		case 5:
			if (cycleStages[j][i - 1] != "*") {
				cycleStages[j][i] = "WB";
			}
			else {
				cycleStages[j][i] = "*";
			}
		}
	}
}

int calculatev0(int v1, int v2, std::string operater) {
	if (operater.compare("add") == 0 or operater.compare("addi") == 0) {
		return v1 + v2;
	}
	else if (operater.compare("and") == 0 or operater.compare("andi") == 0) {
		return v1 & v2;
	}
	else if (operater.compare("or") == 0 or operater.compare("ori") == 0) {
		return v1 ^ v2;
	}
	else if (operater.compare("slt") == 0 or operater.compare("slti") == 0) {
		return (v1 < v2) ? 1 : 0;
	}
}

//Mike Yang 
std::string getOperation(string instruction, string& operation) {
	int len = instruction.length();
	if (instruction[len - 1] == ':') {
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


void readInstruction(string& target, const string instruction, string& reg1, string& reg2, char type) {
	if (type == 'N') {
		int regCount = 0;
		int i = 0;
		while (i < instruction.length()) {
			if (instruction[i] == '$') {
				switch (regCount) {
				case 0:
					regCount++;
					target = instruction.substr(i + 1, 2);
				case 1:
					if (instruction.substr(i + 1, 4).compare("zero") == 0) {
						reg1 = instruction.substr(i + 1, 4);
					}
					else {
						reg1 = instruction.substr(i + 1, 2);
					}
					regCount++;
				}
			}
			if (instruction[i] == ',' and regCount == 2) {
				if (instruction[i + 1] == '$') {
					reg2 = instruction.substr(i + 2, 2);
				}
				else {
					reg2 = instruction.substr(i + 1, instruction.length() - i - 1);
				}
			}
			i++;
		}
	}
	if (type == 'J') {
		int regCount = 0;
		int i = 0;
		while (i < instruction.length()) {
			if (instruction[i] == '$') {
				switch (regCount) {
				case 0:
					regCount++;
					reg1 = instruction.substr(i + 1, 2);
				case 1:
					reg2 = instruction.substr(i + 1, 2);
				}
			}
			if (regCount == 1 and instruction[i - 1] == ',') {
				int len = instruction.length() - i;
				target = instruction.substr(i, len);
			}
			i++;
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
	std::vector<string> operationStr = { "add", "and", "or", "slt" };
	if (std::find(operationStr.begin(), operationStr.end(), operater) != operationStr.end()) {
		v2 = registers[reg2].second;
		v0 = calculatev0(v1, v2, operater);
	}
	else {
		v2 = stoi(reg2);
		v0 = calculatev0(v1, v2, operater);
	}
	registers[target].second = v0;
}


void printOut(std::vector<std::vector<std::string> > const& cycleStages, std::vector<std::string> const& cycleInstru,
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

int getNop(map<string, int> targets, std::string reg1, std::string reg2) {
	int tp;

	bool t1 = targets.find(reg1) != targets.end();
	bool t2 = targets.find(reg2) != targets.end();
	if (t1 and t2) {
		tp = min(targets[reg1], targets[reg2]);
		return max(6 - tp, 0);
	}
	else if (t1) {
		tp = targets[reg1];
		return max(6 - tp, 0);
	}
	else if (t2) {
		tp = targets[reg2];
		return max(6 - tp, 0);
	}
	else return 0;
}




void cycleStageN(vector<vector<string> > & cycleStages, int i, int j, int stageCurr, vector<int> & nopNum) {
	if (nopNum[j] > 0) {
		cycleStages[j][i] = cycleStages[j][i - 1];
		nopNum[j]--;
	}
	else if (stageCurr == 1) {
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
	else if ((cycleStages[j][i - 1].compare("*") == 0 and cycleStages[j][i - 3].compare("IF") == 0) or cycleStages[j][i - 1].compare("*") == 0 and cycleStages[j][i - 3].compare("ID") == 0) {
		cycleStages[j][i] = "*";
	}
	else if (stageCurr == 5 and cycleStages[j][i - 1] != "*") {
		cycleStages[j][i] = "WB";
	}
	else {
		switch (stageCurr) {
		case 2:
			cycleStages[j][i] = "ID";
		case 3:
			cycleStages[j][i] = "EX";
		case 4:
			cycleStages[j][i] = "MEM";
		}
	}
}


int main(int argc, const char* argv[]) {

	std::map<std::string, std::pair<int, int> > registers;
	registers["s0"] = make_pair(0, 0);
	registers["s1"] = make_pair(0, 0);
	registers["s2"] = make_pair(0, 0);
	registers["s3"] = make_pair(0, 0);
	registers["s4"] = make_pair(0, 0);
	registers["s5"] = make_pair(0, 0);
	registers["s6"] = make_pair(0, 0);
	registers["s7"] = make_pair(0, 0);
	registers["t0"] = make_pair(0, 0);
	registers["t1"] = make_pair(0, 0);
	registers["t2"] = make_pair(0, 0);
	registers["t3"] = make_pair(0, 0);
	registers["t4"] = make_pair(0, 0);
	registers["t5"] = make_pair(0, 0);
	registers["t6"] = make_pair(0, 0);
	registers["t7"] = make_pair(0, 0);
	registers["t8"] = make_pair(0, 0);
	registers["t9"] = make_pair(0, 0);

	std::vector<std::string> allInstru;
	std::map<std::string, int> allBranch;
	std::map<std::string, int> targets;
	std::vector<std::string> cycleInstru;
	std::vector<std::vector<std::string> > cycleStages;
	vector<string> tmp;

	int i = 0;
	while (i < 16) {
		cycleStages.push_back(tmp);
		i++;
	}

	ifstream infile;
	infile.open(argv[2]);

	string instruction;
	int lineNum = 0;
	while (getline(infile, instruction)) {
		lineNum++;
		allInstru.push_back(instruction);
		int len = instruction.length();
		if (instruction[len-1] == ':') {
			string branch = instruction.substr(0, len-1);
			allBranch[branch] = lineNum;
		}
	}
	if (string(argv[1]).compare("F") == 0) {
		cout << "START OF SIMULATION (forwarding)" << endl << "----------------------------------------------------------------------------------" << endl;
		int nextInstru = 0;
		int i = 0;
		while (i < 16) {
			int j = 0;
			while (j <= cycleInstru.size()) {
				std::string instruCurr;
				std::string operation;
				std::string instruType;
				if (j == i) {
					if (cycleInstru.size() != j + 1) {
						if (nextInstru >= allInstru.size()) {
							break;
						}
						instruType = getOperation(allInstru[nextInstru], operation);
						if (instruType.compare("branch") == 0) {
							nextInstru++;
						}
						cycleInstru.push_back(allInstru[nextInstru]);
						nextInstru++;
					}
				}
				if (j == cycleInstru.size()) {
					break;
				}
				instruCurr = cycleInstru[j];
				int stageCurr;
				stageCurr = i - j + 1;
				cyclestage(cycleStages, i, j, stageCurr);

				instruType = getOperation(instruCurr, operation);
				std::string target;
				std::string reg1;
				std::string reg2;

				if (instruType.compare("J") != 0) {
					readInstruction(target, instruCurr, reg1, reg2, 'N');
					if (stageCurr == 3) {
						updateReg(operation, target, reg1, reg2, registers);
					}

				}
				if (instruType.compare("J") == 0) {
					readInstruction(target, instruCurr, reg1, reg2, 'J');
					if (stageCurr == 5) {

						int reg1Val;
						int reg2Val;
						//Qiran Sun
						reg1Val = registers[reg1].second;
						reg2Val = registers[reg2].second;

						if ((operation.compare("beq") == 0 and reg1Val == reg2Val) or (operation.compare("bne") == 0 and reg1Val != reg2Val)) {
							int k = 1;
							while (k < 4) {
								cycleStages[j+k][i] = "*";
								k++;
							}
							nextInstru = allBranch[target];
							cycleInstru.push_back(allInstru[nextInstru]);
							nextInstru++;
						}
					}
				}

				if (stageCurr == 5 and find(cycleStages[j].begin(), cycleStages[j].end(), "*") == cycleStages[j].end()) {
					registers[target].first = registers[target].second;
				}
				if (j == cycleInstru.size() - 1 and stageCurr == 5) {
					printOut(cycleStages, cycleInstru, registers);
					cout << "END OF SIMULATION" << endl;
					return 0;
				}
				j++;
			}
			printOut(cycleStages, cycleInstru, registers);
			i++;
		}
	}
	//Hongbo Zhao
	if (string(argv[1]).compare("N") == 0) {
		cout << "START OF SIMULATION (no forwarding)" << endl << "----------------------------------------------------------------------------------" << endl;
		int nextInstru = 0;
		vector<int> nopNum;
		int i = 0;
		while (i < 16) {
			nopNum.push_back(0);
			i++;
		}
		i = 0;
		while (i < 16) {
			int j = 0;
			while (j <= cycleInstru.size()) {
				std::string instruCurr;
				std::string operation;
				std::string instruType;
				if (j == i) {
					if (cycleInstru.size() != j + 1) {
						if (nextInstru >= allInstru.size()) {
							break;
						}
						instruType = getOperation(allInstru[nextInstru], operation);
						if (instruType.compare("branch") == 0) {
							nextInstru++;
						}
						cycleInstru.push_back(allInstru[nextInstru]);
						nextInstru++;
					}
				}
				if (j == cycleInstru.size()) {
					break;
				}

				instruCurr = cycleInstru[j];
				int stageCurr;
				stageCurr = i - j + 1;

				instruType = getOperation(instruCurr, operation);
				std::string target;
				std::string reg1;
				std::string reg2;
				if (instruType.compare("nop") == 0) {
					cycleStageN(cycleStages, i, j, stageCurr, nopNum);
				}
				else if (instruType.compare("J") != 0) {
					readInstruction(target, instruCurr, reg1, reg2, 'N');
					targets[target] = stageCurr;
					if (stageCurr == 3) {
						int nop = getNop(targets, reg1, reg2);
						if (nop) {
							if (nop == 1) {
								cycleInstru.insert(cycleInstru.begin() + j, nop, "nop");
								vector<string> tempStage = cycleStages[j];
								tempStage[i] = "*";
								cycleStages.insert(cycleStages.begin() + j, nop, tempStage);
								cycleStages[j + nop][i] = "ID";

								if (cycleStages[j + nop + 1].size() != 0) {
									cycleStages[j + nop + 1][i] = "IF";
								}
								j = j + 1;
								break;
							}

							if (nop == 2) {
								cycleInstru.insert(cycleInstru.begin() + j, nop, "nop");
								vector<string> tempStage = cycleStages[j];
								tempStage[i] = "*";
								cycleStages.insert(cycleStages.begin() + j, nop, tempStage);
								cycleStages[j + nop][i] = "ID";

								if (cycleStages[j + nop + 1].size() != 0) {
									cycleStages[j + nop + 1][i] = "IF";
								}

								j = j + 2;
								nopNum[j] = 1;
								nopNum[j+1] = 1;
								break;

							}
						}
						cycleStageN(cycleStages, i, j, stageCurr, nopNum);
					}
					else if (stageCurr == 5) {
						updateReg(operation, target, reg1, reg2, registers);
						registers[target].first = registers[target].second;
						cycleStageN(cycleStages, i, j, stageCurr, nopNum);
					}
					else {
						cycleStageN(cycleStages, i, j, stageCurr, nopNum);
					}
				}
				else if (instruType.compare("J") == 0) {
					readInstruction(target, instruCurr, reg1, reg2, 'J');
					targets[target] = stageCurr;

					if (stageCurr == 3) {
						int nop = getNop(targets, reg1, reg2);
						if (nop) {
							if (nop == 2) {
								cycleInstru.insert(cycleInstru.begin() + j, nop, "nop");
								vector<string> tempStage = cycleStages[j];
								tempStage[i] = "*";
								cycleStages.insert(cycleStages.begin() + j, nop, tempStage);
								cycleStages[j + nop][i] = "ID";

								if (cycleStages[j + nop + 1].size() != 0) {
									cycleStages[j + nop + 1][i] = "IF";
								}
								j = j + 2;
								nopNum[j] = 1;
								nopNum[j + 1] = 1;
							}
							if (nop == 1) {
								cycleInstru.insert(cycleInstru.begin() + j, nop, "nop");
								vector<string> tempStage = cycleStages[j];
								tempStage[i] = "*";
								cycleStages.insert(cycleStages.begin() + j, nop, tempStage);
								cycleStages[j + nop][i] = "ID";

								if (cycleStages[j + nop + 1].size() != 0) {
									cycleStages[j + nop + 1][i] = "IF";
								}
								j = j + 1;
							}
						}
						else {
							cycleStageN(cycleStages, i, j, stageCurr, nopNum);
						}
					}
					else if (stageCurr == 5) {
						cycleStageN(cycleStages, i, j, stageCurr, nopNum);
						int reg1Val;
						int reg2Val;
						reg1Val = registers[reg1].second;
						reg2Val = registers[reg2].second;

						if ((operation.compare("beq") == 0 and reg1Val == reg2Val) or (operation.compare("bne") == 0 and reg1Val != reg2Val)) {
							nextInstru = allBranch[target];
							cycleInstru.push_back(allInstru[nextInstru]);
							nextInstru++;
							cyclestage(cycleStages, i, j + 4, 1);
						}
					}
					else if (i != j) {
						cycleStageN(cycleStages, i, j, stageCurr, nopNum);
					}
				}

				if (j == cycleInstru.size() - 1 and stageCurr == 5) {
					printOut(cycleStages, cycleInstru, registers);
					cout << "END OF SIMULATION" << endl;
					return 0;
				}
				j++;
			}
			printOut(cycleStages, cycleInstru, registers);
			i++;
		}
	}
	cout << "END OF SIMULATION" << endl;
	return 0;
}
