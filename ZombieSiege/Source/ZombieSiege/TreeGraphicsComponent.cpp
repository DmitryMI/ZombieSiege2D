// Fill out your copyright notice in the Description page of Project Settings.


#include "TreeGraphicsComponent.h"
#include "Tree.h"

// Sets default values for this component's properties
UTreeGraphicsComponent::UTreeGraphicsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	treeSpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("TreeSpriteComponent"), true);

	FAttachmentTransformRules attachmentRules(EAttachmentRule::KeepRelative, false);

	bool bSpriteAttached = treeSpriteComponent->AttachToComponent(this, attachmentRules);
	if (!bSpriteAttached)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to attach TreeSpriteComponent to the RootComponent!"));
	}

	treeSpriteComponent->SetMobility(EComponentMobility::Movable);
	treeSpriteComponent->SetWorldRotation(FRotator(0, 0, -90));
	treeSpriteComponent->SetRelativeLocation(FVector::UpVector * spriteHeight);
}

UPaperSprite* UTreeGraphicsComponent::GetCorrectTreeSprite()
{
	AActor* owner = GetOwner();
	check(owner);
	ATree* tree = Cast<ATree>(owner);
	check(tree);

	ETreeSize treeSize;
	ETreeType treeType;

	tree->GetTreeTypeAndSize(treeType, treeSize);

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

void UTreeGraphicsComponent::UpdateTreeSprite()
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

	treeSpriteComponent->SetRelativeLocation(FVector::UpVector * spriteHeight);
}


void UTreeGraphicsComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* owner = GetOwner();
	check(owner);
	ATree* tree = Cast<ATree>(owner);
	check(tree);

	UpdateTreeSprite();

	tree->OnTreeTypeAndSizeChangedEvent().AddUObject(this, &UTreeGraphicsComponent::OnTreeTypeAndSizeChangedHandler);
}

void UTreeGraphicsComponent::OnTreeTypeAndSizeChangedHandler()
{
	UpdateTreeSprite();
}


void UTreeGraphicsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTreeGraphicsComponent::SetIsTransparent(bool bIsTransparent)
{
	check(treeSpriteComponent);

	FLinearColor color;
	if (bIsTransparent)
	{
		color = FLinearColor(1, 1, 1, 0.5f);
	}
	else
	{
		color = FLinearColor(1, 1, 1, 1);
	}

	treeSpriteComponent->SetSpriteColor(color);
}

