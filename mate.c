#define MATE_IMPLEMENTATION
#include "mate.h"

i32 main() {
  StartBuild();
  {
    Executable exe = CreateExecutable((ExecutableOptions){.output = "main", .flags = "-Wall -Wextra -ggdb -std=c23"});

    AddFile(exe, "./src/main.c");
    AddFile(exe, "./src/camera.c");
    AddFile(exe, "./src/renderer.c");
    AddFile(exe, "./src/gl.c");
    AddFile(exe, "./src/objects/obj.c");

    AddIncludePaths(exe, "./vendor/SDL3/include", "./vendor/SDL3_image/include");
    AddLibraryPaths(exe, "./vendor/SDL3/lib", "./vendor/SDL3_image/lib");

    AddIncludePaths(exe, "./vendor/assimp/include");
    AddLibraryPaths(exe, "./vendor/assimp/lib");

    AddIncludePaths(exe, "./vendor/glew/include");
    AddLibraryPaths(exe, "./vendor/glew/lib");

    AddIncludePaths(exe, "./src", "./vendor/base/", "./vendor/cglm/");

    FileCopy(S("./vendor/SDL3/bin/SDL3.dll"), S("./build/SDL3.dll"));
    FileCopy(S("./vendor/SDL3_image/bin/SDL3_image.dll"), S("./build/SDL3_image.dll"));
    FileCopy(S("./vendor/glew/bin/glew32.dll"), S("./build/glew32.dll"));
    FileCopy(S("./vendor/assimp/bin/assimp-vc143-mt.dll"), S("./build/assimp-vc143-mt.dll"));

    LinkSystemLibraries(exe, "assimp", "SDL3", "SDL3_image", "glew32", "opengl32");
    LinkSystemLibraries(exe, "user32", "gdi32", "shell32", "winmm", "setupapi", "version", "imm32", "ole32");

    InstallExecutable(exe);
    RunCommand(exe.outputPath);
    CreateCompileCommands(exe);
  }
  EndBuild();
}
