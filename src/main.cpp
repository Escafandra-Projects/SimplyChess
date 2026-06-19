#include "core/Game.h"

#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <limits.h>

#ifdef __APPLE__
  #include <mach-o/dyld.h>
#elif defined(_WIN32)
  #include <windows.h>
#endif

/*
    SimplyChess v 0.7 (Beta)
    Fecha de creación: 03/09/2020
*/

static void setWorkingDirectoryToExecutable() {
    std::filesystem::path exePath;
#ifdef __APPLE__
    char buf[PATH_MAX];
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) == 0) {
        // Resolve symlinks just in case
        std::error_code ec;
        exePath = std::filesystem::canonical(buf, ec);
        if (ec) exePath = buf;
    }
#elif defined(__linux__)
    std::error_code ec;
    exePath = std::filesystem::canonical("/proc/self/exe", ec);
#elif defined(_WIN32)
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    exePath = buf;
#endif
    if (!exePath.empty()) {
        std::error_code ec;
        auto dir = exePath.parent_path();
#ifdef __APPLE__
        if (dir.filename() == "MacOS") {
            dir = dir.parent_path() / "Resources";
        }
#endif
        std::filesystem::current_path(dir, ec);
    }
}

int main() {
    // Establecer el directorio de trabajo al del ejecutable para cargar recursos correctamente
    setWorkingDirectoryToExecutable();

    // Crea y ejecuta el juego, capturando errores de inicialización (carga de recursos).
    try {
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error fatal: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
