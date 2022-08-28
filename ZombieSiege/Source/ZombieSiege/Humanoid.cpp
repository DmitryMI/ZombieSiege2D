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
	if (velocity.IsNearlyZero())
	{
		return EFaceDirection::NONE;
	}

	EFaceDirection directionFlags = EFaceDirection::NONE;

	if (FMath::Abs(velocity.X) > FMath::Abs(velocity.Y))
	{
		if (velocity.X > 0)
		{
			directionFlags = EFaceDirection::Right;
		}
		else
		{
			directionFlags = EFaceDirection::Left;
		}
	}
	else
	{
		if (velocity.Y > 0)
		{
			directionFlags = EFaceDirection::Down;
		}
		else
		{
			directionFlags = EFaceDirection::Up;
		}
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

		currentAnimation = EAnimationState::Moving;
	}
	else
	{
		currentAnimation = EAnimationState::None;
	}
}


