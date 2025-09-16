#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define TEST_FOLDER "test/"

int main(int argc, char **argv)
{
  NOB_GO_REBUILD_URSELF(argc, argv);

  if(!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;
  Nob_Cmd cmd = {0};

  // building main
  nob_cmd_append(&cmd,
                 "clang",
                 "-Wall",
                 "-Wextra",
                 "-I./include",
                 "-std=c17",
                 "-o",
                 BUILD_FOLDER"circuita",
                 SRC_FOLDER"main.c",
                 SRC_FOLDER"components/resistor.c");

  if(!nob_cmd_run(&cmd)) return 1;

  // building tests
  nob_cmd_append(&cmd,
                 "clang",
                 "-Wall",
                 "-Wextra",
                 "-I./include",
                 "-std=c17",
                 "-o",
                 BUILD_FOLDER"test_resistor",
                 TEST_FOLDER"test_resistor.c",
                 SRC_FOLDER"components/resistor.c"
                 );
  if(!nob_cmd_run(&cmd)) return 1;

  return 0;
}
