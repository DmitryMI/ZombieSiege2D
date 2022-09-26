// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"
#include "PaperSprite.h"
#include "ZombieSiegePlayerState.h"
#include "Kismet/GameplayStatics.h"

void ATree::GetRandomSizeAndType(ETreeType& outType, ETreeSize& outSize)
{
	outType = static_cast<ETreeType>(FMath::RandRange(0, static_cast<uint8>(ETreeType::ETREETYPE_MAX) - 1));
	outSize = static_cast<ETreeSize>(FMath::RandRange(0, static_cast<uint8>(ETreeSize::ETREESIZE_MAX) - 1));
}

float ATree::ReceiveDamage(const FDamageInstance& damage)
{
	if (damage.source != nullptr)
	{
		OnHarvestingHit();
	}

	// UnitBase's implementation decreases health and initiates dying sequence
	return Super::ReceiveDamage(damage);
}

void ATree::GetTreeTypeAndSize(ETreeType& outType, ETreeSize& outSize)
{
	outType = treeType;
	outSize = treeSize;
}

void ATree::SetTreeTypeAndSize(ETreeType inType, ETreeSize inSize)
{
	ETreeType oldType = treeType;
	ETreeSize oldSize = treeSize;

	treeType = inType;
	treeSize = inSize;

	if (oldType != treeType || oldSize != treeSize)
	{
		treeTypeAndSizeChangedEvent.Broadcast();
	}
}

void ATree::BeginPlay()
{
	Super::BeginPlay();
	graphicsComponent = Cast<UTreeGraphicsComponent>(GetComponentByClass(UTreeGraphicsComponent::StaticClass()));

	check(graphicsComponent);
}

void ATree::FinishDying(const FDamageInstance& killingDamageInstance)
{
	if (killingDamageInstance.source != nullptr)
	{
		// Add lumber to the attacker's storage
		AZombieSiegePlayerState* attackerPlayerState = killingDamageInstance.source->GetOwningPlayerState();

		if (attackerPlayerState)
		{
			// Flooring the Max Health
			int lumberAmount = static_cast<int>(GetMaxHealth());

			attackerPlayerState->AddResourceToStorage(EResourceType::Lumber, lumberAmount);
		}
	}

	// TODO Replace with Tree-specific code
	Super::FinishDying(killingDamageInstance);
}

ATree::ATree()
{
	bCanEverBeGathered = true;
}

