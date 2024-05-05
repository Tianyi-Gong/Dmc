// Fill out your copyright notice in the Description page of Project Settings.


#include "DmcAnimInstance.h"

EMoveDirection8Way UDmcAnimInstance::SelectDirection8WayFromAngle(float Angle, float DeadZoe, EMoveDirection8Way CurrenrtDirection, bool CurrentIsMove)
{
	const float AngleZone = 45;
	const float HalfAnfleZone = AngleZone / 2;

	float ClampAngle = FRotator::ClampAxis(Angle);

	float ConvertClampAngle = (ClampAngle >= 360 - HalfAnfleZone - DeadZoe) ? Angle : ClampAngle;

	float CurrentDirectionMin = AngleZone * ((int)CurrenrtDirection - 1)- HalfAnfleZone - DeadZoe;
	float CurrentDirectionMax = AngleZone * ((int)CurrenrtDirection - 1) + HalfAnfleZone + DeadZoe;

	if (!CurrentIsMove)
	{
		return EMoveDirection8Way::None;
	}
	else if (ConvertClampAngle >= CurrentDirectionMin && ConvertClampAngle <= CurrentDirectionMax && CurrenrtDirection != EMoveDirection8Way::None)
	{
		return CurrenrtDirection;
	}
	else
	{
		int DirectionInt = FMath::RoundToInt(ClampAngle / AngleZone) % 8 + 1;

		return (EMoveDirection8Way)DirectionInt;
	}
}

EMoveDirection4Way UDmcAnimInstance::SelectDirection4WayFromAngle(float Angle, float DeadZoe, EMoveDirection4Way CurrenrtDirection, bool CurrentIsMove)
{
	const float AngleZone = 90;
	const float HalfAnfleZone = AngleZone / 2;

	float ClampAngle = FRotator::ClampAxis(Angle);

	float ConvertClampAngle = (ClampAngle >= 360 - HalfAnfleZone - DeadZoe) ? Angle : ClampAngle;

	float CurrentDirectionMin = AngleZone * ((int)CurrenrtDirection - 1) - HalfAnfleZone - DeadZoe;
	float CurrentDirectionMax = AngleZone * ((int)CurrenrtDirection - 1) + HalfAnfleZone + DeadZoe;

	if (!CurrentIsMove)
	{
		return EMoveDirection4Way::None;
	}
	else if (ConvertClampAngle >= CurrentDirectionMin && ConvertClampAngle <= CurrentDirectionMax && CurrenrtDirection != EMoveDirection4Way::None)
	{
		return CurrenrtDirection;
	}
	else
	{
		int DirectionInt = FMath::RoundToInt(ClampAngle / AngleZone) % 4 + 1;

		return (EMoveDirection4Way)DirectionInt;
	}
}

EMoveDirection2Way UDmcAnimInstance::SelectDirection2WayFromAngle(float Angle, float DeadZoe, EMoveDirection2Way CurrenrtDirection, bool CurrentIsMove)
{
	const float AngleZone = 180;
	const float HalfAnfleZone = AngleZone / 2;

	float ClampAngle = FRotator::ClampAxis(Angle);

	float ConvertClampAngle = (ClampAngle >= 360 - HalfAnfleZone - DeadZoe) ? Angle : ClampAngle;

	float CurrentDirectionMin = AngleZone * ((int)CurrenrtDirection - 1) - HalfAnfleZone - DeadZoe;
	float CurrentDirectionMax = AngleZone * ((int)CurrenrtDirection - 1) + HalfAnfleZone + DeadZoe;

	if (!CurrentIsMove)
	{
		return EMoveDirection2Way::None;
	}
	else if (ConvertClampAngle >= CurrentDirectionMin && ConvertClampAngle <= CurrentDirectionMax && CurrenrtDirection != EMoveDirection2Way::None)
	{
		return CurrenrtDirection;
	}
	else
	{
		int DirectionInt = FMath::RoundToInt(ClampAngle / AngleZone) % 2 + 1;

		return (EMoveDirection2Way)DirectionInt;
	}
}