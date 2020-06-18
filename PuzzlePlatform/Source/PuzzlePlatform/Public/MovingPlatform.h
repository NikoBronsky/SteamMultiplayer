// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MovingPlatform.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORM_API AMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	AMovingPlatform();

	void AddActiveTrigger();

	void RemoveActiveTrigger();

protected:

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
	

	UPROPERTY(EditAnywhere, Category = "MovingPlatform", meta = (MakeEditWidget = true))
	FVector EndLocation;

	FVector StartLocation;

	FRotator ActorStartRotation;
	
	void MovePlatformToTarget(float DeltaTime);

	void SwitchDirection();
	
	bool IsAtTarget();
	

	// If you use constant interpolation you should pick value greater then 50, if not 1 is good.
	UPROPERTY(EditAnywhere, Category = "MovingPlatform")
	float InterpSpeed;

	bool bIsForward;

private:

	UPROPERTY(EditAnywhere, Category = "MovingPlatform")
	int ActiveTriggers = 1;


};
