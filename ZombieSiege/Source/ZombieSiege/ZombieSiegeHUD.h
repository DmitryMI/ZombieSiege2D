// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ZombieSiegeHUD.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AZombieSiegeHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FLinearColor selectionBoxColor;

	UPROPERTY(EditDefaultsOnly)
	float selectionBoxThickness;

	bool bDrawSelectionBox;
	FVector2D selectionBoxStart;
	FVector2D selectionBoxEnd;
	

protected:
	virtual void DrawSelectionBox();

public:
	virtual void DrawHUD() override;

	FBox2D GetSelectionBox() const;

	UFUNCTION(BlueprintCallable)
	void BeginSelection(const FVector2D boxStart);

	UFUNCTION(BlueprintCallable)
	void UpdateSelection(const FVector2D boxEnd);

	UFUNCTION(BlueprintCallable)
	void EndSelection();	
	
};
