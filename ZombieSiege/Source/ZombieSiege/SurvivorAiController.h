// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitAiController.h"
#include "SurvivorAiController.generated.h"

class AJobBase;
class AUnitBase;
class ABuilding;

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ASurvivorAiController : public AUnitAiController
{
	GENERATED_BODY()


private:
	UPROPERTY(VisibleAnywhere)
	AJobBase* assignedToJob;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitOrder> gatherOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitOrder> buildOrderClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUnitOrder> repairOrderClass;
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float deltaTime) override;

	UFUNCTION(BlueprintCallable)
	AJobBase* GetAssignedToJob();

	UFUNCTION(BlueprintCallable)
	virtual void SetAssignedToJob(AJobBase* job);

	UFUNCTION(BlueprintCallable)
	virtual void IssueGatherOrder(ADoodad* gatherableUnit, bool bQueue = false);

	UFUNCTION(BlueprintCallable)
	virtual void IssueBuildOrder(TSubclassOf<ABuilding> buildingClass, const FVector& location, bool bQueue = false);

	UFUNCTION(BlueprintCallable)
	virtual void IssueRepairOrder(ABuilding* building, bool bQueue = false);

	
};
