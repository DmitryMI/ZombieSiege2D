// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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

public:

	UFUNCTION(BlueprintCallable)
	bool IsAutoAimEnabled();

	UFUNCTION(BlueprintCallable)
	void SetAutoAimEnabled(bool bEnabled);

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
