#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <cstring>
#include <algorithm>
#include <iomanip>
using namespace std;


void cyclestage(int i, int j, vector<vector<string>>& cycleStages, int stageCurr) {
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

void cycleStageN(int i, int j, int stageCurr, vector<vector<string> >& cycleStages, vector<int>& nopNum) {
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

void updateReg(std::string operater, std::map<std::string, std::pair<int, int> >& registers, std::string target, std::string reg1, std::string reg2
	) {

	int val = 0;
	int val1;
	int val2;

	if (reg1.compare("zero") == 0) {
		val1 = 0;
	}
	else {
		val1 = registers[reg1].second;
	}
	std::vector<string> operationStr = { "add", "and", "or", "slt" };
	if (std::find(operationStr.begin(), operationStr.end(), operater) != operationStr.end()) {
		val2 = registers[reg2].second;
		val = calculatev0(val1, val2, operater);
	}
	else {
		val2 = stoi(reg2);
		val = calculatev0(val1, val2, operater);
	}
	registers[target].second = val;
}


void printOut(std::vector<std::vector<std::string> > const& cycleStages, std::vector<std::string> const& cycleInstru,
	std::map<std::string, std::pair<int, int> > registers)
{
	int instruCount = cycleInstru.size();
	
	std::cout << "CPU Cycles ===>     1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16" << std::endl;

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
	cout << "$t9 = " << registers["t9"].first << endl << "----------------------------------------------------------------------------------\n";
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
		int index = instruction.length() - 1;
		lineNum++;
		allInstru.push_back(instruction);
		if (instruction[index] == ':') {
			string branch = instruction.substr(0, index);
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
				std::string reg1;
				std::string reg2;
				int stageCurr = i + 1 - j;
				std::string target;

				instruType = getOperation(instruCurr, operation);
				cyclestage(i, j, cycleStages, stageCurr);

				if (instruType.compare("J") != 0) {
					readInstruction(target, instruCurr, reg1, reg2, 'N');
					if (stageCurr == 3) {
						updateReg(operation, registers, target, reg1, reg2);
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

				std::string target;
				std::string reg1;
				std::string reg2;
				instruCurr = cycleInstru[j];
				int stageCurr;
				stageCurr = i - j + 1;

				instruType = getOperation(instruCurr, operation);
				
				if (instruType.compare("nop") == 0) {
					cycleStageN(i, j, stageCurr, cycleStages, nopNum);
				}
				else if (instruType.compare("J") != 0) {
					readInstruction(target, instruCurr, reg1, reg2, 'N');
					targets[target] = stageCurr;
					if (stageCurr == 3) {
						int nop = getNop(targets, reg1, reg2);
						if (nop) {
							if (nop == 1) {

								std::vector<string> cycleNew;
								for (int i = 0; i < j; i++) {
									cycleNew.push_back(cycleInstru[i]);
								}
								for (int m = 0; m < nop; m++) {
									cycleNew.push_back("nop");
								}
								for (int n = j; n < cycleInstru.size(); n++) {
									cycleNew.push_back(cycleInstru[n]);
								}
								cycleInstru = cycleNew;


								vector<string> tempStage = cycleStages[j];
								tempStage[i] = "*";
								std::vector<std::vector<std::string> > cycleNew1;
								for (int i = 0; i < j; i++) {
									cycleNew1.push_back(cycleStages[i]);
								}
								for (int m = 0; m < nop; m++) {
									cycleNew1.push_back(tempStage);
								}
								for (int n = j; n < cycleInstru.size(); n++) {
									cycleNew1.push_back(cycleStages[n]);
								}
								cycleStages = cycleNew1;

								cycleStages[j + nop][i] = "ID";

								if (cycleStages[j + nop + 1].size() > 0) {
									cycleStages[j + nop + 1][i] = "IF";
								}
								j = j + 1;
								break;
							}

							if (nop == 2) {
								std::vector<string> cycleNew;
								for (int i = 0; i < j; i++) {
									cycleNew.push_back(cycleInstru[i]);
								}
								for (int m = 0; m < nop; m++) {
									cycleNew.push_back("nop");
								}
								for (int n = j; n < cycleInstru.size(); n++) {
									cycleNew.push_back(cycleInstru[n]);
								}
								cycleInstru = cycleNew;

								vector<string> tempStage = cycleStages[j];
								tempStage[i] = "*";
								std::vector<std::vector<std::string> > cycleNew1;
								for (int i = 0; i < j; i++) {
									cycleNew1.push_back(cycleStages[i]);
								}
								for (int m = 0; m < nop; m++) {
									cycleNew1.push_back(tempStage);
								}
								for (int n = j; n < cycleInstru.size(); n++) {
									cycleNew1.push_back(cycleStages[n]);
								}
								cycleStages = cycleNew1;

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
						cycleStageN(i, j, stageCurr, cycleStages, nopNum);
					}
					else if (stageCurr == 5) {
						updateReg(operation, registers, target, reg1, reg2);
						registers[target].first = registers[target].second;
						cycleStageN(i, j, stageCurr, cycleStages, nopNum);
					}
					else {
						cycleStageN(i, j, stageCurr, cycleStages, nopNum);
					}
				}
				else if (instruType.compare("J") == 0) {
					readInstruction(target, instruCurr, reg1, reg2, 'J');
					targets[target] = stageCurr;

					if (stageCurr == 3) {
						int nop = getNop(targets, reg1, reg2);
						if (nop) {
							if (nop == 2) {
								std::vector<string> cycleNew;
								for (int i = 0; i < j; i++) {
									cycleNew.push_back(cycleInstru[i]);
								}
								for (int m = 0; m < nop; m++) {
									cycleNew.push_back("nop");
								}
								for (int n = j; n < cycleInstru.size(); n++) {
									cycleNew.push_back(cycleInstru[n]);
								}
								cycleInstru = cycleNew;

								vector<string> tempStage = cycleStages[j];
								tempStage[i] = "*";
								std::vector<std::vector<std::string> > cycleNew1;
								for (int i = 0; i < j; i++) {
									cycleNew1.push_back(cycleStages[i]);
								}
								for (int m = 0; m < nop; m++) {
									cycleNew1.push_back(tempStage);
								}
								for (int n = j; n < cycleInstru.size(); n++) {
									cycleNew1.push_back(cycleStages[n]);
								}
								cycleStages = cycleNew1;

								cycleStages[j + nop][i] = "ID";

								if (cycleStages[j + nop + 1].size() != 0) {
									cycleStages[j + nop + 1][i] = "IF";
								}
								j = j + 2;
								nopNum[j] = 1;
								nopNum[j + 1] = 1;
							}
							if (nop == 1) {
								std::vector<string> cycleNew;
								for (int i = 0; i < j; i++) {
									cycleNew.push_back(cycleInstru[i]);
								}
								for (int m = 0; m < nop; m++) {
									cycleNew.push_back("nop");
								}
								for (int n = j; n < cycleInstru.size(); n++) {
									cycleNew.push_back(cycleInstru[n]);
								}
								cycleInstru = cycleNew;

								vector<string> tempStage = cycleStages[j];
								tempStage[i] = "*";
								std::vector<std::vector<std::string> > cycleNew1;
								for (int i = 0; i < j; i++) {
									cycleNew1.push_back(cycleStages[i]);
								}
								for (int m = 0; m < nop; m++) {
									cycleNew1.push_back(tempStage);
								}
								for (int n = j; n < cycleInstru.size(); n++) {
									cycleNew1.push_back(cycleStages[n]);
								}
								cycleStages = cycleNew1;
								cycleStages[j + nop][i] = "ID";

								if (cycleStages[j + nop + 1].size() != 0) {
									cycleStages[j + nop + 1][i] = "IF";
								}
								j++;
							}
						}
						else {
							cycleStageN(i, j, stageCurr, cycleStages, nopNum);
						}
					}
					else if (stageCurr == 5) {
						int reg1Val = registers[reg1].second;
						int reg2Val = registers[reg2].second;

						cycleStageN(i, j, stageCurr, cycleStages, nopNum);
						
						if ((operation.compare("beq") == 0 and reg1Val == reg2Val) or (operation.compare("bne") == 0 and reg1Val != reg2Val)) {
							nextInstru = allBranch[target];
							cycleInstru.push_back(allInstru[nextInstru]);
							nextInstru++;
							int l = j + 4;
							cyclestage(i, l, cycleStages,  1);
						}
					}
					else if (i != j) {
						cycleStageN(i, j, stageCurr, cycleStages, nopNum);
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
