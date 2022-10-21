// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZombieSiegeGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AZombieSiegeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:

	// TODO Move to some other actor! PlayerController also relies on this parameter!
	UPROPERTY(EditDefaultsOnly)
	float terrainHeight = 0.0f;

public:

	UFUNCTION(BlueprintCallable, Exec)
	void SpawnDebugHorde(FName unitTypeName, int count, bool bIssueAttackOnMoveOrder);

	UFUNCTION(BlueprintCallable, Exec)
	void SpawnDebugZombies(int count, bool bIssueAttackOnMoveOrder);

	UFUNCTION(BlueprintCallable, Exec)
	void KillAll(FName unitTypeName, bool bForceDeath = true);
	
};
