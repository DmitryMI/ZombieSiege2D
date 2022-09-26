// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PaperFlipbook.h"
#include "FaceDirection.h"
#include "PaperFlipbookComponent.h"
#include "UnitGraphicsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API UUnitGraphicsComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> standFlipbooks;
	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> movingFlipbooks;
	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> attackingFlipbooks;
	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> dyingFlipbooks;
	UPROPERTY(EditDefaultsOnly)
	TArray<UPaperFlipbook*> birthFlipbooks;

	UFUNCTION()
	void UpdateFlipbook(EUnitState state, EFaceDirection direction);
	
protected:

	UPROPERTY(Transient)
	UPaperFlipbookComponent* flipbookRenderer;

	virtual void BeginPlay() override;

public:	

	static int GetDirectionSpriteIndex(EFaceDirection faceDirectionEnum);

	UUnitGraphicsComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		
};
