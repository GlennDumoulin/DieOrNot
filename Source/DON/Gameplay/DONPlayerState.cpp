// Fill out your copyright notice in the Description page of Project Settings.


#include "DONPlayerState.h"
#include "DON/Actors/DONPlayerCharacter.h"

ADONPlayerState::ADONPlayerState()
{
	PlayerScore = 0;
	bReceivedKillScore = false;
	bReceivedSurviveScore = false;
	bCanReceivePowerup = false;
	bCanReceiveEasyPowerup = true;
}

void ADONPlayerState::RoundStart(FTransform location)
{
	bReceivedKillScore = false;
	bReceivedSurviveScore = false;
	bCanReceivePowerup = false;
	GetPawn<ADONPlayerCharacter>()->RoundStart(location);
}

void ADONPlayerState::GiveKillScore()
{
	if (!bReceivedKillScore) {
		bReceivedKillScore = true;
		PlayerScore++;
	}
	Stats.KillStreak++;
	Stats.Kills++;
}

void ADONPlayerState::MarkDeath(bool suicide)
{
	Stats.KillStreak = 0;
	Stats.Deaths++;
	Stats.LoseStreak++;
	if (suicide) Stats.Suicides++;
}

void ADONPlayerState::GiveSurviveScore()
{
	if (!bReceivedSurviveScore) {
		bReceivedSurviveScore = true;
		PlayerScore++;
		Stats.Survives++;
	}
	Stats.LoseStreak = 0;
}

void ADONPlayerState::SetSuicide()
{
	bCanReceivePowerup = false;
}

void ADONPlayerState::SetCanReceivePowerup()
{
	bCanReceivePowerup = true;
}

void ADONPlayerState::SetCanReceiveEasyPowerup()
{
	bCanReceiveEasyPowerup = false;
}

int ADONPlayerState::GetHiddenScore()
{
	return Stats.CalculateScore();
}

FLinearColor ADONPlayerState::GetPlayerColor() const
{
	return GetPawn<ADONPlayerCharacter>()->GetPlayerColor();
}
