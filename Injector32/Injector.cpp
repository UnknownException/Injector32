#include "stdafx.h"
#include "Injector.h"

Injector::Injector()
{
	executablePath = nullptr;
	libraryPath = nullptr;

	ZeroMemory(&processInformation, sizeof(PROCESS_INFORMATION));
}

Injector::~Injector()
{
	if (executablePath)
		delete[] executablePath;

	if (libraryPath)
		delete[] libraryPath;

	if (processInformation.hThread)
		CloseHandle(processInformation.hThread);

	if (processInformation.hProcess)
		CloseHandle(processInformation.hProcess);
}


void Injector::SetExecutablePath(LPCWSTR path)
{
	if (!path)
		return;

	if (executablePath)
		delete[] executablePath;

	executablePath = new WCHAR[wcslen(path) + 1];
	wcscpy_s(executablePath, wcslen(path) + 1, path);
}

void Injector::SetLibraryPath(LPCWSTR path)
{
	if (!path)
		return;

	if (libraryPath)
		delete[] libraryPath;

	libraryPath = new WCHAR[wcslen(path) + 1];
	wcscpy_s(libraryPath, wcslen(path) + 1, path);
}

Injector::InjectionResult Injector::Inject()
{
	if (executablePath == nullptr)
		return EXECUTABLEPATH_NOTSET;

	if (libraryPath == nullptr)
		return LIBRARYPATH_NOTSET;
	
	if (processInformation.hProcess != nullptr)
		return PROCESS_ALREADYINJECTED;

	InjectionResult validate = Validate();
	if (validate != INJECTION_OK)
		return validate;

	STARTUPINFO startupInformation;
	ZeroMemory(&startupInformation, sizeof(STARTUPINFO));
	startupInformation.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(executablePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &startupInformation, &processInformation))
		return PROCESS_CREATEPROCESS_FAILED;

	FARPROC loadLibraryProc = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	if (!loadLibraryProc)
		return PROCESS_GETPROCADDRESS_FAILED;

	LPVOID allocatedString = VirtualAllocEx(processInformation.hProcess, NULL, (wcslen(libraryPath) * sizeof(WCHAR)) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!allocatedString)
		return LIBRARYPATH_VIRTUALALLOCEX_FAILED;

	if (!WriteProcessMemory(processInformation.hProcess, allocatedString, libraryPath, (wcslen(libraryPath) * sizeof(WCHAR)) + 1, NULL))
	{
		VirtualFreeEx(processInformation.hProcess, allocatedString, 0, MEM_RELEASE);
		return LIBRARYPATH_WRITEPROCESSMEMORY_FAILED;
	}

	HANDLE dllHandle = CreateRemoteThread(processInformation.hProcess, 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryProc), allocatedString, CREATE_SUSPENDED, NULL);
	if (!dllHandle)
	{
		VirtualFreeEx(processInformation.hProcess, allocatedString, 0, MEM_RELEASE);
		return LIBRARY_CREATEREMOTETHREAD_FAILED;
	}

#ifdef _DEBUG
	MessageBox(NULL, L"Before resuming DLL thread", L"Debug Information", MB_ICONINFORMATION);
#endif

	if (ResumeThread(dllHandle) == -1)
	{
		CloseHandle(dllHandle);
		VirtualFreeEx(processInformation.hProcess, allocatedString, 0, MEM_RELEASE);
		return LIBRARY_RESUMETHREAD_FAILED;
	}

#ifdef _DEBUG
	MessageBox(NULL, L"After resuming DLL thread", L"Debug Information", MB_ICONINFORMATION);
#endif

	if (WaitForSingleObject(dllHandle, INFINITE) == WAIT_FAILED)
	{
		CloseHandle(dllHandle);
		VirtualFreeEx(processInformation.hProcess, allocatedString, 0, MEM_RELEASE);
		return LIBRARY_WAITFORSINGLEOBJECT_FAILED;
	}

	if (dllHandle)
		CloseHandle(dllHandle);

#ifdef _DEBUG
	MessageBox(NULL, L"After DLL handle closure", L"Debug Information", MB_ICONINFORMATION);
#endif

	if (!VirtualFreeEx(processInformation.hProcess, allocatedString, 0, MEM_RELEASE))
		return LIBRARYPATH_VIRTUALFREEEX_FAILED;

	if (ResumeThread(processInformation.hThread) == -1)
		return PROCESS_RESUMETHREAD_FAILED;

	return INJECTION_OK;
}

bool Injector::Wait()
{
	return !(WaitForSingleObject(processInformation.hProcess, INFINITE) == WAIT_FAILED);
}

bool IsValidPE(LPCWSTR filePath)
{
	return true;
}

bool IsValidLibrary(LPCWSTR filePath)
{
	return true;
}

Injector::InjectionResult  Injector::Validate()
{
	if (!IsValidPE(executablePath))
		return EXECUTABLE_INVALID;

	if (!IsValidLibrary(libraryPath))
		return LIBRARY_INVALID;

	return INJECTION_OK;
}