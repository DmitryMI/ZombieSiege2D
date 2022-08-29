// Fill out your copyright notice in the Description page of Project Settings.


#include "HumanoidGraphicsComponent.h"
#include "Humanoid.h"
#include "FaceDirection.h"
#include "HumanoidState.h"

int UHumanoidGraphicsComponent::GetDirectionSpriteIndex(EFaceDirection faceDirectionEnum)
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

UHumanoidGraphicsComponent::UHumanoidGraphicsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}


void UHumanoidGraphicsComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<USceneComponent*> children;
	GetChildrenComponents(true, children);
	for (USceneComponent* child : children)
	{
		UPaperFlipbookComponent* flipbookComponent = Cast<UPaperFlipbookComponent>(child);
		if (flipbookComponent != nullptr)
		{
			humanoidFlipbookRenderer = flipbookComponent;
		}
	}
	check(humanoidFlipbookRenderer);

	humanoidFlipbookRenderer->SetWorldRotation(FRotator(0, 0, -90));
}


void UHumanoidGraphicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* owner = GetOwner();
	if (owner == nullptr)
	{
		return;
	}

	AHumanoid* humanoid = Cast<AHumanoid>(owner);
	check(humanoid);

	EFaceDirection direction = humanoid->GetFacingDirection();
	int directionIndex = GetDirectionSpriteIndex(direction);

	TArray<UPaperFlipbook*>* animationSet = nullptr;

	switch (humanoid->GetHumanoidState())
	{
	case EHumanoidState::None:
		animationSet = &standFlipbooks;
		break;
	case EHumanoidState::Moving:
		animationSet = &movingFlipbooks;
		break;
	case EHumanoidState::Attacking:
		animationSet = &attackingFlipbooks;
		break;
	case EHumanoidState::Dying:
		animationSet = &dyingFlipbooks;
		break;
	}

	check(animationSet);
	check(animationSet->Num() > directionIndex);

	UPaperFlipbook* flipbook = (*animationSet)[directionIndex];
	check(flipbook);

	check(humanoidFlipbookRenderer);
	humanoidFlipbookRenderer->SetFlipbook(flipbook);
}

