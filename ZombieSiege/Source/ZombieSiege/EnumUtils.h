// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FaceDirection.h"
#include "UnitState.h"
#include "UnitOrder.h"
#include "EnumUtils.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UEnumUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "EnumUtils")
	static EFaceDirection GetNextFaceDirection(EFaceDirection direction);

	UFUNCTION(BlueprintCallable, Category = "EnumUtils")
	static EFaceDirection GetPreviousFaceDirection(EFaceDirection direction);

	UFUNCTION(BlueprintCallable, Category = "EnumUtils")
	static EUnitState GetNextUnitState(EUnitState state);

	UFUNCTION(BlueprintCallable, Category = "EnumUtils")
	static EUnitState GetPreviousUnitState(EUnitState state);

	UFUNCTION(BlueprintCallable, Category = "EnumUtils")
	static FString GetUnitStateName(EUnitState state);

	UFUNCTION(BlueprintCallable, Category = "EnumUtils")
	static FString GetFaceDirectionName(EFaceDirection direction);

	UFUNCTION(BlueprintCallable, Category = "EnumUtils")
	static FString GetOrderResultName(EOrderResult value);

	static FString GetTeamAttitudeName(ETeamAttitude::Type attitude);
};
