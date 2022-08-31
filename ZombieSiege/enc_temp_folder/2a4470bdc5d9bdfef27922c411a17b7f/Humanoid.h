// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "FaceDirection.h"
#include "HumanoidState.h"
#include "HumanoidGraphicsComponent.h"
#include "WeaponInfoBase.h"
#include "WeaponManager.h"
#include "Humanoid.generated.h"

UCLASS()
class ZOMBIESIEGE_API AHumanoid : public AUnitBase
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	EFaceDirection facingDirection = EFaceDirection::Down;

	UPROPERTY(VisibleAnywhere)
	EHumanoidState currentState = EHumanoidState::None;

	UPROPERTY(VisibleAnywhere, Transient)
	AWeaponManager* weaponManager;

public:
	AHumanoid();

protected:

	UPROPERTY(Transient)
	UHumanoidGraphicsComponent* graphicsComponent;

	UPROPERTY(BlueprintReadOnly)
	UWeaponInfoBase* weaponInfo;
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetHumanoidState(EHumanoidState humanoidState)
	{
		currentState = humanoidState;
	}

public:	

	UFUNCTION(BlueprintCallable)
	UWeaponInfoBase* GetWeaponInfo() 
	{ 
		return weaponInfo;
	}

	UFUNCTION(BlueprintCallable)
	void SetWeaponInfo(UWeaponInfoBase* weapon)
	{ 
		weaponInfo = weapon;
	}

	UFUNCTION(BlueprintCallable)
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
	EHumanoidState GetHumanoidState()
	{
		return currentState;
	}	
};
