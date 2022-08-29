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
	UPROPERTY(VisibleAnywhere)
	float spriteHeight = 10.0f;

	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperSprite*> TreeBushVariants;

	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperSprite*> TreeSmallVariants;

	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperSprite*> TreeMediumVariants;

	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperSprite*> TreeLargeVariants;	

protected:
	UFUNCTION(BlueprintCallable)
	UPaperSprite* GetCorrectTreeSprite();

	UFUNCTION(BlueprintCallable)
	void UpdateTreeSprite();

	UPROPERTY(Transient)
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
