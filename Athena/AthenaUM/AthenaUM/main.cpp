#include "main.h"

std::wstring getCurrentDriveLetterW() {
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring fullPath(buffer);
    return fullPath.substr(0, 1);
}

std::string getCurrentDriveLetter() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string fullPath(buffer);
    return fullPath.substr(0, 1);
}

void findUsbBootGuid(const std::wstring& driveLetter) {
    std::wstring device = L"partition=" + driveLetter + L":";

    std::wstring command = L"bcdedit /enum FIRMWARE";
    FILE* pipe = _wpopen(command.c_str(), L"r");
    if (!pipe) {
        std::wcerr << L"Failed to run bcdedit command." << std::endl;
        return;
    }

    wchar_t buffer[256];
    std::wstring line;
    std::wstring id;
    std::wstring fId;

    while (fgetws(buffer, sizeof(buffer), pipe)) {
        line = buffer;
        std::wstringstream ss(line);
        std::wstring token;
        ss >> token;

        if (token == L"identifier") {
            ss >> id;
        }
        else if (line.find(device) != std::wstring::npos) {
            fId = id;
        }
    }

    _pclose(pipe);

    if (!fId.empty()) {
        std::wstringstream setCommand;
        setCommand << L"bcdedit /set {fwbootmgr} bootsequence " << fId;
        _wsystem(setCommand.str().c_str());
        system("shutdown /r /f /t 0");

    }
    else {
        std::wcerr << L"No boot entry found for the USB device." << std::endl;
    }
}

bool DownloadFile(const std::string& url, const std::string& savePath) {
    HINTERNET hInternet = NULL;
    HINTERNET hConnect = NULL;
    DWORD bytesRead;
    char buffer[4096];

    hInternet = InternetOpen("FileDownloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::wcerr << L"[!] InternetOpen failed: " << GetLastError() << std::endl;
        return false;
    }

    hConnect = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::wcerr << L"[!] InternetOpenUrl failed: " << GetLastError() << std::endl;
        InternetCloseHandle(hInternet);
        return false;
    }

    std::ofstream outFile(savePath, std::ios::binary);
    if (!outFile.is_open()) {
        Log("[!]", "Failed to open output file : " + savePath, RED);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        outFile.write(buffer, bytesRead);
    }
    Log("[+]", "File downloaded successfully to: " + savePath, GREEN);

    outFile.close();
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return true;
}

void launchSteamGame(int appID) {
    std::string steam_path = "";
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Valve\\Steam", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char value[1024];
        DWORD size = sizeof(value);
        if (RegQueryValueEx(hKey, "SteamExe", NULL, NULL, (LPBYTE)value, &size) == ERROR_SUCCESS) {
            steam_path = value;
        }
        RegCloseKey(hKey);
    }

    std::string command = "\"" + steam_path + "\" steam://run/" + std::to_string(appID);
    system(command.c_str());
    return;
}

bool init() 
{
    SetConsoleTitle("CS2-Athena");
    Logo();

	Sleep(1000);

	if (!driver::init() || !driver::checkDriverStatus())
	{
        Log("[!]", "Driver not found, installing driver...", YELLOW);
        std::string beep_file_path = getCurrentDriveLetter() + ":\\beep.wav";
        std::string startup_file_path = getCurrentDriveLetter() + ":\\Startup.nsh";
        std::string bootx64_file_path = getCurrentDriveLetter() + ":\\EFI\\Boot\\bootx64.efi";
        std::string athenaefi_file_path = getCurrentDriveLetter() + ":\\AthenaEFI.efi";

        std::string efi_path = getCurrentDriveLetter() + ":\\EFI";
        std::string efi_boot_path = getCurrentDriveLetter() + ":\\EFI\\Boot";

        if (!CreateDirectory(efi_path.c_str(), NULL) || !CreateDirectory(efi_boot_path.c_str(), NULL))
        {
            Log("[-]", "Failed to create" + getCurrentDriveLetter() + ":/EFI/Boot folder", RED);
			Log("[-]", "Please create the folders manually then press enter to continue.", RED);
            system("pause");
        }


        if (!DownloadFile("https://files.catbox.moe/ghjy48.wav", beep_file_path) || !DownloadFile("https://files.catbox.moe/nf62n0.nsh", startup_file_path) || !DownloadFile("https://files.catbox.moe/bmy0u2.efi", bootx64_file_path) || !DownloadFile("https://files.catbox.moe/12oqma.efi", athenaefi_file_path))
        {
            Log("[-]", "Failed to download driver files", RED);
            system("pause");
            exit(1);
        }
        else
		{
			Log("[+]", "Driver files downloaded successfully", GREEN);
		}

        Log("[!]", "Your computer will restart automatically, please wait...", YELLOW);

		Sleep(3000);

        UNICODE_STRING VariableName = RTL_CONSTANT_STRING(VARIABLE_NAME);
        NtSetSystemEnvironmentValueEx(
            &VariableName,
            &DummyGuid,
            0,
            0,
            ATTRIBUTES);

        std::wstring usbDriveLetter = getCurrentDriveLetterW();
        findUsbBootGuid(usbDriveLetter);
	}

    if (!readOffsets())
    {
        exit(1);
    }
    else
    {
        Log("[+]", "Offsets found", GREEN);
    }

    Config::init();

    if (settings::arduino::enable)
        Arduino::arduino.init();

    if (settings::soundesp::enable) 
    {
        std::filesystem::path filePath("beep.wav");

        if (!std::filesystem::exists(filePath)) 
        {
            Log("[-]", "beep.wav file for SoundEsp not found", RED);
            std::string beep_file_path = getCurrentDriveLetter() + ":\\beep.wav";
            if (!DownloadFile("https://files.catbox.moe/3wwyh6.wav", beep_file_path))
			{
				Log("[-]", "Failed to download beep.wav file", RED);
				system("pause");
				exit(1);
			}
			else
			{
				Log("[+]", "beep.wav file downloaded successfully", GREEN);
			}
        }
    }

    shared::game::pid = utils::process::findProcessIdByName("cs2.exe");
    if (!shared::game::pid || shared::game::pid == 1)
    {
        int appID = 730;
        launchSteamGame(appID);

		Log("[!]", "Waiting for CS2 to start...", YELLOW);
        Sleep(3000);
		Log("[-]", "Press any key once in game to continue.", YELLOW);
		system("pause > nul");
    }
    else
    {
        Log("[+]", "CS2 found", GREEN);
    }

    while (!shared::game::pid)
    {
        shared::game::pid = utils::process::findProcessIdByName("cs2.exe");
        Sleep(1000);
    }

    shared::game::client = utils::process::getModuleAddress(shared::game::pid, "client.dll");
    if (!shared::game::client || shared::game::client == 1)
    {
        Log("[-]", "client.dll address not found", RED);
        system("pause");
        exit(1);
    }
    else
    {
        std::ostringstream oss;
        oss << std::hex << shared::game::client;
        Log("[+]", "client.dll address found -> 0x" + oss.str(), GREEN);
    }

    shared::game::engine = utils::process::getModuleAddress(shared::game::pid, "engine2.dll");
    if (!shared::game::engine || shared::game::engine == 1)
    {
        Log("[-]", "engine2.dll address not found", RED);
        system("pause");
        exit(1);
    }
    else
    {
        std::ostringstream oss;
        oss << std::hex << shared::game::engine;
        Log("[+]", "engine.dll address found -> 0x" + oss.str(), GREEN);
    }

    if (entity_loop::start())
    {
        Log("[+]", "Entity loop thread started", GREEN);
    }
    else 
    {
        Log("[-]", "Failed to start entity loop thread", RED);
    }

    if (!features::start()) 
    {
        Log("[-]", "All cheat features are turned off or problem with config file.", RED);
        system("pause");
        return false;
    }

    return true;
}

bool IsInternetConnected() {
    int result = system("ping -n 1 8.8.8.8 > nul");
    return result == 0;
}

int main()
{
	if (!IsInternetConnected()) {
		Log("[-]", "No internet connection found, please connect to the internet.", RED);
	}
    while (!IsInternetConnected()) {
        Sleep(1000);
    }

    system("cls");

    if (init()) 
    {
        for (;;) 
        {
            shared::game::screen_size.x = GetSystemMetrics(SM_CXSCREEN);
            shared::game::screen_size.y = GetSystemMetrics(SM_CYSCREEN);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}