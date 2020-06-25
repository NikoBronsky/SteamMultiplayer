// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "UObject/ConstructorHelpers.h"
#include "MenuSystem/ServerRow.h"
#include "Components/TextBlock.h"

UMainMenu::UMainMenu()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/PuzzlePlatforms/UI/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}


void UMainMenu::SetServerList(TArray<FString> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (World != nullptr)
	{
		ServerList->ClearChildren();

		for (const FString& ServerName : ServerNames)
		{

			UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
			if (Row != nullptr)
			{
				Row->ServerName->SetText(FText::FromString(ServerName));
				ServerList->AddChild(Row);
			}
		}
	}
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
	}

	if (CancelJoin)
	{
		CancelJoin->OnClicked.AddDynamic(this, &UMainMenu::BackToMainMenu);
	}

	if (ApplyIP)
	{
		ApplyIP->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UMainMenu::Quit);
	}

	return true;
}

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		MenuInterface->Host();
	}
}

void UMainMenu::OpenJoinMenu()
{
	if (JoinMenu != nullptr && MenuSwitcher != nullptr)
	{
		MenuSwitcher->SetActiveWidget(JoinMenu);
	}
	
}

void UMainMenu::BackToMainMenu()
{
	if (MainMenu != nullptr)
	{
		MenuSwitcher->SetActiveWidget(MainMenu);
	}
}

void UMainMenu::Quit()
{
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController != nullptr)
		{
			PlayerController->ConsoleCommand("quit");
		}
	}
}

void UMainMenu::JoinServer()
{
	if (MenuInterface != nullptr /*&& IPAdressField != nullptr*/)
	{
/*		FString Address = IPAdressField->GetText().ToString();*/
		MenuInterface->Join("");
	}
	
}
