// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GenericTeamAgentInterface.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "SelectionIndicatorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API USelectionIndicatorComponent : public UPaperFlipbookComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	UPaperFlipbook* indicatorFlipbook;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor selectionColorFriendly = FLinearColor::Blue;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor selectionColorNeutral = FLinearColor::Yellow;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor selectionColorHostile = FLinearColor::Red;

	UPROPERTY(VisibleAnywhere)
	bool bIsSelected;

	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ETeamAttitude::Type> attitude;

public:	
	// Sets default values for this component's properties
	USelectionIndicatorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void UpdateSelectionFlipbook();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool IsSelected();

	UFUNCTION(BlueprintCallable)
	void SetIsSelected(bool bIsSelectedArg, ETeamAttitude::Type attitudeArg);
};
