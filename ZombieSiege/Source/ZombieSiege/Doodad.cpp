// Fill out your copyright notice in the Description page of Project Settings.


#include "Doodad.h"

ADoodad::ADoodad()
{
 	// Doodads never tick.
	PrimaryActorTick.bCanEverTick = false;

}

void ADoodad::BeginPlay()
{
	Super::BeginPlay();
	
}


