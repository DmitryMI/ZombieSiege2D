// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "FaceDirection.h"
#include "UnitState.h"
#include "UnitGraphicsComponent.h"
#include "WeaponInfo.h"
#include "WeaponManager.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Humanoid.generated.h"

UCLASS()
class ZOMBIESIEGE_API AHumanoid : public AUnitBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	FName weaponDefaultName = "SurvivorFists";	

	UPROPERTY(VisibleAnywhere)
	AWeaponManager* weaponManager;

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* weaponDefault;

public:
	AHumanoid();	

protected:

	UPROPERTY(Transient)
	UUnitGraphicsComponent* graphicsComponent;		

	UFUNCTION(BlueprintCallable)
	AWeaponManager* GetWeaponManager();
	
	virtual void BeginPlay() override;	

public:		

	UFUNCTION(BlueprintCallable)
	UWeaponInfo* GetWeaponInfo() 
	{ 
		return activeWeapon;
	}

	UFUNCTION(BlueprintCallable)
	void SetWeaponInfo(UWeaponInfo* weapon)
	{ 
		activeWeapon = weapon;
	}

	UFUNCTION(BlueprintCallable)
	static EFaceDirection GetDirectionFromVector(const FVector& vec);

	virtual void Tick(float DeltaTime) override;	

	virtual bool CanMove() const override;

	virtual float GetAttackRange() override;

	virtual bool CanEverAttackTarget(AUnitBase* target) override;

	virtual bool CanAttackTarget(AUnitBase* target) override;

	virtual bool AttackTarget(AUnitBase* target) override;
};
