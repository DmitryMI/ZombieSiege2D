// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"
#include "PaperSprite.h"
#include "Kismet/GameplayStatics.h"

void ATree::GetRandomSizeAndType(ETreeType& outType, ETreeSize& outSize)
{
	outType = static_cast<ETreeType>(FMath::RandRange(0, static_cast<uint8>(ETreeType::ETREETYPE_MAX) - 1));
	outSize = static_cast<ETreeSize>(FMath::RandRange(0, static_cast<uint8>(ETreeSize::ETREESIZE_MAX) - 1));
}

ATree::ATree()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"), false);

	treeSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TreeSpriteComponent"), false);	

	FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepRelative, false);
	
	bool bSpriteAttached = treeSpriteComponent->AttachToComponent(RootComponent, attachmentRules);
	if (!bSpriteAttached)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to attach TreeSpriteComponent to the RootComponent!"));
	}

	// TODO Consider making trees static. SPRITES OF STATIC OBJECTS CANNOT BE CHANGED AT RUNTIME!
	// 
	//treeSpriteComponent->SetMobility(EComponentMobility::Static);
	treeSpriteComponent->SetWorldRotation(FRotator(-90, 0, 0));

}


UPaperSprite* ATree::GetCorrectTreeSprite()
{
	
	TArray<UPaperSprite*>* sizeArray = nullptr;
	if (treeSize == ETreeSize::Bush)
	{
		sizeArray = &TreeBushVariants;
	}
	else if (treeSize == ETreeSize::Small)
	{
		sizeArray = &TreeSmallVariants;
	}
	else if (treeSize == ETreeSize::Medium)
	{
		sizeArray = &TreeMediumVariants;
	}
	else if (treeSize == ETreeSize::Large)
	{
		sizeArray = &TreeLargeVariants;
	}

	if (sizeArray == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Unknown Tree Size value: %d!"), treeSize);
		return nullptr;
	}

	uint8 varianceIndex = static_cast<uint8>(treeType);
	
	if (varianceIndex >= sizeArray->Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Tree Variance %d is not defined for Tree Size %d!"), varianceIndex, static_cast<uint8>(treeSize));
		return nullptr;
	}

	UPaperSprite* sprite = (*sizeArray)[varianceIndex];
	return sprite;
}

void ATree::UpdateTreeSprite()
{
	UPaperSprite* sprite = GetCorrectTreeSprite();

	FString name = sprite->GetName();	

	if (sprite != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Setting sprite to %s"), *name);
		bool spriteSet = treeSpriteComponent->SetSprite(sprite);
		if (!spriteSet)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to set sprite!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Setting sprite to NULLPTR"));
	}
}
