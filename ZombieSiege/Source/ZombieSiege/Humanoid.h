// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "FaceDirection.h"
#include "HumanoidState.h"
#include "HumanoidGraphicsComponent.h"
#include "WeaponInfo.h"
#include "WeaponManager.h"
#include "GameFramework/FloatingPawnMovement.h"
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

	UPROPERTY(VisibleAnywhere)
	AWeaponManager* weaponManager;

	UPROPERTY(VisibleAnywhere)
	bool bIsOnCooldown;

	FTimerHandle attackBackswingTimerHandle;
	FTimerDelegate attackBackswingTimerDelegate;	

	FTimerHandle attackRelaxationTimerHandle;
	FTimerDelegate attackRelaxationTimerDelegate;

	FTimerHandle attackCooldownTimerHandle;
	FTimerDelegate attackCooldownTimerDelegate;
	

	UFUNCTION()
	void OnBackswingTimerElapsed(AUnitBase* target);

	UFUNCTION()
	void OnRelaxationTimerElapsed();

	UFUNCTION()
	void OnCooldownTimerElapsed();

public:
	AHumanoid();

	DECLARE_EVENT_TwoParams(AHumanoid, FOnHumanoidStateChangedEvent, EHumanoidState stateOld, EHumanoidState stateNew);
	FOnHumanoidStateChangedEvent& OnHumanoidStateChanged() { return onHumanoidStateChangedEvent; }
private:
	FOnHumanoidStateChangedEvent onHumanoidStateChangedEvent;

protected:

	UPROPERTY(Transient)
	UHumanoidGraphicsComponent* graphicsComponent;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponInfo* weaponInfo;
	
	virtual void BeginPlay() override;

	virtual void SetupDefaultWeapon();

	virtual bool CanCommitAttackTarget(AUnitBase* target);

	UFUNCTION(BlueprintCallable)
	void SetHumanoidState(EHumanoidState nextState);	

public:	

	UFUNCTION(BlueprintCallable)
	EHumanoidState GetHumanoidState();

	UFUNCTION(BlueprintCallable)
	UWeaponInfo* GetWeaponInfo() 
	{ 
		return weaponInfo;
	}

	UFUNCTION(BlueprintCallable)
	void SetWeaponInfo(UWeaponInfo* weapon)
	{ 
		weaponInfo = weapon;
	}

	UFUNCTION(BlueprintCallable)
	static EFaceDirection GetDirectionFromVector(const FVector& vec);

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

	virtual bool CanMove() override;

	virtual bool CanAttackTarget(AUnitBase* target);

	virtual bool AttackTarget(AUnitBase* target);

	
};
