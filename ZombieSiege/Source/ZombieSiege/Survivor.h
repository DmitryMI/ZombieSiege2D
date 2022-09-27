// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Humanoid.h"
#include "UnitStartedBuildingEventArgs.h"
#include "Survivor.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ASurvivor : public AHumanoid
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly)
	FName repairWeaponName = "SurvivorRepair";

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* repairWeapon;

protected:
	
	virtual void BeginPlay() override;

public:
	ASurvivor();
	
	UFUNCTION(BlueprintCallable)
	virtual bool CanBuild(const TSubclassOf<ABuilding>& buildingClass, FVector location);

	UFUNCTION(BlueprintCallable)
	virtual ABuilding* Build(const TSubclassOf<ABuilding>& buildingClass, FVector location);

	UFUNCTION(BlueprintCallable)
	virtual bool CanRepairTarget(ABuilding* target);

	UFUNCTION(BlueprintCallable)
	virtual float RepairTarget(ABuilding* target);

public:
	DECLARE_EVENT_OneParam(ASurvivor, FOnUnitStartedBuildingEvent, const FUnitStartedBuildingEventArgs&);
	FOnUnitStartedBuildingEvent& OnUnitStartedBuilding() { return onUnitStartedBuildingEvent; }
	
private:
	FOnUnitStartedBuildingEvent onUnitStartedBuildingEvent;
};
