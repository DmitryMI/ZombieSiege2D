// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitBase.h"
#include "HealingInstance.h"
#include "BuildingProgressChangedEventArgs.h"
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
	bool bIsBuiltOnSpawn = false;

	UPROPERTY(EditDefaultsOnly)
	float initialHealth = 100.0f;

	UPROPERTY(VisibleAnywhere)
	float buildingProgress = 0.0f;

protected:
	virtual void BeginPlay() override;

public:
	ABuilding();

	virtual float ReceiveHealing(const FHealingInstance& repair);

	UFUNCTION(BlueprintCallable)
	float GetBuildingProgressFraction();
	
	UFUNCTION(BlueprintCallable)
	float GetBuildingProgress();

	UFUNCTION(BlueprintCallable)
	void SetBuildingProgressFraction(float progressFraction);

	UFUNCTION(BlueprintCallable)
	void SetBuildingProgress(float progress);

	UFUNCTION(BlueprintCallable)
	void AddBuildingProgress(float addProgress);

	UFUNCTION(BlueprintCallable)
	bool IsBuiltOnSpawn();

	UFUNCTION(BlueprintCallable)
	void SetIsBuiltOnSpawn(bool isBuiltOnSpawn);

	UFUNCTION(BlueprintCallable)
	bool IsFullyBuilt();

	DECLARE_EVENT_OneParam(ABuilding, FOnBuildingProgressChanged, const FBuildingProgressChangedEventArgs&);

	FOnBuildingProgressChanged& OnBuildingProgressChanged() { return onBuildingProgressChangedEvent; }

private:
	FOnBuildingProgressChanged onBuildingProgressChangedEvent;
};
