#define NOB_IMPLEMENTATION
#include "nob.h"


#define BUILD_DIR "build/"

int main(int argc, char **argv)
{
	nob_mkdir_if_not_exists(BUILD_DIR);
	NOB_GO_REBUILD_URSELF(argc, argv);
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "cc", "-std=c99", "-Wall", "-Wextra", "-o", BUILD_DIR"main");
	nob_cmd_append(&cmd, "src/demo/main.c");


	nob_cmd_append(&cmd, "-lraylib");
	if (!nob_cmd_run(&cmd)) return 1;
	return 0;
}
