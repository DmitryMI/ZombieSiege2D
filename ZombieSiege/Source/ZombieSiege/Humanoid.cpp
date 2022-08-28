// Fill out your copyright notice in the Description page of Project Settings.


#include "Humanoid.h"

// Sets default values
AHumanoid::AHumanoid()
{
	PrimaryActorTick.bCanEverTick = true;

	graphicsComponent = CreateDefaultSubobject<UHumanoidGraphicsComponent>(TEXT("GraphicsComponent"), false);
	RootComponent = graphicsComponent;
}

void AHumanoid::BeginPlay()
{
	Super::BeginPlay();
	
}

EFaceDirection AHumanoid::GetDirectionFromVelocity(const FVector& velocity)
{
	EFaceDirection directionFlags = EFaceDirection::NONE;

	if (velocity.X > 0)
	{
		directionFlags |= EFaceDirection::Right;
	}
	else if (velocity.X < 0)
	{
		directionFlags |= EFaceDirection::Left;
	}

	if (velocity.Y > 0)
	{
		directionFlags |= EFaceDirection::Down;
	}
	else if (velocity.Y < 0)
	{
		directionFlags |= EFaceDirection::Up;
	}

	if (velocity.IsNearlyZero())
	{		
		directionFlags = EFaceDirection::NONE;
	}

	return directionFlags;
}

void AHumanoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector velocity = GetVelocity();

	EFaceDirection directionFlags = GetDirectionFromVelocity(velocity);
	
	if (!velocity.IsNearlyZero())
	{
		// We change current direction only if the velocity is not Zero
		// This way the Humanoid will remains their last direction after they stop moving
		facingDirection = directionFlags;
	}
}


