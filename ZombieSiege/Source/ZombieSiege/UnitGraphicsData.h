// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PaperFlipbook.h"
#include "UnitState.h"
#include "FaceDirection.h"
#include "UnitGraphicsData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ZOMBIESIEGE_API UUnitGraphicsData : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> standFlipbooks;

	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> movingFlipbooks;

	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> attackingFlipbooks;

	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> dyingFlipbooks;

	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> birthFlipbooks;
	
public:

	UFUNCTION(BlueprintCallable)
	static UUnitGraphicsData* CreateUnitGraphicsData(UObject* outer, FName graphicsName, TSubclassOf<UUnitGraphicsData> graphicsClass)
	{
		return NewObject<UUnitGraphicsData>(outer, graphicsClass.Get(), graphicsName);
	}

	UFUNCTION(BlueprintCallable)
	UPaperFlipbook* GetFlipbook(EUnitState state, EFaceDirection direction);
};
