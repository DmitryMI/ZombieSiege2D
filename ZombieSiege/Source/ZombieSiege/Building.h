// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "HealingInstance.h"
#include "BuildingProgressChangedEventArgs.h"
#include "ZombieSiegePlayerController.h"
#include "Turret.h"
#include "Building.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ABuilding : public AUnitBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATurret> turretClass;

	UPROPERTY(EditDefaultsOnly)
	float turretSpawnRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly)
	FName weaponDefaultName;

	UPROPERTY(EditAnywhere)
	bool bIsBuiltOnSpawn = false;

	UPROPERTY(VisibleAnywhere)
	float buildingProgress = 0.0f;

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* weaponDefault;

	UPROPERTY(VisibleAnywhere)
	TArray<ATurret*> turrets;

protected:
	virtual void BeginPlay() override;

	virtual bool CanAttackAnything(FAttackTestParameters testParams);

public:
	ABuilding();


	virtual bool CanAttackPoint(const FVector& targetPoint, FAttackTestParameters testParams) override;

	virtual bool CanAttackTarget(AUnitBase* targetUnit, FAttackTestParameters testParams) override;

	virtual float ReceiveHealing(const FHealingInstance& repair) override;

	UFUNCTION(BlueprintCallable)
	float GetBuildingProgressFraction() const;
	
	UFUNCTION(BlueprintCallable)
	float GetBuildingProgress() const;

	UFUNCTION(BlueprintCallable)
	void SetBuildingProgressFraction(float progressFraction);

	UFUNCTION(BlueprintCallable)
	void SetBuildingProgress(float progress);

	UFUNCTION(BlueprintCallable)
	void AddBuildingProgress(float addProgress);

	UFUNCTION(BlueprintCallable)
	bool IsBuiltOnSpawn() const;

	UFUNCTION(BlueprintCallable)
	void SetIsBuiltOnSpawn(bool isBuiltOnSpawn);

	UFUNCTION(BlueprintCallable)
	bool IsFullyBuilt() const;

	UFUNCTION(BlueprintCallable)
	bool NeedsRepair() const;

	DECLARE_EVENT_OneParam(ABuilding, FOnBuildingProgressChanged, const FBuildingProgressChangedEventArgs&);

	FOnBuildingProgressChanged& OnBuildingProgressChanged() { return onBuildingProgressChangedEvent; }

	virtual bool CanBeBuiltAt(UWorld* world, const FVector& location, AUnitBase* builder = nullptr) const;

	virtual bool CanBeBuiltBy(AZombieSiegePlayerController* playerController) const;

	virtual bool CanBeBuilt(UWorld* world, const FVector& location, AZombieSiegePlayerController* playerController, AUnitBase* builder = nullptr);

private:
	FOnBuildingProgressChanged onBuildingProgressChangedEvent;
};
