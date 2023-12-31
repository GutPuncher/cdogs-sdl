/*
	C-Dogs SDL
	A port of the legendary (and fun) action/arcade cdogs.

	Copyright (c) 2013-2014, 2016-2017, 2020-2023 Cong Xu
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include "c_array.h"
#include "campaign_entry.h"
#include "character.h"
#include "mission.h"
#include "sys_config.h"

typedef struct
{
	char *Name;
	CArray subFolders; // of CampaignList
	CArray list;	   // of CampaignEntry
} CampaignList;

typedef struct
{
	CampaignList campaignList;
	CampaignList dogfightList;
	CampaignEntry quickPlayEntry;
} CustomCampaigns;

typedef struct
{
	char *Title;
	char *Author;
	char *Description;
	bool Ammo;
	bool SkipWeaponMenu;
	bool BuyAndSell;
	bool RandomPickups;
	int DoorOpenTicks;
	int Lives;
	int MaxLives;
	int PlayerHP;
	int PlayerMaxHP;
	int PlayerExcessHP;
	CArray Missions; // of Mission
	CharacterStore characters;
	MusicChunk CustomSongs[MUSIC_COUNT];
	void *CustomData;
	void (*CustomDataTerminate)(void *);
} CampaignSetting;

typedef struct
{
	CampaignSetting Setting;
	CampaignEntry Entry;
	int MissionIndex;
	bool IsLoaded;
	// TODO: it may be possible to completely remove IsClient and rely on
	// protocol definitions
	bool IsClient;
	bool IsError;
	bool OptionsSet;
	bool IsComplete;
	bool IsQuit;
} Campaign;
extern Campaign gCampaign;

void CampaignInit(Campaign *campaign);
void CampaignTerminate(Campaign *campaign);
void CampaignSettingInit(CampaignSetting *setting);
void CampaignSettingTerminate(CampaignSetting *c);
void CampaignSettingTerminateAll(CampaignSetting *setting);

int CampaignGetMaxLives(const Campaign *c);
int CampaignGetLives(const Campaign *c);
int CampaignGetMaxHP(const Campaign *c);
int CampaignGetExcessHP(const Campaign *c);
int CampaignGetHP(const Campaign *c);

bool CampaignListIsEmpty(const CampaignList *c);

void LoadAllCampaigns(CustomCampaigns *campaigns);
void UnloadAllCampaigns(CustomCampaigns *campaigns);

Mission *CampaignGetCurrentMission(Campaign *campaign);
void CampaignSeedRandom(const Campaign *campaign);

void CampaignAndMissionSetup(Campaign *campaign, struct MissionOptions *mo);
void CampaignDeleteMission(Campaign *c, const size_t idx);
