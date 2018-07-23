#include "stdafx.h"
#include "InjectorWindow.h"
#include "commdlg.h"

InjectorWindow::InjectorWindow()
{
	SetClassname(L"InjectorWindow");

#ifdef _WIN64
	#ifdef _DEBUG
		SetTitle(L"Injector32 (64-bit debug)");
	#else
		SetTitle(L"Injector32 (64-bit)");
	#endif
#else
	#ifdef _DEBUG
		SetTitle(L"Injector32 (32-bit debug)");
	#else
		SetTitle(L"Injector32 (32-bit)");
	#endif
#endif

	fntDefault = nullptr;
	fntBold = nullptr;

	tlExecutableLabel = nullptr;
	tlExecutablePath = nullptr;
	btnExecutableOpen = nullptr;

	tlLibraryLabel = nullptr;
	tlLibraryPath = nullptr;
	btnLibraryOpen = nullptr;

	btnInject = nullptr;

	SetPosition(Vector2<int>(CW_USEDEFAULT, 0));
	SetSize(Vector2<int>(512, 164));
	SetResizable(false);
	SetMaximizable(false);

	injector = nullptr;
}

InjectorWindow::~InjectorWindow()
{
	if (fntDefault)
		delete fntDefault;

	if (fntBold)
		delete fntBold;

	if (tlExecutableLabel)
		delete tlExecutableLabel;

	if (tlExecutablePath)
		delete tlExecutablePath;

	if (btnExecutableOpen)
		delete btnExecutableOpen;

	if (tlLibraryLabel)
		delete tlLibraryLabel;

	if (tlLibraryPath)
		delete tlLibraryPath;

	if (btnLibraryOpen)
		delete btnLibraryOpen;

	if (btnInject)
		delete btnInject;

	if (injector)
		delete injector;
}

bool InjectorWindow::Initialize()
{
	if (!Window::Initialize())
		return false;

	fntDefault = new Font();
	fntDefault->SetName(L"Arial");
	fntDefault->SetSize(9);
	fntDefault->SetBold(false);

	fntBold = new Font();
	fntBold->SetName(L"Arial");
	fntBold->SetSize(9);
	fntBold->SetBold(true);

	tlExecutableLabel = new TextLabel();
	tlExecutableLabel->SetParent(GetSelf());
	tlExecutableLabel->SetPosition(Vector2<int>(6, 10));
	tlExecutableLabel->SetSize(Vector2<int>(160, 16));
	tlExecutableLabel->SetText(L"Executable Path");
	tlExecutableLabel->SetFont(fntBold);
	tlExecutableLabel->Create();

	tlExecutablePath = new TextLabel();
	tlExecutablePath->SetParent(GetSelf());
	tlExecutablePath->SetPosition(Vector2<int>(10, 28));
	tlExecutablePath->SetSize(Vector2<int>(476, 20));
	tlExecutablePath->SetText(L"Select an executable");
	tlExecutablePath->SetFont(fntDefault);
	tlExecutablePath->SetSunken(true);
	tlExecutablePath->Create();

	btnExecutableOpen = new Button();
	btnExecutableOpen->SetParent(GetSelf());
	btnExecutableOpen->SetPosition(Vector2<int>(406, 8));
	btnExecutableOpen->SetSize(Vector2<int>(80, 20));
	btnExecutableOpen->SetText(L"Open");
	btnExecutableOpen->SetFont(fntDefault);
	btnExecutableOpen->Create();

	tlLibraryLabel = new TextLabel();
	tlLibraryLabel->SetParent(GetSelf());
	tlLibraryLabel->SetPosition(Vector2<int>(6, 58));
	tlLibraryLabel->SetSize(Vector2<int>(160, 16));
	tlLibraryLabel->SetText(L"DLL Path");
	tlLibraryLabel->SetFont(fntBold);
	tlLibraryLabel->Create();

	tlLibraryPath = new TextLabel();
	tlLibraryPath->SetParent(GetSelf());
	tlLibraryPath->SetPosition(Vector2<int>(10, 76));
	tlLibraryPath->SetSize(Vector2<int>(476, 20));
	tlLibraryPath->SetText(L"Select a dynamic link library");
	tlLibraryPath->SetFont(fntDefault);
	tlLibraryPath->SetSunken(true);
	tlLibraryPath->Create();

	btnLibraryOpen = new Button();
	btnLibraryOpen->SetParent(GetSelf());
	btnLibraryOpen->SetPosition(Vector2<int>(406, 56));
	btnLibraryOpen->SetSize(Vector2<int>(80, 20));
	btnLibraryOpen->SetText(L"Open");
	btnLibraryOpen->SetFont(fntDefault);
	btnLibraryOpen->Create();

	btnInject = new Button();
	btnInject->SetParent(GetSelf());
	btnInject->SetPosition(Vector2<int>(406, 104));
	btnInject->SetSize(Vector2<int>(80, 20));
	btnInject->SetText(L"Inject");
	btnInject->SetFont(fntBold);
	btnInject->Create();

	injector = new Injector();

	return true;
}

bool InjectorWindow::Update()
{
	if (!Window::Update())
		return false;

	return true;
}

LRESULT CALLBACK InjectorWindow::Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COMMAND:
		{
			switch (HIWORD(wParam))
			{
				case BN_CLICKED:
				{
					HWND control = (HWND)lParam;
					if (btnExecutableOpen->IsSame(control))
					{
						OPENFILENAME ofn;
						WCHAR szFile[_MAX_PATH];
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = GetSelf();
						ofn.lpstrFile = szFile;
						ofn.lpstrFile[0] = '\0';
						ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);
						ofn.lpstrFilter = L"Executable (.exe)\0*.exe\0All Files\0*.*\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = 0;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

						if (GetOpenFileName(&ofn) == TRUE)
						{
							injector->SetExecutablePath(ofn.lpstrFile);
							tlExecutablePath->SetText(ofn.lpstrFile);
						}
					}
					else if (btnLibraryOpen->IsSame(control))
					{
						OPENFILENAME ofn;
						WCHAR szFile[_MAX_PATH];
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = GetSelf();
						ofn.lpstrFile = szFile;
						ofn.lpstrFile[0] = '\0';
						ofn.nMaxFile = sizeof(szFile) / sizeof(WCHAR);
						ofn.lpstrFilter = L"Dynamic Link Library (.dll)\0*.dll\0All Files\0*.*\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = 0;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

						if (GetOpenFileName(&ofn) == TRUE)
						{
							injector->SetLibraryPath(ofn.lpstrFile);
							tlLibraryPath->SetText(ofn.lpstrFile);
						}
					}
					else if (btnInject->IsSame(control))
					{
						if (injector->Inject() != Injector::INJECTION_OK)
						{
							MessageBox(NULL, L"DLL injection has failed!", L"Error", MB_ICONERROR);
						}
						else
						{
							SetVisible(false);

							injector->Wait();
							delete injector;

							injector = new Injector();
							injector->SetExecutablePath(tlExecutablePath->GetText());
							injector->SetLibraryPath(tlLibraryPath->GetText());

							SetVisible(true);
						}
					}
					else
						break;

					return 0;
				}
				break;
				default:
					break;
			}
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);

			return 0;
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}