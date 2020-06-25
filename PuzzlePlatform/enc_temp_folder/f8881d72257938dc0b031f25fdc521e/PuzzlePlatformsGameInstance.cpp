// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "PlatformTrigger.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

const static FName SESSION_NAME = TEXT("My Session Game");

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance()
{
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Constructor"));

	static ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/PuzzlePlatforms/UI/MenuSystem/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;
	MenuClass = MenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/PuzzlePlatforms/UI/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;
	InGameMenuClass = InGameMenuBPClass.Class;

}


void UPuzzlePlatformsGameInstance::Init()
{
	Super::Init();
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Sybsystem %s"), *Subsystem->GetSubsystemName().ToString());
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionsComplete);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
	}
}

void UPuzzlePlatformsGameInstance::LoadMenuWidget()
{
	if (MenuClass != nullptr)
	{
		Menu = CreateWidget<UMainMenu>(this, MenuClass);
		if (Menu != nullptr)
		{
			Menu->Setup();
			Menu->SetMenuInterface(this);
		}
	}
}

void UPuzzlePlatformsGameInstance::InGameLoadMenu()
{
	if (InGameMenuClass != nullptr)
	{
		UMenuWidget* IngameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);

		IngameMenu->Setup();

		IngameMenu->SetMenuInterface(this);
	}
}

void UPuzzlePlatformsGameInstance::Host()
{
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
}
void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return;
	}
	UEngine* Engine = GetEngine();
	if (Engine)
	{
		Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

		UWorld* World = GetWorld();
		if (World == nullptr)
		{
			return;
		}

		World->ServerTravel("/Game/PuzzlePlatforms/Maps/Puzzle_P?listen");
	}
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
	}
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid() && Menu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));

		TArray<FString> ServerNames;

		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session names: %s"), *SearchResult.GetSessionIdStr());
			ServerNames.Add(SearchResult.GetSessionIdStr());
		}
		Menu->SetServerList(ServerNames);
	}
}

void UPuzzlePlatformsGameInstance::CreateSession()
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bShouldAdvertise = true;

	SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UPuzzlePlatformsGameInstance::Join(const FString& Address)
{
	if (Menu != nullptr)
	{
		Menu->SetServerList({ "Test1","Test2" });
	}
// 	UEngine* Engine = GetEngine();
// 	if (Engine)
// 	{
// 		Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
// 
// 		APlayerController* PlayerController = GetFirstLocalPlayerController();
// 		if (PlayerController)
// 		{
// 			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
// 		}
// 	}
}

void UPuzzlePlatformsGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true;
		UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}


void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel("/Game/PuzzlePlatforms/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
	}
}

