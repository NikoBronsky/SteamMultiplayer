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

const static FName SESSION_NAME = TEXT("My Session Game");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("Server name");

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
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionsComplete);
		}
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

void UPuzzlePlatformsGameInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;
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

	if (Menu != nullptr)
	{
		Menu->TearDown();
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

		World->ServerTravel("/Game/PuzzlePlatforms/Maps/Lobby?listen");
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

		TArray<FServerData> ServerNames;

		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session names: %s"), *SearchResult.GetSessionIdStr());
			FServerData Data;
			Data.Name = SearchResult.GetSessionIdStr();
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			Data.HostUsername = SearchResult.Session.OwningUserName;
			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				Data.Name = ServerName;
			}
			else
			{
				Data.Name = "Could not find name.";
			}
			
			ServerNames.Add(Data);
		}
		Menu->SetServerList(ServerNames);
	}
}

void UPuzzlePlatformsGameInstance::OnJoinSessionsComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("INTERFACE PROBLEM"));
		return;
	}
		

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
		return;
	}

	UEngine* Engine = GetEngine();
 	if (Engine)
 	{
 		Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));
 
 		APlayerController* PlayerController = GetFirstLocalPlayerController();
 		if (PlayerController)
 		{
 			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UPuzzlePlatformsGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			SessionSettings.bIsLANMatch = false;
		}

		
		SessionSettings.NumPublicConnections = 2;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	if (Menu != nullptr)
	{
		Menu->TearDown();
	}

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UPuzzlePlatformsGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
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

