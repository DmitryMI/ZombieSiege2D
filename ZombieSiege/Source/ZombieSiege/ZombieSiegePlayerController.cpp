// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

void AZombieSiegePlayerController::OnCameraVertical(float value)
{
	APawn* posessedPawn = GetPawn();
	if (posessedPawn != nullptr)
	{
		FVector location = posessedPawn->GetActorLocation();
		location += FVector::LeftVector * cameraDefaultSpeed * value * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		posessedPawn->SetActorLocation(location);
	}
}

void AZombieSiegePlayerController::OnCameraHorizontal(float value)
{
	APawn* posessedPawn = GetPawn();
	if (posessedPawn != nullptr)
	{
		FVector location = posessedPawn->GetActorLocation();
		location += FVector::ForwardVector * cameraDefaultSpeed * value * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		posessedPawn->SetActorLocation(location);
	}
}

void AZombieSiegePlayerController::OnCameraZoom(float value)
{
	APawn* posessedPawn = GetPawn();
	if (posessedPawn == nullptr)
	{
		return;		
	}

	if (FMath::IsNearlyZero(value))
	{
		return;
	}

	TArray<AActor*> cameraActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "StrategicCamera", cameraActors);

	if (cameraActors.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Strategic Camera not found!"));
		return;
	}

	AActor* cameraActor = cameraActors[0];
	UCameraComponent* cameraComponent = Cast< UCameraComponent>(cameraActor->GetComponentByClass(UCameraComponent::StaticClass()));
	if (cameraComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Strategic Camera actor does not have a camera component!"));
		return;
	}

	float currentSize = cameraComponent->OrthoWidth;

	currentSize -= value * cameraDefaultZoomSpeed * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	currentSize = FMath::Clamp(currentSize, cameraDefaultMinSize, cameraDefaultMaxSize);

	cameraComponent->SetOrthoWidth(currentSize);
}

void AZombieSiegePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	this->InputComponent->BindAxis("CameraVertical", this, &AZombieSiegePlayerController::OnCameraVertical);
	this->InputComponent->BindAxis("CameraHorizontal", this, &AZombieSiegePlayerController::OnCameraHorizontal);
	this->InputComponent->BindAxis("CameraZoom", this, &AZombieSiegePlayerController::OnCameraZoom);
}