// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitGraphicsData.h"
#include "ZombieSiegeUtils.h"

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

	check(animationSet);
	check(animationSet->Num() > directionIndex);

	UPaperFlipbook* flipbook = (*animationSet)[directionIndex];
	check(flipbook);

	return flipbook;
}
