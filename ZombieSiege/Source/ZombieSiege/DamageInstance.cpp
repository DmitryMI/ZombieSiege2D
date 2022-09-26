// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageInstance.h"

FDamageInstance::FDamageInstance()
{
	this->source = nullptr;
	this->amount = 0;
}

FDamageInstance::FDamageInstance(AUnitBase* source, float amount)
{
	this->source = source;
	this->amount = amount;

	check(this->amount >= 0);
}

FDamageInstance::~FDamageInstance()
{
}
