#include "utils.h"

DWORD utils::process::findProcessIdByName(const std::string& processName) 
{
    DWORD processId = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hSnapshot, &pe32)) {
            do {
                if (std::string(pe32.szExeFile) == processName) {
                    processId = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }
        CloseHandle(hSnapshot);
    }
    return processId;
}


DWORD_PTR utils::process::getModuleAddress(const DWORD processID, const char* moduleName)
{
    DWORD_PTR dwModuleBaseAddress = 0;
    DWORD_PTR result = 0;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
    if (hSnapshot != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 ModuleEntry32;
        ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
        if (Module32First(hSnapshot, &ModuleEntry32))
        {
            do {
                if (_stricmp(ModuleEntry32.szModule, moduleName) == 0)
                {
                    dwModuleBaseAddress = reinterpret_cast<DWORD_PTR>(ModuleEntry32.modBaseAddr);
                    result = dwModuleBaseAddress;
                    break;
                }
            } while (Module32Next(hSnapshot, &ModuleEntry32));
        }
        CloseHandle(hSnapshot);
    }
    return result;
}

double utils::math::distance(
    const int x1,
    const int y1,
    const int x2,
    const int y2) 
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

bool utils::mouse::left_click()
{
    if (settings::arduino::enable)
    {
        char buffer[] = "0:0;1";
        Arduino::arduino.send(buffer, sizeof(buffer));
        return true;
    }
    else 
    {
        INPUT    Input = { 0 };
        // left down 
        Input.type = INPUT_MOUSE;
        Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        ::SendInput(1, &Input, sizeof(INPUT));

        // left up
        ::ZeroMemory(&Input, sizeof(INPUT));
        Input.type = INPUT_MOUSE;
        Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        ::SendInput(1, &Input, sizeof(INPUT));
        return true;
    }
}

bool utils::text::stob(const std::string& str)
{
    if (str == "true" || str == "1") 
    {
        return true;
    }
    else if (str == "false" || str == "0") 
    {
        return false;
    }
}

int utils::text::stovk(const std::string& str)
{
    for (size_t i = 0; i < sizeof(KeyNames) / sizeof(KeyNames[0]); ++i) 
    {
        if (str == KeyNames[i]) 
        {
            return KeyCodes[i];
        }
    }
    return -1;
}

std::string utils::text::utos(const std::string& str)
{
    std::string result = str;
    for (char& c : result) 
    {
        if (c == '_') {
            c = ' ';
        }
    }
    return result;
}

int utils::text::stobn(const std::string& boneName)
{
    static const
    std::unordered_map<std::string, int>
    boneMap = 
    {
        {"HEAD", HEAD},
        {"NECK", NECK},
        {"PELVIS", PELVIS},
        {"ANKLE_R", ANKLE_R},
        {"ANKLE_L", ANKLE_L},
        {"LEG_UPPER_L", LEG_UPPER_L},
        {"LEG_UPPER_R", LEG_UPPER_R},
        {"ARM_UPPER_R", ARM_UPPER_R},
        {"ARM_LOWER_R", ARM_LOWER_R},
        {"ARM_UPPER_L", ARM_UPPER_L},
        {"ARM_LOWER_L", ARM_LOWER_L},
        {"HAND_R", HAND_R},
        {"HAND_L", HAND_L}
    };

    auto it = boneMap.find(boneName);
    if (it != boneMap.end()) 
    {
        return it->second;
    }
    else 
    {
        return -1;
    }
}

void utils::audio::beep() 
{
    PlaySound(TEXT("beep.wav"), NULL, SND_FILENAME);
}

void utils::mouse::aim_at(float x, float y)
{
    float TargetX = 0;
    float TargetY = 0;

    POINT cursorPos;
    GetCursorPos(&cursorPos);

    int ScreenCenterX = cursorPos.x;
    int ScreenCenterY = cursorPos.y;

    if (settings::aimbot::smooth == 0) settings::aimbot::smooth = 1;

    if (x != 0)
    {
        if (x > ScreenCenterX)
        {
            TargetX = (x - ScreenCenterX);
            if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
        }

        if (x < ScreenCenterX)
        {
            TargetX = -(ScreenCenterX - x);
            if (TargetX + ScreenCenterX < 0) TargetX = 0;
        }
    }

    if (y != 0)
    {
        if (y > ScreenCenterY)
        {
            TargetY = (y - ScreenCenterY);
            if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
        }

        if (y < ScreenCenterY)
        {
            TargetY = -(ScreenCenterY - y);
            if (TargetY + ScreenCenterY < 0) TargetY = 0;
        }
    }

    TargetX /= settings::aimbot::smooth;
    TargetY /= settings::aimbot::smooth;
    if (abs(TargetX) < 1)
    {
        if (TargetX > 0)
        {
            TargetX = 1;
        }
        if (TargetX < 0)
        {
            TargetX = -1;
        }
    }
    if (abs(TargetY) < 1)
    {
        if (TargetY > 0)
        {
            TargetY = 1;
        }
        if (TargetY < 0)
        {
            TargetY = -1;
        }
    }
    if (settings::arduino::enable) 
    {
        char buffer[20];
        sprintf_s(buffer, "%d:%d;0", (int)TargetX, (int)TargetY);
        Arduino::arduino.send(buffer, sizeof(buffer));
        return;
    }
    mouse_event(0x0001, (UINT)(TargetX), (UINT)(TargetY), NULL, NULL);
    return;
}

void utils::output::printl()
{
    printf("  ___      _                   \n");
    printf(" / __|__ _| |_  _ _ __ ___ ___ \n");
    printf("| (__/ _` | | || | '_ (_-</ _ \\\n");
    printf(" \\___\\__,_|_|\\_, | .__/__/\\___/\n");
    printf("             |__/|_|           \n\n");
    return;


}

void utils::output::printc(const char* prefix, const std::string& text, WORD color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, color);
    printf("%s ", prefix);

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

    printf("%s\n", text.c_str());
}

namespace fs = std::filesystem;

std::string fetch_raw_data(const std::string& url) {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[4096];
    std::string response_data;

    hInternet = InternetOpen("Updater", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "[-] Error initializing Updater." << std::endl;
        return "";
    }

    hConnect = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "[-] Error opening URL: " << url << std::endl;
        InternetCloseHandle(hInternet);
        return "";
    }

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response_data.append(buffer, bytesRead);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return response_data;
}

bool save_to_file(const std::string& data, const std::string& file_path) {
    try {
        fs::create_directories(fs::path(file_path).parent_path());
        std::ofstream file(file_path, std::ios::binary);

        if (file.is_open()) {
            file.write(data.c_str(), data.size());
            file.close();
            return true;
        }
        else {
            Log("[-]", "Error opening file for writing: " + file_path, RED);
            return false;
        }
    }
    catch (const std::exception& e) {
        Log("[-]", "Error saving offsets to" + file_path + ": " + e.what(), RED);
        return false;
    }
}

bool utils::updater::updateOffsets()
{
    const std::string offsets_url = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.json";
    const std::string client_url = "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.json";
    const std::string offsets_path = "./offsets/offsets.json";
    const std::string client_path = "./offsets/client.dll.json";

    std::string offsets_data = fetch_raw_data(offsets_url);
    std::string client_data = fetch_raw_data(client_url);

    if (!offsets_data.empty() && !client_data.empty()) {
        if (save_to_file(offsets_data, offsets_path) && save_to_file(client_data, client_path))
        {
            Log("[+]", "Offsets updated successfully", GREEN);
            Sleep(2500);
            return true;
        }
        else {
            Sleep(3000);
            return false;
        }
    }
    else {
        Log("[-]", "Error while saving offsets", RED);
        Sleep(3000);
        return false;
    }
}