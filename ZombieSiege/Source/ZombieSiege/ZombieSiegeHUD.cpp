// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegeHUD.h"

void AZombieSiegeHUD::DrawSelectionBox()
{
	float sx = selectionBoxStart.X;
	float sy = selectionBoxStart.Y;
	float ex = selectionBoxEnd.X;
	float ey = selectionBoxEnd.Y;

	// Horizontal 1
	DrawLine(sx, sy, ex, sy, selectionBoxColor, selectionBoxThickness);

	// Horizontal 2
	DrawLine(sx, ey, ex, ey, selectionBoxColor, selectionBoxThickness);

	// Vertical 1
	DrawLine(sx, sy, sx, ey, selectionBoxColor, selectionBoxThickness);

	// Vertical 2
	DrawLine(ex, sy, ex, ey, selectionBoxColor, selectionBoxThickness);

}

void AZombieSiegeHUD::DrawHUD()
{
	if (bDrawSelectionBox)
	{
		DrawSelectionBox();
	}
}

FBox2D AZombieSiegeHUD::GetSelectionBox() const
{
	FVector2D point1 = selectionBoxStart;
	FVector2D point2 = FVector2D(selectionBoxStart.X, selectionBoxEnd.Y);
	FVector2D point3 = selectionBoxEnd;
	FVector2D point4 = FVector2D(selectionBoxEnd.X, selectionBoxEnd.Y);
	return FBox2D({point1, point2, point3, point4 });
}

void AZombieSiegeHUD::BeginSelection(const FVector2D boxStart)
{
	bDrawSelectionBox = true;
	selectionBoxStart = boxStart;
	selectionBoxEnd = boxStart;
}

void AZombieSiegeHUD::UpdateSelection(const FVector2D boxEnd)
{
	selectionBoxEnd = boxEnd;
}

void AZombieSiegeHUD::EndSelection()
{
	bDrawSelectionBox = false;
	selectionBoxStart = FVector2D::Zero();
	selectionBoxEnd = FVector2D::Zero();
}
