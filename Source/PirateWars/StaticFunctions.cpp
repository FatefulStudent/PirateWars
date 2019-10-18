// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticFunctions.h"
#include "Kismet/KismetMathLibrary.h"
#define TOLERANCE 0.0001

/** Find the smallest angle between two headings (in degrees) */
float UStaticFunctions::FindDeltaAngleDegrees(float A1, float A2)
{
	// Find the difference
	float Delta = A2 - A1;

	// If change is larger than 180
	if (Delta > 180.0f)
	{
		// Flip to negative equivalent
		Delta = Delta - (360.0f);
	}
	else if (Delta < -180.0f)
	{
		// Otherwise, if change is smaller than -180
		// Flip to positive equivalent
		Delta = Delta + (360.0f);
	}

	// Return delta in [-180,180] range
	return Delta;
}

bool UStaticFunctions::FindLookAtAngle2D(const FVector2D& Start, const FVector2D& Target, float &Angle)
{
	FVector2D Normal = (Target - Start).GetSafeNormal();
	if (!Normal.IsNearlyZero())
	{
		Angle = FMath::RadiansToDegrees(FMath::Atan2(Normal.Y, Normal.X));
		return true;
	}
	return false;
}

float UStaticFunctions::FindYIntersectBySlopeAndPointOfLine(float SlopeAngle, FVector2D OriginPoint)
{
	return -OriginPoint.X * FMath::Tan(FMath::DegreesToRadians(SlopeAngle)) + OriginPoint.Y;
}

float UStaticFunctions::PseudoScalarMultiplication(FVector2D OriginalVector, FVector2D TargetedVector)
{
	return OriginalVector.X*TargetedVector.Y - OriginalVector.Y*TargetedVector.X;
}

bool UStaticFunctions::TargetedPointIsOnLeftSideOfTheLine(float SlopeAngle, FVector2D OriginPointOfTheLine, FVector2D TargetedPoint)
{
	if (abs(SlopeAngle - 90) < TOLERANCE)
	{
		UE_LOG(LogTemp, Warning, TEXT("EDGE CASE 1"))
		return TargetedPoint.X > OriginPointOfTheLine.X;
	}
	else if (abs(SlopeAngle + 90) < TOLERANCE)
	{
		UE_LOG(LogTemp, Warning, TEXT("EDGE CASE 2"))
		return TargetedPoint.X < OriginPointOfTheLine.X;
	}

	float YIntersect = FindYIntersectBySlopeAndPointOfLine(SlopeAngle, OriginPointOfTheLine);
	FVector2D OriginalVector = FVector2D(OriginPointOfTheLine.X - 0, OriginPointOfTheLine.Y - YIntersect);
	FVector2D TargetedVector = FVector2D(TargetedPoint.X - 0, TargetedPoint.Y - YIntersect);
	float PseudoMiltiplicationRes = PseudoScalarMultiplication(OriginalVector, TargetedVector);
	return (abs(SlopeAngle) > 90 ? PseudoMiltiplicationRes > 0 : PseudoScalarMultiplication(OriginalVector, TargetedVector) < 0);
}