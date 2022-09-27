// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PaperFlipbook.h"
#include "FaceDirection.h"
#include "UnitGraphicsData.h"
#include "PaperFlipbookComponent.h"
#include "UnitGraphicsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API UUnitGraphicsComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere)
	UUnitGraphicsData* unitGraphicsData;

	UFUNCTION()
	void UpdateFlipbook(EUnitState state, EFaceDirection direction);
	
protected:

	UPROPERTY(Transient)
	UPaperFlipbookComponent* flipbookRenderer;

	virtual void BeginPlay() override;

public:	

	UUnitGraphicsComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;		
};
