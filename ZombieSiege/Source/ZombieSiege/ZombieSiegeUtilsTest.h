// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieSiegeUtils.h"
#include "ZombieSiegeUtilsTest.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API UZombieSiegeUtilsTest : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	
public:

	UFUNCTION(BlueprintCallable)
	static bool TestInsertSortedAscending();
};

