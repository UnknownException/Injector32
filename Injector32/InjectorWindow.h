#pragma once
#include "GUIComponents/Window.h"
#include "GUIComponents/TextBox.h"
#include "GUIComponents/TextLabel.h"
#include "GUIComponents/Button.h"
#include "Injector.h"

class InjectorWindow : public Window {
	Font* fntDefault;
	Font* fntBold;

	TextLabel* tlExecutableLabel;
	TextLabel* tlExecutablePath;
	Button* btnExecutableOpen;

	TextLabel* tlLibraryLabel;
	TextLabel* tlLibraryPath;
	Button* btnLibraryOpen;

	Button* btnInject;

	Injector* injector;
public:
	InjectorWindow();
	virtual ~InjectorWindow();

	bool Initialize() override;
	bool Update() override;

protected:
	LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};