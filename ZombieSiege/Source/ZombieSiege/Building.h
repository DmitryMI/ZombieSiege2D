// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "HealingInstance.h"
#include "BuildingProgressChangedEventArgs.h"
#include "ZombieSiegePlayerController.h"
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
	FName weaponDefaultName;

	UPROPERTY(EditDefaultsOnly)
	bool bIsBuiltOnSpawn = false;

	UPROPERTY(VisibleAnywhere)
	float buildingProgress = 0.0f;

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* weaponDefault;

protected:
	virtual void BeginPlay() override;

public:
	ABuilding();

	virtual float ReceiveHealing(const FHealingInstance& repair) override;

	virtual bool CanBeginAttackTarget(AUnitBase* attackTarget) override;

	virtual bool CanBeginAttackPoint(const FVector& targetPoint) override;

	virtual bool CanEverAttackPoint() override;

	virtual bool CanEverAttackTarget(AUnitBase* targetUnit) override;

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
