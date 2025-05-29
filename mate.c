#define MATE_IMPLEMENTATION
#include "mate.h"

i32 main() {
  StartBuild();
  {
    CreateExecutable((ExecutableOptions){.output = "main", .flags = "-Wall -Wextra -ggdb -std=c23"});

    AddFile("./src/main.c");
    AddFile("./src/model.c");
    AddFile("./src/renderer.c");
    AddFile("./src/gl.c");

    AddIncludePaths("./vendor/SDL3/include", "./vendor/SDL3_image/include");
    AddLibraryPaths("./vendor/SDL3/lib", "./vendor/SDL3_image/lib");

    AddIncludePaths("./vendor/cglm/", "./vendor/glew/include", "./src");
    AddLibraryPaths("./vendor/glew/lib/Release/x64");

    LinkSystemLibraries("SDL3", "SDL3_image", "glew32", "opengl32");
    LinkSystemLibraries("user32", "gdi32", "shell32", "winmm", "setupapi", "version", "imm32", "ole32");

    String exePath = InstallExecutable();
    RunCommand(exePath);
    CreateCompileCommands();
  }
  EndBuild();
}
