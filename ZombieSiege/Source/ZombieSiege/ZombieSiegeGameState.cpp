// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegeGameState.h"

void AZombieSiegeGameState::BeginPlay()
{
	Super::BeginPlay();

	FGenericTeamId::FAttitudeSolverFunction solver = GetTeamAttitude;
	FGenericTeamId::SetAttitudeSolver(solver);
}

ETeamAttitude::Type AZombieSiegeGameState::GetTeamAttitude(FGenericTeamId teamSource, FGenericTeamId teamTarget)
{
	if (teamSource.GetId() == teamTarget.GetId())
	{
		// Same teams are always friendly
		return ETeamAttitude::Friendly;
	}

	if (teamTarget.GetId() == TEAM_NEUTRAL_PASSIVE)
	{
		// Neutral passives are always considered neutral by all other teams
		return ETeamAttitude::Neutral;
	}

	if (teamSource.GetId() == TEAM_NEUTRAL_AGGRESSIVE || teamTarget.GetId() == TEAM_NEUTRAL_AGGRESSIVE)
	{
		// Neatral aggressives consider everybody their enemies. All other teams consider neutral aggressives enemies
		return ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Neutral;
}
