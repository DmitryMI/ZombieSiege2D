// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/AudioComponent.h"
#include "FootstepSoundComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESIEGE_API UFootstepSoundComponent : public UAudioComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.01"))
	float triggerDistance = 100.0f;

	UPROPERTY(VisibleAnywhere)
	float travelledDistanceAccumulator = 0.0f;

	UPROPERTY(VisibleAnywhere)
	FVector lastFrameLocation;

public:	
	// Sets default values for this component's properties
	UFootstepSoundComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
