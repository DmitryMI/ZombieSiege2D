// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TreeType.h"
#include "TreeSize.h"
#include "PaperSpriteComponent.h"
#include "TreeGraphicsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API UTreeGraphicsComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Sprites|Tree")
	float spriteHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Sprites|Tree")
	TArray<UPaperSprite*> TreeBushVariants;

	UPROPERTY(EditDefaultsOnly, Category = "Sprites|Tree")
	TArray<UPaperSprite*> TreeSmallVariants;

	UPROPERTY(EditDefaultsOnly, Category = "Sprites|Tree")
	TArray<UPaperSprite*> TreeMediumVariants;

	UPROPERTY(EditDefaultsOnly, Category = "Sprites|Tree")
	TArray<UPaperSprite*> TreeLargeVariants;	

protected:
	UFUNCTION(BlueprintCallable)
	UPaperSprite* GetCorrectTreeSprite();

	UFUNCTION(BlueprintCallable)
	void UpdateTreeSprite();

	UPROPERTY()
	UPaperSpriteComponent* treeSpriteComponent;

public:	
	UTreeGraphicsComponent();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnTreeTypeAndSizeChangedHandler();

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetSpriteZ(float z)
	{
		spriteHeight = z;
		UpdateTreeSprite();
	}

	UFUNCTION(BlueprintCallable)
	void SetIsTransparent(bool bIsTransparent);
};
