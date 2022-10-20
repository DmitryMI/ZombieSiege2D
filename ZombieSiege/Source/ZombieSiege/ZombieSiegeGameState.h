// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GenericTeamAgentInterface.h"
#include "Macros.h"
#include "ZombieSiegeGameState.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AZombieSiegeGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	static ETeamAttitude::Type GetTeamAttitude(FGenericTeamId teamSource, FGenericTeamId teamTarget);
	
};
