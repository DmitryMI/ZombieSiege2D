// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitGraphicsData.h"
#include "ZombieSiegeUtils.h"
#include "EnumUtils.h"

UPaperFlipbook* UUnitGraphicsData::GetFlipbook(EUnitState state, EFaceDirection direction)
{
	int directionIndex = UZombieSiegeUtils::GetDirectionSpriteIndex(direction);

	TArray<UPaperFlipbook*>* animationSet = nullptr;

	switch (state)
	{
	case EUnitState::None:
		animationSet = &standFlipbooks;
		break;
	case EUnitState::Moving:
		animationSet = &movingFlipbooks;
		break;
	case EUnitState::AttackingBackswing:
	case EUnitState::AttackingRelaxation:
		animationSet = &attackingFlipbooks;
		break;
	case EUnitState::Dying:
		animationSet = &dyingFlipbooks;
		break;
	case EUnitState::Birth:
		animationSet = &birthFlipbooks;
		break;
	}

	if (!animationSet)
	{
		FString name;
		GetName(name);
		FString stateName = UEnumUtils::GetUnitStateName(state);
		//UE_LOG(LogTemp, Error, TEXT("Animation of %s is not defined for state %s"), *name, *stateName);
		return nullptr;
	}

	if (animationSet->Num() <= directionIndex)
	{
		FString name;
		GetName(name);
		FString stateName = UEnumUtils::GetUnitStateName(state);
		FString directionName = UEnumUtils::GetFaceDirectionName(direction);
		UE_LOG(LogTemp, Error, TEXT("Animation of %s is not defined for state %s and direction %s"), *name, *stateName, *directionName);
		return nullptr;
	}

	//check(animationSet);
	//check(animationSet->Num() > directionIndex);

	UPaperFlipbook* flipbook = (*animationSet)[directionIndex];
	check(flipbook);

	return flipbook;
}
