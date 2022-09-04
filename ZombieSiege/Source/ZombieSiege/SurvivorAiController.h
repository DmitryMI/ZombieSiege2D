// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SurvivorAiController.generated.h"

class AJobBase;

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ASurvivorAiController : public AAIController
{
	GENERATED_BODY()


private:
	UPROPERTY(VisibleAnywhere)
	AJobBase* assignedToJob;
	
protected:

public:
	UFUNCTION(BlueprintCallable)
	AJobBase* GetAssignedToJob();

	UFUNCTION(BlueprintCallable)
	virtual void SetAssignedToJob(AJobBase* job);
};
