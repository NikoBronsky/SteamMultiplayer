// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSystem/MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "UObject/ConstructorHelpers.h"
#include "MenuSystem/ServerRow.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"

UMainMenu::UMainMenu()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/PuzzlePlatforms/UI/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}


void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (World != nullptr)
	{
		ServerList->ClearChildren();

		uint32 i = 0;
		for (const FServerData& ServerData : ServerNames)
		{

			UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
			if (Row != nullptr)
			{
				Row->ServerName->SetText(FText::FromString(ServerData.Name));
				Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
				FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
				Row->ConnectionFraction->SetText(FText::FromString(FractionText));
				Row->Setup(this, i);
				++i;
				ServerList->AddChild(Row);
			}
		}
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UServerRow> (ServerList->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->bIsSelected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}
	}
}

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);
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

	if (ApplyHostName)
	{
		ApplyHostName->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
	}

	if (CancelHosting)
	{
		ApplyHostName->OnClicked.AddDynamic(this, &UMainMenu::BackToMainMenu);
	}

	return true;
}

void UMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		FString ServerName = ServerHostName->GetText().ToString();
		MenuInterface->Host(ServerName);
	}
}

void UMainMenu::OpenJoinMenu()
{
	if (JoinMenu != nullptr && MenuSwitcher != nullptr)
	{
		MenuSwitcher->SetActiveWidget(JoinMenu);
		if (MenuInterface != nullptr)
		{
			MenuInterface->RefreshServerList();
		}
	}
	
}

void UMainMenu::OpenHostMenu()
{
	{
		MenuSwitcher->SetActiveWidget(HostMenu);
		if (MenuInterface != nullptr)
		{
			MenuInterface->RefreshServerList();
		}
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
	if (SelectedIndex.IsSet() && MenuInterface != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index %d."), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set."));
	}
}
