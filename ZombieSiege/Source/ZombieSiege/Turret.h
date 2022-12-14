// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "WeaponInfo.h"
#include "AttackDispatcher.h"
#include "AttackTestParameters.h"
#include "Turret.generated.h"

class AUnitBase;

UCLASS()
class ZOMBIESIEGE_API ATurret : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	bool bAutoSetupWeapon = false;

	UPROPERTY(EditDefaultsOnly)
	FName weaponDefaultName;

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* weaponInfo;

	UPROPERTY(VisibleAnywhere)
	UAttackDispatcher* attackDispatcher;

	UPROPERTY(VisibleAnywhere)
	AUnitBase* owningUnit;

	UPROPERTY(VisibleAnywhere)
	bool bIsTurretActive = true;

	UPROPERTY(VisibleAnywhere)
	int turretIndex = -1;

public:	
	// Sets default values for this component's properties
	ATurret();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual bool CanAttackAnything(FAttackTestParameters testParams);

public:	
	void SetWeaponInfo(UWeaponInfo* weapon);

	UFUNCTION(BlueprintCallable)
	bool IsOnCooldown();

	UFUNCTION(BlueprintCallable)
	bool CanAttackTarget(AActor* targetActor, FAttackTestParameters testParams);

	UFUNCTION(BlueprintCallable)
	bool CanAttackPoint(const FVector& targetPoint, FAttackTestParameters testParams);

	UFUNCTION(BlueprintCallable)
	bool CanBeginAttackTarget(AActor* targetActor);

	UFUNCTION(BlueprintCallable)
	bool CanBeginAttackPoint(const FVector& targetPoint);

	UFUNCTION(BlueprintCallable)
	bool BeginAttackTarget(AActor* targetActor);

	UFUNCTION(BlueprintCallable)
	bool BeginAttackPoint(const FVector& targetPoint);

	UFUNCTION(BlueprintCallable)
	void SetTurrectActive(bool active);

	UFUNCTION(BlueprintCallable)
	bool IsTurretActive();

	UFUNCTION(BlueprintCallable)
	void SetOwningUnit(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	int GetTurretIndex();

	UFUNCTION(BlueprintCallable)
	void SetTurretIndex(int index);

	UFUNCTION(BlueprintCallable)
	AUnitBase* GetOwningUnit();
};
