// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UnitBase.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h" 
#include "TurretAIController.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ATurretAIController : public AAIController
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere)
	bool bAutoAimEnabled = true;

protected:
	virtual void BeginPlay() override;

	UAISenseConfig_Sight* GetSightSenseConfig();

	void SetPerceptionSightRadius(float radius);

	virtual void OnPossess(APawn* pawn) override;

	UFUNCTION()
	virtual void OnTargetPerceptionUpdatedHandler(AActor* Actor, FAIStimulus Stimulus);

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	bool GetPerceivedAttackableEnemies(TArray<AUnitBase*>& outEnemies) const;

	UFUNCTION(BlueprintCallable)
	bool IsAutoAimEnabled();

	UFUNCTION(BlueprintCallable)
	void SetAutoAimEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable)
	AUnitBase* GetTargetUnit();

	UFUNCTION(BlueprintCallable)
	void SetTargetUnit(AUnitBase* target);
};
