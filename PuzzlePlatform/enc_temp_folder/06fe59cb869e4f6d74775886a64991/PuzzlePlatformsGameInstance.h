// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "OnlineSubsystem.h"
#include "PuzzlePlatformsGameInstance.generated.h"

class UUserWidget;
class FOnlineSessionSearch;
/**
 * 
 */
UCLASS()
class PUZZLEPLATFORM_API UPuzzlePlatformsGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
	UPuzzlePlatformsGameInstance();

	virtual void Init() override;

	UFUNCTION(Exec, BlueprintCallable)
	void LoadMenuWidget();

	UFUNCTION(Exec, BlueprintCallable)
	void InGameLoadMenu();

	UFUNCTION(Exec)
	void Host() override;

	UFUNCTION(Exec)
	void Join(const FString& Address) override;

	UFUNCTION(Exec)
	void LoadMainMenu() override;

private:

	TSubclassOf<UUserWidget> MenuClass;

	TSubclassOf<UUserWidget> InGameMenuClass;	

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);

	void CreateSession();
};
