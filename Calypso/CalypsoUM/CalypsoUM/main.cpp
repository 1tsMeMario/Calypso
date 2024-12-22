#include "main.h"

std::wstring getCurrentDriveLetterW() {
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring fullPath(buffer);
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

bool init() 
{
    SetConsoleTitle("CS2-Calypso");
    Logo();

	Sleep(1000);

	if (!driver::init() || !driver::checkDriverStatus())
	{
        Log("[!]", "Driver not found, installing driver...", YELLOW);
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
            system("pause");
            exit(1);
        }
    }

    shared::game::pid = utils::process::findProcessIdByName("cs2.exe");
    if (!shared::game::pid || shared::game::pid == 1)
    {
        Log("[-]", "CS2 not found", RED);
        system("pause");
        exit(1);
    }
    else
    {
        Log("[+]", "CS2 found", GREEN);
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