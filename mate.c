#define MATE_IMPLEMENTATION
#include "mate.h"

i32 main() {
  StartBuild();
  {
    Executable exe = CreateExecutable((ExecutableOptions){.output = "main", .flags = "-Wall -Wextra -ggdb -std=c23"});

    AddFile(exe, "./src/main.c");
    AddFile(exe, "./src/model.c");
    AddFile(exe, "./src/camera.c");
    AddFile(exe, "./src/renderer.c");
    AddFile(exe, "./src/gl.c");
    AddFile(exe, "./src/objects/obj.c");

    AddIncludePaths(exe, "./vendor/SDL3/include", "./vendor/SDL3_image/include", "./vendor/base/");
    AddLibraryPaths(exe, "./vendor/SDL3/lib", "./vendor/SDL3_image/lib");

    AddIncludePaths(exe, "./vendor/assimp/include");
    AddLibraryPaths(exe, "./vendor/assimp/lib");

    AddIncludePaths(exe, "./vendor/cglm/", "./vendor/glew/include", "./src");
    AddLibraryPaths(exe, "./vendor/glew/lib/Release/x64");

    LinkSystemLibraries(exe, "assimp", "SDL3", "SDL3_image", "glew32", "opengl32");
    LinkSystemLibraries(exe, "user32", "gdi32", "shell32", "winmm", "setupapi", "version", "imm32", "ole32");

    InstallExecutable(exe);
    RunCommand(exe.outputPath);
    CreateCompileCommands(exe);
  }
  EndBuild();
}
