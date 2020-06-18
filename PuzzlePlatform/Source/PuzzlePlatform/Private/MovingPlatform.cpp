// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsForward = true;
	InterpSpeed = 100;

	SetMobility(EComponentMobility::Movable);

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}


}

void AMovingPlatform::AddActiveTrigger()
{
	ActiveTriggers++;
}

void AMovingPlatform::RemoveActiveTrigger()
{
	if (ActiveTriggers > 0)
	{
		ActiveTriggers--;
	}
}

void AMovingPlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ActiveTriggers > 0)
	{
		if (HasAuthority())
		{
			if (IsAtTarget())
			{
				SwitchDirection();
			}
			MovePlatformToTarget(DeltaSeconds);
		}
	}
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	EndLocation = GetTransform().TransformPosition(EndLocation);
}

void AMovingPlatform::MovePlatformToTarget(float DeltaTime)
{
	FVector LocationToSet;
	FVector Destination;

	if (bIsForward)
	{
		Destination = EndLocation;
	}
	else
	{
		Destination = StartLocation;
	}	

	LocationToSet = FMath::VInterpConstantTo(GetActorLocation(), Destination, DeltaTime, InterpSpeed);	

	SetActorLocation(LocationToSet);
}

void AMovingPlatform::SwitchDirection()
{
	bIsForward = !bIsForward;	
}

bool AMovingPlatform::IsAtTarget()
{
	FVector CompareWith;	

	if (bIsForward)
	{
		CompareWith = EndLocation;
	}
	else
	{
		CompareWith = StartLocation;
	}

	float DistanceTo = FMath::Abs((CompareWith.Size()) - GetActorLocation().Size());

	if (DistanceTo <= 0)
	{
		/*UE_LOG(LogTemp, Warning, TEXT("Distance is %f"), DistanceTo);*/

		return true;		
	}
	return false;
}

