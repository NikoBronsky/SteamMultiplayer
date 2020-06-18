// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/InGameMenu.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UInGameMenu::ResumeGame);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitGame);
	}

	return true;
}

void UInGameMenu::QuitGame()
{
	if (MenuInterface != nullptr)
	{
		TearDown();
		MenuInterface->LoadMainMenu();
	}
}

void UInGameMenu::ResumeGame()
{
	TearDown();
}
