// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"
#include "PaperSprite.h"
#include "Kismet/GameplayStatics.h"

ATree* ATree::BeginSpawnTree(UWorld* world, TSubclassOf<ATree> treeClass, const FVector& location)
{

	FTransform transform(location);
	ESpawnActorCollisionHandlingMethod collisionHandling = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* treeActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(world, treeClass, transform, collisionHandling);

	if (treeActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to deferred-spawn a tree!"));
		return nullptr;
	}

	treeActor->SetFolderPath("Trees");

	ATree* tree = Cast<ATree>(treeActor);

	if (tree == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast deferred-spawned actor to ATree!"));
		return nullptr;
	}

	//tree->RootComponent->SetMobility(MOBILITY);
	//tree->treeSpriteComponent->SetMobility(MOBILITY);

	return tree;
}

void ATree::FinishSpawnTree(ATree* tree)
{
	UGameplayStatics::FinishSpawningActor(tree, tree->GetTransform());
}

void ATree::GetRandomSizeAndType(ETreeType& outType, ETreeSize& outSize)
{
	outType = static_cast<ETreeType>(FMath::RandRange(0, static_cast<uint8>(ETreeType::ETREETYPE_MAX) - 1));
	outSize = static_cast<ETreeSize>(FMath::RandRange(0, static_cast<uint8>(ETreeSize::ETREESIZE_MAX) - 1));
}

ATree::ATree()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"), false);	

	RootComponent->SetMobility(EComponentMobility::Static);

	treeSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TreeSpriteComponent"), false);	

	FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepRelative, false);
	
	bool bSpriteAttached = treeSpriteComponent->AttachToComponent(RootComponent, attachmentRules);
	if (!bSpriteAttached)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to attach TreeSpriteComponent to the RootComponent!"));
	}

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

	if (sprite != nullptr)
	{
		bool spriteSet = treeSpriteComponent->SetSprite(sprite);
		if (!spriteSet)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to set sprite! Do not update sprites of Static actors!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Setting sprite to NULLPTR"));
	}
}
