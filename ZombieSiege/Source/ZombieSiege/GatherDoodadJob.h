// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JobBase.h"
#include "GatherDoodadJob.generated.h"

class ADoodad;

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AGatherDoodadJob : public AJobBase
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	ADoodad* targetDoodad;

	virtual void FindExecutors() override;

	virtual void BeginPlay() override;

	virtual bool CanExecute() override;

	virtual bool IsJobInitialized() override;

	virtual bool IsFinished() override;

	virtual bool IsFailed() override;

	virtual void OnStateChanged(EJobState stateOld, EJobState stateNew) override;

	virtual bool IsValidExecutor(AUnitBase* executor) override;

	virtual float CalculateJobSpecificPriorityMetric(AUnitBase* unit) override;

	bool ShouldAssignUnemployedUnit(AUnitBase* unit);

	virtual void IssueOrders();

public:
	
	AGatherDoodadJob();

	virtual void Tick(float deltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void SetTargetDoodad(ADoodad* doodad);

	UFUNCTION(BlueprintCallable)
	ADoodad* GetTargetDoodad();
};
