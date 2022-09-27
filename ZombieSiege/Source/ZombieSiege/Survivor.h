// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Humanoid.h"
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
	
	virtual bool CanBuild(const TSubclassOf<ABuilding>& buildingClass, FVector location);
	virtual ABuilding* Build(const TSubclassOf<ABuilding>& buildingClass, FVector location);

	virtual bool CanRepairTarget(ABuilding* target);
	virtual float RepairTarget(ABuilding* target);
};
