#include "stdafx.h"
#include "InjectorWindow.h"
#include "Injector.h"

int GUIApplication(HINSTANCE hInstance, int nCmdShow)
{
	InjectorWindow* injectorWindow = new InjectorWindow();
	if (!injectorWindow->Create(hInstance, nCmdShow))
	{
		MessageBox(NULL, L"Failed to create application interface", L"Error", MB_ICONERROR);

		delete injectorWindow;
		return FALSE;
	}

	injectorWindow->Initialize();

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		injectorWindow->Update();
	}

	delete injectorWindow;

	return (int)msg.wParam;
}

bool QuickInject(LPCWSTR executablePath, LPCWSTR libraryPath)
{
#ifdef _DEBUG
	MessageBox(NULL, executablePath, L"EXE", MB_ICONINFORMATION);
	MessageBox(NULL, libraryPath, L"DLL", MB_ICONINFORMATION);
#endif

	Injector* injector = new Injector();
	injector->SetExecutablePath(executablePath);
	injector->SetLibraryPath(libraryPath);
	if (injector->Inject() != Injector::INJECTION_OK)
	{
		delete injector;
		return false;
	}

	injector->Wait();
	delete injector;

	return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	int executionResult = 0;	
	int commandCount = 0;
	LPWSTR* commandList = CommandLineToArgvW(GetCommandLineW(), &commandCount);

	if (commandCount == 3)
	{
		WCHAR* executablePath = new WCHAR[_MAX_PATH];
		WCHAR* libraryPath = new WCHAR[_MAX_PATH];

		for (int i = 0; i < commandCount; ++i)
		{
			if (i == 1)
				wcscpy_s(executablePath, wcslen(commandList[i]) + 1, commandList[i]);
			else if(i == 2)
				wcscpy_s(libraryPath, wcslen(commandList[i]) + 1, commandList[i]);
		}

		executionResult = QuickInject(executablePath, libraryPath) ? 0 : -1;
		if (executionResult != 0)
			MessageBox(NULL, L"DLL injection has failed!", L"Error", MB_ICONERROR);

		delete[] executablePath;
		delete[] libraryPath;
	}
	else
	{
		executionResult = GUIApplication(hInstance, nCmdShow);
	}

	LocalFree(commandList);

	return executionResult;
}