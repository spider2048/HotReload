#include <common.h>

HMODULE prevLibHandle = nullptr;
std::ofstream logFile("reload.log", std::ios::ate);

fs::file_time_type lastWriteTime{};
int reloadCount = 0;
fs::path tempFilePath = "";

// TODO: Use directory watchers
void _worker() {
    fs::path reloadPath = std::getenv("RELOAD");  // may change

    if (!fs::exists(reloadPath)) {
        logFile << fmt::format("[-] dll at `{}` doesn't exist\n", reloadPath.string());
        return;
    }

    // check access time
    auto currentWriteTime = fs::last_write_time(reloadPath);
    if (currentWriteTime == lastWriteTime) return;
    lastWriteTime = currentWriteTime;

    // copy original file to temp location

    fs::path newTempFilePath = std::tmpnam(nullptr) / reloadPath.filename();
    fs::create_directory(newTempFilePath.parent_path());

    try {
        fs::copy(reloadPath, newTempFilePath);
    } catch (std::exception& ex) {
        // failed to copy => should not happen
        logFile << "[-] critical exception:\n";
        logFile << "copy failed from src: `" << reloadPath << "`\n";
        logFile << "to dst`" << newTempFilePath << "`\n";
        logFile << "due to: " << ex.what() << "\n";
        MessageBoxA(NULL,
                    fmt::format("failed to copy file to {} from {} due to {}",
                                newTempFilePath.string(), reloadPath.string(), ex.what())
                        .c_str(),
                    "Critical error", MB_OK);

        return;
    }

    HMODULE libHandle = LoadLibraryW(newTempFilePath.c_str());
    logFile << fmt::format("[+] loading reload dll at `{}` handle {}\n", reloadPath.string(),
                           (void*)libHandle);

    if (libHandle == nullptr) {
        logFile << fmt::format("[+] failed to load library with err={}\n", GetLastError());
        return;
    }

    // Old library is freed after the new library loads successfully
    if (prevLibHandle != nullptr) FreeLibraryAndExitThread(prevLibHandle, 0);

    prevLibHandle = libHandle;

    // remove the old library after freeing
    while (fs::exists(tempFilePath)) {
        fs::remove(tempFilePath.parent_path());
        Sleep(500);
    }

    tempFilePath = std::move(newTempFilePath);  // replace
}

void worker() {
    while (true) {
        _worker();
        logFile.flush();
        Sleep(3000);
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            std::thread(worker).detach();
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            if (lpvReserved != nullptr) break;
            break;
    }

    return true;
}