// Fill out your copyright notice in the Description page of Project Settings.


#include "HumanoidGraphicsComponent.h"
#include "Humanoid.h"
#include "FaceDirection.h"
#include "AnimationState.h"

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

	humanoidFlipbookRenderer = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("SpriteRenderer"), false);
	ensure(humanoidFlipbookRenderer);

	bool attached = humanoidFlipbookRenderer->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	ensure(attached);

	humanoidFlipbookRenderer->SetWorldRotation(FRotator(0, 0, -90));
}


void UHumanoidGraphicsComponent::BeginPlay()
{
	Super::BeginPlay();
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

	switch (humanoid->GetAnimationState())
	{
	case EAnimationState::None:
		animationSet = &standFlipbooks;
		break;
	case EAnimationState::Moving:
		animationSet = &movingFlipbooks;
		break;
	case EAnimationState::Attacking:
		animationSet = &attackingFlipbooks;
		break;
	case EAnimationState::Dying:
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

