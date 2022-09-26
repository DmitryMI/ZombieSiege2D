// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitGraphicsComponent.h"
#include "Humanoid.h"
#include "FaceDirection.h"
#include "UnitState.h"

int UUnitGraphicsComponent::GetDirectionSpriteIndex(EFaceDirection faceDirectionEnum)
{
	if (static_cast<uint8>(faceDirectionEnum) & static_cast<uint8>(EFaceDirection::Up))
	{
		return 0;
	}
	else if (static_cast<uint8>(faceDirectionEnum) & static_cast<uint8>(EFaceDirection::Down))
	{
		return 1;
	}
	else if (static_cast<uint8>(faceDirectionEnum) & static_cast<uint8>(EFaceDirection::Right))
	{
		return 2;
	}
	else if (static_cast<uint8>(faceDirectionEnum) & static_cast<uint8>(EFaceDirection::Left))
	{
		return 3;
	}
	else
	{
		return 1;
	}
}

UUnitGraphicsComponent::UUnitGraphicsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}


void UUnitGraphicsComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> children;
	GetChildrenComponents(true, children);
	for (USceneComponent* child : children)
	{
		UPaperFlipbookComponent* flipbookComponent = Cast<UPaperFlipbookComponent>(child);
		if (flipbookComponent != nullptr)
		{
			flipbookRenderer = flipbookComponent;
		}
	}
	check(flipbookRenderer);

	flipbookRenderer->SetWorldRotation(FRotator(0, 0, -90));

	AActor* owner = GetOwner();
	check(owner);

	AHumanoid* humanoid = Cast<AHumanoid>(owner);
	check(humanoid);
}

void UUnitGraphicsComponent::UpdateFlipbook(EUnitState state, EFaceDirection direction)
{
	int directionIndex = GetDirectionSpriteIndex(direction);

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

	check(flipbookRenderer);
	flipbookRenderer->SetFlipbook(flipbook);
}


void UUnitGraphicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* owner = GetOwner();
	check(owner);

	AHumanoid* humanoid = Cast<AHumanoid>(owner);
	check(humanoid);

	UpdateFlipbook(humanoid->GetUnitState(), humanoid->GetFacingDirection());
}

