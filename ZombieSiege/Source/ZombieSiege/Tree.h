// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Doodad.h"
#include "TreeType.h"
#include "TreeSize.h"
#include "PaperSpriteComponent.h"
#include "TreeGraphicsComponent.h"
#include "Tree.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API ATree : public ADoodad
{
	GENERATED_BODY()

private:	

	UPROPERTY(VisibleAnywhere)
	ETreeType treeType;

	UPROPERTY(VisibleAnywhere)
	ETreeSize treeSize;		

protected:
	UPROPERTY(Transient)
	UTreeGraphicsComponent* graphicsComponent;

	virtual void BeginPlay() override;

public:

	ATree();

	// Statics

	UFUNCTION(BlueprintCallable)
	static void GetRandomSizeAndType(ETreeType& outType, ETreeSize& outSize);

	// End of Statics


	UFUNCTION(BlueprintCallable)
	void GetTreeTypeAndSize(ETreeType& outType, ETreeSize& outSize)
	{
		outType = treeType;
		outSize = treeSize;
	}

	UFUNCTION(BlueprintCallable)
	void SetTreeTypeAndSize(ETreeType inType, ETreeSize inSize);

	DECLARE_MULTICAST_DELEGATE(FTreeTypeAndSizeChangedEvent);
	FTreeTypeAndSizeChangedEvent& OnTreeTypeAndSizeChangedEvent() { return treeTypeAndSizeChangedEvent; }	

private:
	FTreeTypeAndSizeChangedEvent treeTypeAndSizeChangedEvent;

};
