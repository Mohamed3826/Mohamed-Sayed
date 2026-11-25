#include <bits/stdc++.h>
using namespace std;

class Memory {
private:
    vector<string> mem;
public:
    Memory() {
        mem.resize(256, "00");
    }

    string get(int addr) {
        if (addr >= 0 && addr < 256) {
            return mem[addr];
        }
        return "00";
    }

    void set(int addr, string val) {
        if (addr >= 0 && addr < 256) {
            if (val.size() == 1) {
                val = "0" + val;
            }
            for (int i = 0; i < val.size(); i++) {
                val[i] = toupper(val[i]);
            }
            mem[addr] = val;
        }
    }

    void reset() {
        for (int i = 0; i < 256; i++) {
            mem[i] = "00";
        }
    }
};

class Register {
private:
    vector<string> reg;
public:
    Register() {
        reg.resize(16, "00");
    }

    string get(int idx) {
        if (idx >= 0 && idx < 16) {
            return reg[idx];
        }
        return "00";
    }

    void set(int idx, string val) {
        if (idx >= 0 && idx < 16) {
            if (val.size() == 1) {
                val = "0" + val;
            }
            for (int i = 0; i < val.size(); i++) {
                val[i] = toupper(val[i]);
            }
            reg[idx] = val;
        }
    }

    void reset() {
        for (int i = 0; i < 16; i++) {
            reg[i] = "00";
        }
    }
};

class ALU {
public:
    int toDec(string h) {
        int res = 0;
        stringstream ss;
        ss << hex << h;
        ss >> res;
        return res;
    }

    string toHex(int d) {
        d = d % 256;
        if (d < 0) d += 256;
        stringstream ss;
        ss << uppercase << hex << d;
        string res = ss.str();
        if (res.size() == 1) {
            res = "0" + res;
        }
        return res;
    }

    string add(string a, string b) {
        int x = toDec(a);
        int y = toDec(b);
        return toHex(x + y);
    }
};

class CPU {
private:
    int pc;
    string ir;
    Register R;
    ALU alu;
    bool stop;

public:
    CPU() {
        pc = 0;
        ir = "0000";
        stop = true;
    }

    void reset() {
        pc = 0;
        ir = "0000";
        R.reset();
        stop = false;
    }

    bool isStopped() {
        return stop;
    }

    void step(Memory &M, string &screen) {
        if (stop || pc >= 254) {
            stop = true;
            return;
        }

        ir = M.get(pc) + M.get(pc + 1);
        if (ir == "0000") {
            stop = true;
            return;
        }
        pc += 2;

        char op = toupper(ir[0]);
        int r = alu.toDec(string(1, ir[1]));
        int x = alu.toDec(string(1, ir[2]));
        int y = alu.toDec(string(1, ir[3]));
        int xy = alu.toDec(ir.substr(2, 2));

        if (op == '1') {
            R.set(r, M.get(xy));
        } else if (op == '2') {
            R.set(r, ir.substr(2, 2));
        } else if (op == '3') {
            if (xy == 0) {
                screen += R.get(r) + " ";
            } else {
                M.set(xy, R.get(r));
            }
        } else if (op == '4') {
            R.set(y, R.get(x));
        } else if (op == '5') {
            R.set(r, alu.add(R.get(x), R.get(y)));
        } else if (op == 'B') {
            if (R.get(r) == R.get(0)) {
                pc = xy;
            }
        } else if (op == 'C') {
            stop = true;
        }
    }

    void showStatus(Memory &M, string screen) {
        cout << "\nPC: " << alu.toHex(pc) << " | IR: " << ir << "\n";
        cout << "Screen: " << screen << "\n";
        cout << "Registers:\n";
        for (int i = 0; i < 16; i++) {
            cout << "R" << uppercase << hex << i << "=" << R.get(i) << "  ";
            if ((i + 1) % 4 == 0) cout << "\n";
        }
        cout << "Memory (00 to 1F):\n";
        for (int i = 0; i < 32; i++) {
            if (i < 16) cout << "0";
            cout << uppercase << hex << i << ":" << M.get(i) << " ";
            if ((i + 1) % 8 == 0) cout << "\n";
        }
        cout << dec;
    }
};

class Machine {
private:
    CPU cpu;
    Memory mem;
    string screen;

public:
    void loadFile(string fname) {
        ifstream fin(fname);
        if (!fin.is_open()) {
            cout << "File not found!\n";
            return;
        }
        cpu.reset();
        mem.reset();
        screen = "";

        string line;
        int addr = 0;
        while (fin >> line && addr < 254) {
            if (line.size() == 4) {
                mem.set(addr, line.substr(0, 2));
                mem.set(addr + 1, line.substr(2, 2));
                addr += 2;
            }
        }
        fin.close();
        cout << "Loaded from file successfully.\n";
    }

    void loadConsole() {
        cpu.reset();
        mem.reset();
        screen = "";

        cout << "Enter instructions (type END to stop):\n";
        string line;
        int addr = 0;
        while (cin >> line) {
            for (int i = 0; i < line.size(); i++) line[i] = toupper(line[i]);
            if (line == "END") break;
            if (line.size() == 4 && addr < 254) {
                mem.set(addr, line.substr(0, 2));
                mem.set(addr + 1, line.substr(2, 2));
                addr += 2;
            } else {
                cout << "Invalid! Enter 4 hex chars or END.\n";
            }
        }
        cout << "Loaded from console successfully.\n";
    }

    void runStep() {
        cpu.step(mem, screen);
        cpu.showStatus(mem, screen);
    }

    void runAll() {
        int limit = 0;
        while (!cpu.isStopped() && limit < 1000) {
            cpu.step(mem, screen);
            limit++;
        }
        cout << "\nFinal Screen Output: " << screen << "\n";
    }

    void display() {
        cpu.showStatus(mem, screen);
    }
};

int main() {
    Machine vm;
    string choice;

    while (true) {
        cout << "\n1. Load from File\n";
        cout << "2. Enter in Console\n";
        cout << "3. Run One Step\n";
        cout << "4. Run All\n";
        cout << "5. Show Registers & Memory\n";
        cout << "6. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == "1") {
            string fname;
            cout << "Enter file name: ";
            cin >> fname;
            vm.loadFile(fname);
        } else if (choice == "2") {
            vm.loadConsole();
        } else if (choice == "3") {
            vm.runStep();
        } else if (choice == "4") {
            vm.runAll();
        } else if (choice == "5") {
            vm.display();
        } else if (choice == "6") {
            break;
        } else {
            cout << "Wrong choice!\n";
        }
    }
    return 0;
}