// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FaceDirection.h"
#include "AnimationState.h"
#include "HumanoidGraphicsComponent.h"
#include "Humanoid.generated.h"

UCLASS()
class ZOMBIESIEGE_API AHumanoid : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	EFaceDirection facingDirection = EFaceDirection::Down;

	UPROPERTY(VisibleAnywhere)
	EAnimationState currentAnimation = EAnimationState::None;

public:
	AHumanoid();

protected:

	UPROPERTY(Transient)
	UHumanoidGraphicsComponent* graphicsComponent;
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetAnimationState(EAnimationState animation)
	{
		currentAnimation = animation;
	}

public:	
	static EFaceDirection GetDirectionFromVelocity(const FVector& velocity);

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetFacingDirection(EFaceDirection direction)
	{
		facingDirection = direction;
	}

	UFUNCTION(BlueprintCallable)
	EFaceDirection GetFacingDirection()
	{
		return facingDirection;
	}

	UFUNCTION(BlueprintCallable)
	EAnimationState GetAnimationState()
	{
		return currentAnimation;
	}
};
