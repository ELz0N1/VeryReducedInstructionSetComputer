#include "emulator.cpp"
#include "gtest/gtest.h"

TEST(Emulator, BasicTest) {
  std::string program = R"(
    Mov R0 5
    Mov R1 1    
    Mov Jmpz 6  
                  
    Mul R1 R0   
    Sub R0 1    
    Jmp 2       

    Mov R0 R1
  )";

  EXPECT_EQ(Emulator::emulate(program), 120);
}

TEST(Emulator, ReadFromIOTest) {
  std::string filename = "factorial.vrisc";
  std::optional<std::string> program = readStringFromFile("factorial.vrisc");

  if (!program) {
    FAIL() << "Can't open file";
  }

  int another_fact = Emulator::emulate(*program);

  EXPECT_EQ(another_fact, 120);
}

TEST(Emulator, MulAndDivTest) {
  std::string program = R"(
    Mov R0 2
    Mov R1 3
    Mul R1 4
    Mul R0 R1
    Div R0 3
  )";

  EXPECT_EQ(Emulator::emulate(program), 8);
}

TEST(Emulator, LoadAndStoreTest) {
  std::string program = R"(
    Mov R0 42
    Store R0 10
    Mov R0 0
    Load R0 10
  )";

  EXPECT_EQ(Emulator::emulate(program), 42);
}

TEST(Emulator, JmpTest) {
  std::string program = R"(
    Mov R0 5
    Jmp 3
    Mov R0 99
    Mov R0 6
  )";

  EXPECT_EQ(Emulator::emulate(program), 6);
}

TEST(Emulator, JmpzTest) {
  std::string program = R"(
    Mov R0 0
    Mov Jmpz 3
    Add R0 1
    Add R0 1
  )";

  EXPECT_EQ(Emulator::emulate(program), 1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}