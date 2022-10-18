// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Humanoid.h"
#include "UnitStartedBuildingEventArgs.h"
#include "Survivor.generated.h"

class ADoodad;

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ASurvivor : public AHumanoid
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly)
	FName repairWeaponName = "SurvivorFistsRepairing";

	UPROPERTY(EditDefaultsOnly)
	FName gatheringWeaponName = "SurvivorFistsGathering";

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* repairWeapon;

	UPROPERTY(VisibleAnywhere)
	UWeaponInfo* gatheringWeapon;

	void PushFromBuildingRadius(FVector buildingLocation, float radius);

protected:
	
	virtual void BeginPlay() override;

public:
	ASurvivor();
	
	UFUNCTION(BlueprintCallable)
	virtual bool CanBuild(const TSubclassOf<ABuilding>& buildingClass, FVector location);

	UFUNCTION(BlueprintCallable)
	virtual ABuilding* Build(const TSubclassOf<ABuilding>& buildingClass, FVector location);

	UFUNCTION(BlueprintCallable)
	virtual bool CanRepairTarget(ABuilding* target, FAttackTestParameters testParams);

	UFUNCTION(BlueprintCallable)
	virtual float RepairTarget(ABuilding* target);

	UFUNCTION(BlueprintCallable)
	virtual bool CanGatherTarget(ADoodad* gatherTarget, FAttackTestParameters testParams);

	UFUNCTION(BlueprintCallable)
	virtual float GatherTarget(ADoodad* gatherTarget);

public:
	DECLARE_EVENT_OneParam(ASurvivor, FOnUnitStartedBuildingEvent, const FUnitStartedBuildingEventArgs&);
	FOnUnitStartedBuildingEvent& OnUnitStartedBuilding() { return onUnitStartedBuildingEvent; }
	
private:
	FOnUnitStartedBuildingEvent onUnitStartedBuildingEvent;
};
