#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <optional>

namespace Emulator {
  enum Reg {
    R0, R1, R2, R3
  };

  class EmulatorState;

  // TODO: implement all instructions listed in ISA. This class should be base class for all insturctions
  class Instruction {
  public:
    virtual void eval(EmulatorState& emul) = 0;
    virtual ~Instruction() {};
  };

  /* This function accepts the program written in the vrisc assembly
   * If the input program is correct, returns sequence of insturction, corresponding to the input program.
   * If the input text is incorrect, the behaviour is undefined
   */
  std::vector<Instruction*> parse(const std::string& program) {
    // TODO: implement it!
    // feel free to change signature of this function
    return std::vector<Instruction*>{};
  }
  
  struct EmulatorState {
    static const size_t regs_size = 4;
    static const size_t mem_size  = 1024;

    std::vector<int> _registers{regs_size}; 
    std::vector<int> _mem{mem_size};

    size_t _pc = 0; // program counter register
  };

  /* Emulate receive a program, written in the vrisc assembly,
   * in case of the correct program, emulate returns R0 value at the end of the emulation.
   * If the program is incorrect, that is, either its text is not vrisc assembly language or it contains UB(endless cycles),
   * the behaviour of emulate if also undefined. Handle these cases in any way.
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
}

// Simple helper for file as single line. Feel free to change it or delete it completely
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
