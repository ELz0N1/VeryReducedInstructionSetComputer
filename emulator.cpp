#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace Emulator {
enum Reg { R0, R1, R2, R3 };

struct EmulatorState {
  static const size_t regs_size = 4;
  static const size_t mem_size = 1024;

  std::vector<int> _registers{regs_size};
  std::vector<int> _mem{mem_size};

  size_t _pc = 0;  // program counter register
};

// TODO: implement all instructions listed in ISA. This class should be base
// class for all insturctions
class Instruction {
 public:
  virtual void eval(EmulatorState& emul) = 0;
  virtual ~Instruction() {};
};

class MovReg : public Instruction {
  Reg dst_, src_;

 public:
  MovReg(Reg dst, Reg src) : dst_(dst), src_(src) {}
  void eval(EmulatorState& emul) override {
    emul._registers[dst_] = emul._registers[src_];
    ++emul._pc;
  }
};

class MovImm : public Instruction {
  Reg reg_;
  int val_;

 public:
  MovImm(Reg reg, int val) : reg_(reg), val_(val) {}
  void eval(EmulatorState& emul) override {
    emul._registers[reg_] = val_;
    ++emul._pc;
  }
};

class AddReg : public Instruction {
  Reg lhs_, rhs_;

 public:
  AddReg(Reg lhs, Reg rhs) : lhs_(lhs), rhs_(rhs) {}
  void eval(EmulatorState& emul) override {
    emul._registers[lhs_] += emul._registers[rhs_];
    ++emul._pc;
  }
};

class AddImm : public Instruction {
  Reg reg_;
  int val_;

 public:
  AddImm(Reg reg, int val) : reg_(reg), val_(val) {}
  void eval(EmulatorState& emul) override {
    emul._registers[reg_] += val_;
    ++emul._pc;
  }
};

class SubReg : public Instruction {
  Reg lhs_, rhs_;

 public:
  SubReg(Reg lhs, Reg rhs) : lhs_(lhs), rhs_(rhs) {}
  void eval(EmulatorState& emul) override {
    emul._registers[lhs_] -= emul._registers[rhs_];
    ++emul._pc;
  }
};

class SubImm : public Instruction {
  Reg reg_;
  int val_;

 public:
  SubImm(Reg reg, int val) : reg_(reg), val_(val) {}
  void eval(EmulatorState& emul) override {
    emul._registers[reg_] -= val_;
    ++emul._pc;
  }
};

class MulReg : public Instruction {
  Reg lhs_, rhs_;

 public:
  MulReg(Reg lhs, Reg rhs) : lhs_(lhs), rhs_(rhs) {}
  void eval(EmulatorState& emul) override {
    emul._registers[lhs_] *= emul._registers[rhs_];
    ++emul._pc;
  }
};

class MulImm : public Instruction {
  Reg reg_;
  int val_;

 public:
  MulImm(Reg reg, int val) : reg_(reg), val_(val) {}
  void eval(EmulatorState& emul) override {
    emul._registers[reg_] *= val_;
    ++emul._pc;
  }
};

class DivReg : public Instruction {
  Reg lhs_, rhs_;

 public:
  DivReg(Reg lhs, Reg rhs) : lhs_(lhs), rhs_(rhs) {}
  void eval(EmulatorState& emul) override {
    emul._registers[lhs_] /= emul._registers[rhs_];
    ++emul._pc;
  }
};

class DivImm : public Instruction {
  Reg reg_;
  int val_;

 public:
  DivImm(Reg reg, int val) : reg_(reg), val_(val) {}
  void eval(EmulatorState& emul) override {
    emul._registers[reg_] /= val_;
    ++emul._pc;
  }
};

class Store : public Instruction {
  Reg reg_;
  int addr_;

 public:
  Store(Reg reg, int addr) : reg_(reg), addr_(addr) {}
  void eval(EmulatorState& emul) override {
    emul._mem[addr_] = emul._registers[reg_];
    ++emul._pc;
  }
};

class Load : public Instruction {
  Reg reg_;
  int addr_;

 public:
  Load(Reg reg, int addr) : reg_(reg), addr_(addr) {}
  void eval(EmulatorState& emul) override {
    emul._registers[reg_] = emul._mem[addr_];
    ++emul._pc;
  }
};

class Jmp : public Instruction {
  size_t target_;

 public:
  explicit Jmp(size_t target) : target_(target) {}
  void eval(EmulatorState& emul) override { emul._pc = target_; }
};

class Jmpz : public Instruction {
  size_t target_;

 public:
  explicit Jmpz(size_t target) : target_(target) {}
  void eval(EmulatorState& emul) override {
    if (emul._registers[R0] == 0) {
      emul._pc = target_;
    } else {
      ++emul._pc;
    }
  }
};

static int parseInt(const std::string& token) { return std::stoi(token, 0); }

static Reg parseReg(const std::string& token) {
  return static_cast<Reg>(parseInt(token.substr(1)));
}

/* This function accepts the program written in the vrisc assembly
 * If the input program is correct, returns sequence of insturction,
 * corresponding to the input program. If the input text is incorrect, the
 * behaviour is undefined
 */
std::vector<Instruction*> parse(const std::string& program) {
  std::vector<Instruction*> instructions;
  std::stringstream lines{program};
  std::string line;

  while (std::getline(lines, line)) {
    std::istringstream iss{line};
    std::vector<std::string> tok;

    for (std::string t; iss >> t;) {
      tok.push_back(t);
    }

    if (tok.empty()) {
      continue;
    }

    const std::string op = tok[0];

    if (op == "Mov" && tok[1] == "Jmpz") {
      int target = parseInt(tok[2]);
      instructions.push_back(new Jmpz(target));
      continue;
    }

    if (op == "Mov") {
      Reg dst = parseReg(tok[1]);
      if (tok[2].find("R", 0) == 0) {
        Reg src = parseReg(tok[2]);
        instructions.push_back(new MovReg(dst, src));
      } else {
        int imm = parseInt(tok[2]);
        instructions.push_back(new MovImm(dst, imm));
      }
    } else if (op == "Add") {
      Reg dst = parseReg(tok[1]);
      if (tok[2].find("R", 0) == 0) {
        Reg src = parseReg(tok[2]);
        instructions.push_back(new AddReg(dst, src));
      } else {
        int imm = parseInt(tok[2]);
        instructions.push_back(new AddImm(dst, imm));
      }
    } else if (op == "Sub") {
      Reg dst = parseReg(tok[1]);
      if (tok[2].find("R", 0) == 0) {
        Reg src = parseReg(tok[2]);
        instructions.push_back(new SubReg(dst, src));
      } else {
        int imm = parseInt(tok[2]);
        instructions.push_back(new SubImm(dst, imm));
      }
    } else if (op == "Mul") {
      Reg dst = parseReg(tok[1]);
      if (tok[2].find("R", 0) == 0) {
        Reg src = parseReg(tok[2]);
        instructions.push_back(new MulReg(dst, src));
      } else {
        int imm = parseInt(tok[2]);
        instructions.push_back(new MulImm(dst, imm));
      }
    } else if (op == "Div") {
      Reg dst = parseReg(tok[1]);
      if (tok[2].find("R", 0) == 0) {
        Reg src = parseReg(tok[2]);
        instructions.push_back(new DivReg(dst, src));
      } else {
        int imm = parseInt(tok[2]);
        instructions.push_back(new DivImm(dst, imm));
      }
    } else if (op == "Store") {
      Reg dst = parseReg(tok[1]);
      int addr = parseInt(tok[2]);
      instructions.push_back(new Store(dst, addr));
    } else if (op == "Load") {
      Reg dst = parseReg(tok[1]);
      int addr = parseInt(tok[2]);
      instructions.push_back(new Load(dst, addr));
    } else if (op == "Jmp") {
      int target = parseInt(tok[1]);
      instructions.push_back(new Jmp(target));
    }
  }

  return instructions;
}

/* Emulate receive a program, written in the vrisc assembly,
 * in case of the correct program, emulate returns R0 value at the end of the
 * emulation. If the program is incorrect, that is, either its text is not vrisc
 * assembly language or it contains UB(endless cycles), the behaviour of emulate
 * if also undefined. Handle these cases in any way.
 */
int emulate(const std::string& program_text) {
  // Feel free to change code of this function
  std::vector<Instruction*> program = parse(program_text);

  EmulatorState state;

  while (state._pc < program.size()) {
    program[state._pc]->eval(state);
  }

  for (size_t i = 0; i < program.size(); i++) {
    delete program[i];
  }

  return state._registers[R0];
}
}  // namespace Emulator

// Simple helper for file as single line. Feel free to change it or delete it
// completely
std::optional<std::string> readStringFromFile(const std::string& filename) {
  std::ifstream file{filename};

  if (!file) return {};

  std::stringstream buf;
  buf << file.rdbuf();

  return buf.str();
}

int main() {
  // For writing test you can write programs directly in raw string literals
  std::string factorial = R"(
    Mov R0 5
    Mov R1 1
    Mov Jmpz 6

    Mul R1 R0
    Sub R0 1
    Jmp 2

    Mov R0 R1
  )";

  // The result should be 120
  int fact5 = Emulator::emulate(factorial);

  // Or you can use file IO
  std::string filename = "factorial.vrisc";
  std::optional<std::string> program = readStringFromFile("factorial.vrisc");

  if (!program) {
    std::cerr << "Can't open file" << std::endl;
    return 1;
  }

  // And this also should be 120
  int another_fact = Emulator::emulate(*program);

  // TODO: remeber the tests is very important!

  return 0;
}
