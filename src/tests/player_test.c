#define SDL_MAIN_HANDLED
#include <cbehave/cbehave.h>

#include <player.h>

#include <SDL_joystick.h>

#include <actors.h>
#include <game_mode.h>
#include <net_client.h>
#include <utils.h>

// Stubs
CampaignOptions gCampaign;
NetClient gNetClient;
CArray gPlayerTemplates;
TActor *ActorGetByUID(const int uid)
{
	UNUSED(uid);
	return NULL;
}
void ActorDestroy(TActor *a)
{
	UNUSED(a);
}
void CharacterSetColors(Character *c)
{
	UNUSED(c);
}
const char *JoyName(const int deviceIndex)
{
	UNUSED(deviceIndex);
	return NULL;
}
int ModeLives(const GameMode mode)
{
	UNUSED(mode);
	return 0;
}
int ModeMaxHealth(const GameMode mode)
{
	UNUSED(mode);
	return 0;
}
NPlayerData NMakePlayerData(const PlayerData *p)
{
	UNUSED(p);
	NPlayerData d = NPlayerData_init_default;
	return d;
}
const GunDescription *StrGunDescription(const char *s)
{
	UNUSED(s);
	return NULL;
}


FEATURE(1, "Assign unused input device")
	// This feature is used to assign input devices to players, before the game
	// begins. All input devices can emit the "fire" command, at which point
	// those input devices are assigned to the next unassigned player. This
	// lets us easily assign different input devices to different players,
	// regardless of the number and type of input devices and players.
	PlayerDataInit(&gPlayerDatas);
	NPlayerData pd = PlayerDataDefault(0);
	PlayerDataAddOrUpdate(pd);
	pd.UID = 1;
	PlayerDataAddOrUpdate(pd);
	SCENARIO("Assign device to unset player")
		GIVEN("a player with an unset input device")
			PlayerData *p = CArrayGet(&gPlayerDatas, 0);
			p->inputDevice = INPUT_DEVICE_UNSET;
		WHEN("I assign it with an input device")
			const input_device_e d = INPUT_DEVICE_KEYBOARD;
			const int idx = 0;
			bool res = PlayerTrySetUnusedInputDevice(p, d, idx);
		THEN("the assignment should succeed")
			SHOULD_BE_TRUE(res);
		AND("the player should have the input device set")
			SHOULD_INT_EQUAL((int)p->inputDevice, (int)d);
			SHOULD_INT_EQUAL(p->deviceIndex, idx);
	SCENARIO_END
	SCENARIO("Assign device to set player")
		GIVEN("a player with an input device")
			PlayerData *p = CArrayGet(&gPlayerDatas, 0);
			const input_device_e d = INPUT_DEVICE_KEYBOARD;
			p->inputDevice = d;
			const int idx = 0;
			p->deviceIndex = idx;
		WHEN("I assign it with a different input device")
			bool res = PlayerTrySetUnusedInputDevice(p, INPUT_DEVICE_MOUSE, 0);
		THEN("the assignment should fail")
			SHOULD_BE_FALSE(res);
		AND("the player's input device should be unchanged");
			SHOULD_INT_EQUAL((int)p->inputDevice, (int)d);
			SHOULD_INT_EQUAL(p->deviceIndex, idx);
	SCENARIO_END
	SCENARIO("Assign already used device")
		GIVEN("a player with an unset input device")
			PlayerData *p = CArrayGet(&gPlayerDatas, 0);
			p->inputDevice = INPUT_DEVICE_UNSET;
		AND("a player with a keyboard device")
			PlayerData *p2 = CArrayGet(&gPlayerDatas, 1);
			p2->inputDevice = INPUT_DEVICE_KEYBOARD;
			p2->deviceIndex = 0;
		WHEN("I assign the first player with the second's input device")
			bool res = PlayerTrySetUnusedInputDevice(
				p, p2->inputDevice, p2->deviceIndex);
		THEN("the assignment should fail")
			SHOULD_BE_FALSE(res);
		AND("the player's input device should be unset");
			SHOULD_INT_EQUAL((int)p->inputDevice, (int)INPUT_DEVICE_UNSET);
	SCENARIO_END
	PlayerDataTerminate(&gPlayerDatas);
FEATURE_END

int main(void)
{
	cbehave_feature features[] =
	{
		{feature_idx(1)}
	};
	
	return cbehave_runner("Player features are:", features);
}
