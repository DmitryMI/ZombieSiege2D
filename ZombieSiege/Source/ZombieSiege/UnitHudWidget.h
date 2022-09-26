// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitHudWidget.generated.h"

class AUnitBase;

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UUnitHudWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly)
	AUnitBase* unit;

	UFUNCTION(BlueprintCallable)
	bool ShouldShowBuildingProgress();

	UFUNCTION(BlueprintCallable)
	float GetBuildingProgressFraction();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUnitHud();

	UFUNCTION(BlueprintCallable)
	void SetUnit(AUnitBase* value);

	UFUNCTION(BlueprintCallable)
	AUnitBase* GetUnit();

};
