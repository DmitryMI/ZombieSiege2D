// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Doodad.h"
#include "TreeType.h"
#include "TreeSize.h"
#include "PaperSpriteComponent.h"
#include "Tree.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ATree : public ADoodad
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category = "Sprites|Tree")
	TArray<UPaperSprite*> TreeBushVariants;

	UPROPERTY(EditDefaultsOnly, Category = "Sprites|Tree")
	TArray<UPaperSprite*> TreeSmallVariants;

	UPROPERTY(EditDefaultsOnly, Category = "Sprites|Tree")
	TArray<UPaperSprite*> TreeMediumVariants;

	UPROPERTY(EditDefaultsOnly, Category = "Sprites|Tree")
	TArray<UPaperSprite*> TreeLargeVariants;

	UPROPERTY(VisibleAnywhere)
	ETreeType treeType;

	UPROPERTY(VisibleAnywhere)
	ETreeSize treeSize;	

	UFUNCTION(BlueprintCallable)
	UPaperSprite* GetCorrectTreeSprite();

	UFUNCTION(BlueprintCallable)
	void UpdateTreeSprite();

protected:

	UPROPERTY(VisibleAnywhere, Transient)
	UPaperSpriteComponent* treeSpriteComponent;

public:

	UFUNCTION(BlueprintCallable)
	static ATree* BeginSpawnTree(UWorld* world, TSubclassOf<ATree> treeClass, const FVector& location);

	UFUNCTION(BlueprintCallable)
	static void FinishSpawnTree(ATree* tree);

	UFUNCTION(BlueprintCallable)
	static void GetRandomSizeAndType(ETreeType& outType, ETreeSize& outSize);

	ATree();

	UFUNCTION(BlueprintCallable)
	void GetTreeTypeAndSize(ETreeType& outType, ETreeSize& outSize)
	{
		outType = treeType;
		outSize = treeSize;
	}

	UFUNCTION(BlueprintCallable)
	void SetTreeTypeAndSize(ETreeType inType, ETreeSize inSize, bool bUpdateSprite)
	{
		treeType = inType;
		treeSize = inSize;

		if (bUpdateSprite)
		{
			UpdateTreeSprite();
		}
	}

	UFUNCTION(BlueprintCallable)
	ETreeType GetTreeType()
	{
		return treeType;
	}

	UFUNCTION(BlueprintCallable)
	virtual void SetTreeType(ETreeType treeTypeArg)
	{		
		this->treeType = treeTypeArg;
		UpdateTreeSprite();
	}

	UFUNCTION(BlueprintCallable)
	ETreeSize GetTreeSize()
	{
		return treeSize;
	}

	UFUNCTION(BlueprintCallable)
	virtual void SetTreeSize(ETreeSize treeSizeArg)
	{
		this->treeSize = treeSizeArg;
		
		UpdateTreeSprite();
	}
};
