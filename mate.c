#define MATE_IMPLEMENTATION
#include "mate.h"

#define ERROR_COUNT 4

i32 main() {
  StartBuild();
  {
    Executable exe = CreateExecutable((ExecutableOptions){.output = "main", .flags = "-Wall -Wextra -ggdb -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_SDL3 -DIMGUI_IMPL_OPENGL_LOADER_GL3W"});
    AddFile(exe, "./src/main.c");
    AddFile(exe, "./src/camera.c");
    AddFile(exe, "./src/renderer.c");
    AddFile(exe, "./src/ui.c");
    AddFile(exe, "./src/gl.c");
    AddFile(exe, "./src/shader.c");
    AddFile(exe, "./src/objects/obj.c");
    AddFile(exe, "./src/scenes/scene.c");

    AddIncludePaths(exe, "./vendor/SDL3/include", "./vendor/SDL3_image/include");
    AddLibraryPaths(exe, "./vendor/SDL3/lib", "./vendor/SDL3_image/lib");

    AddIncludePaths(exe, "./vendor/assimp/include");
    AddLibraryPaths(exe, "./vendor/assimp/lib");

    AddIncludePaths(exe, "./vendor/glew/include");
    AddLibraryPaths(exe, "./vendor/glew/lib");

    AddIncludePaths(exe, "./vendor/cimgui");
    AddLibraryPaths(exe, "./vendor/cimgui/lib");

    AddIncludePaths(exe, "./src", "./vendor/base/", "./vendor/cglm/");

    // Copy runtime DLLs
    errno_t errors[ERROR_COUNT];
    errors[0] = FileCopy(S("./vendor/SDL3/bin/SDL3.dll"), S("./build/SDL3.dll"));
    errors[1] = FileCopy(S("./vendor/SDL3_image/bin/SDL3_image.dll"), S("./build/SDL3_image.dll"));
    errors[2] = FileCopy(S("./vendor/glew/bin/glew32.dll"), S("./build/glew32.dll"));
    errors[3] = FileCopy(S("./vendor/assimp/bin/assimp-vc143-mt.dll"), S("./build/assimp-vc143-mt.dll"));

    for (size_t i = 0; i < ERROR_COUNT; i++) {
      errno_t currError = errors[i];
      Assert(currError == SUCCESS, "FileCopy: failed, with error %d, on index %zu", currError, i);
    }

    LinkSystemLibraries(exe, "assimp", "glew32", "opengl32");
    LinkSystemLibraries(exe, "cimgui_sdl");
    LinkSystemLibraries(exe, "SDL3", "SDL3_image");
    LinkSystemLibraries(exe, "user32", "gdi32", "shell32", "winmm", "setupapi", "version", "imm32", "ole32");
    LinkSystemLibraries(exe, "stdc++");

    InstallExecutable(exe);
    RunCommand(exe.outputPath);
    CreateCompileCommands(exe);
  }
  EndBuild();
}
