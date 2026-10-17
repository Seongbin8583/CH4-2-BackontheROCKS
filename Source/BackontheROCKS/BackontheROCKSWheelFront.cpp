// Copyright Epic Games, Inc. All Rights Reserved.

#include "BackontheROCKSWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UBackontheROCKSWheelFront::UBackontheROCKSWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}