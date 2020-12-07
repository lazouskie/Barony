/*-------------------------------------------------------------------------------

	BARONY
	File: player.hpp
	Desc: contains various declarations for player code.

	Copyright 2013-2016 (c) Turning Wheel LLC, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "main.hpp"
#pragma once
#include "interface/interface.hpp"
#include "magic/magic.hpp"


//Splitscreen support stuff.
extern int current_player; //This may not be necessary. Consider this: Each Player instance keeps track of whether it is a network player or a localhost player.

//TODO: Move these into each and every individual player.
extern Entity* selectedEntity[MAXPLAYERS];
extern Entity* lastSelectedEntity[MAXPLAYERS];
extern Sint32 mousex, mousey;
extern Sint32 omousex, omousey;
extern Sint32 mousexrel, mouseyrel;

/*
 * TODO: Will need to make messages work for each hotseat player.
 * This will probably involve taking the current notification_messages thing and instead including that in a wrapper or something that is owned by each player instance.
 * Basically, each player will need to keep track of its own messages.
 *
 * I believe one of the splitscreen layouts included a version where all of the messages were communal and were in the center of the screen or summat.
 */

extern bool splitscreen;

extern int gamepad_deadzone;
extern int gamepad_trigger_deadzone;
extern int gamepad_leftx_sensitivity;
extern int gamepad_lefty_sensitivity;
extern int gamepad_rightx_sensitivity;
extern int gamepad_righty_sensitivity;
extern int gamepad_menux_sensitivity;
extern int gamepad_menuy_sensitivity;

extern bool gamepad_leftx_invert;
extern bool gamepad_lefty_invert;
extern bool gamepad_rightx_invert;
extern bool gamepad_righty_invert;
extern bool gamepad_menux_invert;
extern bool gamepad_menuy_invert;

//Game Controller 1 handler
//TODO: Joystick support?
//extern SDL_GameController* game_controller;

class GameController
{
	SDL_GameController* sdl_device;
	int id;

	int oldLeftTrigger;
	int oldRightTrigger;
	std::string name;
public:
	GameController();
	~GameController();

	struct Binding_t {
		float analog = 0.f;
		float deadzone = 0.f;
		bool binary = false;
		bool consumed = false;

		enum Bindtype_t 
		{
			INVALID,
			KEYBOARD,
			CONTROLLER_AXIS,
			CONTROLLER_BUTTON,
			MOUSE_BUTTON,
			JOYSTICK_AXIS,
			JOYSTICK_BUTTON,
			JOYSTICK_HAT,
			//JOYSTICK_BALL,
			NUM
		};
		Bindtype_t type = INVALID;

		SDL_GameControllerAxis padAxis = SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_INVALID;
		SDL_GameControllerButton padButton = SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_INVALID;
		bool padAxisNegative = false;
	};

	void updateButtons();
	void updateAxis();
	static SDL_GameControllerButton getSDLButtonFromImpulse(const unsigned controllerImpulse);
	static SDL_GameControllerAxis getSDLTriggerFromImpulse(const unsigned controllerImpulse);

	Binding_t buttons[NUM_JOY_STATUS];
	Binding_t axis[NUM_JOY_AXIS_STATUS];

	bool binary(SDL_GameControllerButton binding) const;
	bool binaryToggle(SDL_GameControllerButton binding) const;
	void consumeBinaryToggle(SDL_GameControllerButton binding);
	float analog(SDL_GameControllerButton binding) const;
	bool binary(SDL_GameControllerAxis binding) const;
	bool binaryToggle(SDL_GameControllerAxis binding) const;
	void consumeBinaryToggle(SDL_GameControllerAxis binding);
	float analog(SDL_GameControllerAxis binding) const;


	//! converts the given input to a boolean/analog value
	bool binaryOf(Binding_t& binding);
	float analogOf(Binding_t& binding);


	//Closes the SDL device.
	void close();

	//Opens the SDL device.
	//If c < 0 or c >= SDL_NumJoysticks() or c is not a game controller, then returns false.
	bool open(int c);

	void initBindings();
	const int getID() { return id; }
	const SDL_GameController* getControllerDevice() { return sdl_device; }
	const bool isActive();

	/*
	 * Moves the player's head around.
	 * Handles the triggers.
	 */
	void handleAnalog(int player);

	//Left analog stick movement along the x axis.
	int getLeftXMove();
	//...along the y axis.
	int getLeftYMove();
	//Right analog stick movement along the x axis.
	int getRightXMove();
	//...along the y axis.
	int getRightYMove();

	int getLeftTrigger();
	int getRightTrigger();

	//The amount of movement of the given analog stick along its respective axis, with no gamepad sensitivity application. Deadzone is taken into account.
	int getRawLeftXMove();
	int getRawLeftYMove();
	int getRawRightXMove();
	int getRawRightYMove();

	int getRawLeftTrigger();
	int getRawRightTrigger();

	//Gets the percentage the given stick is pressed along its current axis. From 0% after the deadzone to 100% all the way to the edge of the analog stick.
	float getLeftXPercent();
	float getLeftYPercent();
	float getRightXPercent();
	float getRightYPercent();

	float getLeftTriggerPercent();
	float getRightTriggerPercent();

	//The maximum amount the given analog stick can move on its respective axis. After the gamepad deadzone is taken into account.
	int maxLeftXMove();
	int maxLeftYMove();
	int maxRightXMove();
	int maxRightYMove();

	int maxLeftTrigger();
	int maxRightTrigger();
	int oldRightX;
	int oldRightY;
	int oldAxisRightX;
	int oldAxisRightY;

	/*
	 * Uses dpad to move the cursor around the inventory and select items.
	 * Returns true if moved.
	 */
	bool handleInventoryMovement(const int player);

	/*
	 * Uses dpad to move the cursor around a chest's inventory and select items.
	 * Returns true if moved.
	 */
	bool handleChestMovement(const int player);

	/*
	 * Uses dpad to move the cursor around a shop's inventory and select items.
	 * Returns true if moved.
	 */
	bool handleShopMovement(const int player);

	/*
	 * Uses dpad to move the cursor around Identify GUI's inventory and select items.
	 * Returns true if moved.
	 */
	bool handleIdentifyMovement(const int player);

	/*
	 * Uses dpad to move the cursor around Remove Curse GUI's inventory and select items.
	 * Returns true if moved.
	 */
	bool handleRemoveCurseMovement(const int player);

	/*
	 * Uses dpad to move the cursor through the item context menu and select entries.
	 * Returns true if moved.
	 */
	bool handleItemContextMenu(const int player, const Item& item);

	/*
	* Uses dpad to move the cursor through the item context menu and select entries.
	* Returns true if moved.
	*/
	bool handleRepairGUIMovement(const int player);
};
const int MAX_GAME_CONTROLLERS = 16;
extern std::array<GameController, MAX_GAME_CONTROLLERS> game_controllers;

class Inputs
{
	int playerControllerIds[MAXPLAYERS];
	int playerUsingKeyboardControl = 0;

	class VirtualMouse
	{
	public:
		Sint32 xrel = 0; //mousexrel
		Sint32 yrel = 0; //mouseyrel
		Sint32 ox = 0; //omousex
		Sint32 oy = 0; //omousey
		Sint32 x = 0; //mousex
		Sint32 y = 0; //mousey
		bool draw_cursor = true; //True if the gamepad's d-pad has been used to navigate menus and such. //TODO: Off by default on consoles and the like.
		bool moved = false;
		bool lastMovementFromController = false;
		VirtualMouse() {};
		~VirtualMouse() {};

		void warpMouseInCamera(const view_t& camera, const Sint32 newx, const Sint32 newy)
		{
			x = std::max(camera.winx, std::min(camera.winx + camera.winw, x + newx));
			y = std::max(camera.winy, std::min(camera.winy + camera.winh, y + newy));
			xrel += newx;
			yrel += newy;
			moved = true;
		}
		void warpMouseInScreen(SDL_Window*& window, const Sint32 newx, const Sint32 newy)
		{
			int w, h;
			SDL_GetWindowSize(window, &w, &h);
			x = std::max(0, std::min(w, x + newx));
			y = std::max(0, std::min(h, y + newy));
			xrel += newx;
			yrel += newy;
			moved = true;
		}
	};
	VirtualMouse vmouse[MAXPLAYERS];

	class UIStatus
	{
	public:
		/*
		* So that the cursor jumps back to the hotbar instead of the inventory if a picked up hotbar item is canceled.
		* Its value indicates which hotbar slot it's from.
		* -1 means it's not from the hotbar.
		*/
		int selectedItemFromHotbar = -1;

		Item* selectedItem = nullptr;
		bool toggleclick = false;
		bool itemMenuOpen = false;
		int itemMenuX = 0;
		int itemMenuY = 0;
		int itemMenuSelected = 0;
		Uint32 itemMenuItem = 0;
	};
	UIStatus uiStatus[MAXPLAYERS];

public:
	Inputs() 
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			playerControllerIds[i] = -1;
		}
	};
	~Inputs() {};
	const void setPlayerIDAllowedKeyboard(const int player)
	{
		playerUsingKeyboardControl = player;
	}
	const bool bPlayerUsingKeyboardControl(const int player) const
	{
		if ( !splitscreen )
		{
			return player == clientnum;
		}
		return player == playerUsingKeyboardControl;
	}
	void controllerHandleMouse(const int player);
	const bool bControllerInputPressed(const int player, const unsigned controllerImpulse) const;
	void controllerClearInput(const int player, const unsigned controllerImpulse);
	const bool bMouseLeft (const int player) const;
	const bool bMouseRight(const int player) const;
	const void mouseClearLeft(int player);
	const void mouseClearRight(int player);
	void removeControllerWithDeviceID(const int id)
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			if ( playerControllerIds[i] == id )
			{
				playerControllerIds[i] = -1;
				printlog("[INPUTS]: Removed controller id %d from player index %d.", id, i);
			}
		}
	}
	VirtualMouse* getVirtualMouse(int player)
	{
		if ( player < 0 || player >= MAXPLAYERS )
		{
			printlog("[INPUTS]: Warning: player index %d out of range.", player);
			return nullptr;
		}
		return &vmouse[player];
	}
	UIStatus* getUIInteraction(int player)
	{
		if ( player < 0 || player >= MAXPLAYERS )
		{
			printlog("[INPUTS]: Warning: player index %d out of range.", player);
			return nullptr;
		}
		return &uiStatus[player];
	}
	enum MouseInputs
	{
		OX,
		OY,
		X,
		Y,
		XREL,
		YREL
	};
	const Sint32 getMouse(const int player, MouseInputs input);
	void setMouse(const int player, MouseInputs input, Sint32 value);
	void hideMouseCursors()
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			getVirtualMouse(i)->draw_cursor = false;
		}
	};
	enum MouseWarpFlags : Uint32
	{
		UNSET_RELATIVE_MOUSE = 1,
		SET_RELATIVE_MOUSE = 2,
		SET_MOUSE = 4,
		SET_CONTROLLER = 8
	};
	void warpMouse(const int player, const Sint32 x, const Sint32 y, Uint32 flags);
	const int getControllerID(int player) const
	{
		if ( player < 0 || player >= MAXPLAYERS )
		{
			printlog("[INPUTS]: Warning: player index %d out of range.", player);
			return -1;
		}
		return playerControllerIds[player];
	}
	GameController* getController(int player) const;

	const bool hasController(int player) const 
	{
		if ( player < 0 || player >= MAXPLAYERS )
		{
			printlog("[INPUTS]: Warning: player index %d out of range.", player);
			return false;
		}
		return playerControllerIds[player] != -1;
	}
	void setControllerID(int player, const int id) 
	{
		if ( player < 0 || player >= MAXPLAYERS )
		{
			printlog("[INPUTS]: Warning: player index %d out of range.", player);
		}
		playerControllerIds[player] = id;
	}
	void addControllerIDToNextAvailableInput(const int id)
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			if ( playerControllerIds[i] == -1 )
			{
				playerControllerIds[i] = id;
				printlog("[INPUTS]: Automatically assigned controller id %d to player index %d.", id, i);
				break;
			}
		}
	}
	const bool bPlayerIsControllable(int player) const;
	void updateAllMouse()
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			controllerHandleMouse(i);
		}
	}
	void updateAllOMouse()
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			if ( !bMouseLeft(i) )
			{
				vmouse[i].ox = vmouse[i].x;
				vmouse[i].oy = vmouse[i].y;
				vmouse[i].moved = false;
			}
		}
		/*messagePlayer(0, "x: %d | y: %d / x: %d | y: %d / x: %d | y: %d / x: %d | y: %d ", 
			vmouse[0].ox, vmouse[0].oy,
			vmouse[1].ox, vmouse[1].oy,
			vmouse[2].ox, vmouse[2].oy,
			vmouse[3].ox, vmouse[3].oy);*/
	}
	void updateAllRelMouse()
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			vmouse[i].xrel = 0;
			vmouse[i].yrel = 0;
		}
	}
};
extern Inputs inputs;
void initGameControllers();

static const unsigned NUM_HOTBAR_SLOTS = 10; //NOTE: If you change this, you must dive into drawstatus.c and update the hotbar code. It expects 10.
static const unsigned NUM_HOTBAR_ALTERNATES = 5;

class Player
{
	//Splitscreen support. Every player gets their own screen.
	//Except in multiplayer. In that case, this is just a big old dummy class.
	//SDL_Surface* screen;
	
	//Is this a hotseat player? If so, draw splitscreen and stuff. (Host player is automatically a hotseat player). If not, then this is a dummy container for the multiplayer client.
	bool local_host;
	view_t* cam;

	int playernum;

public:
	Entity* entity;
	bool bSplitscreen = false;
	Player(int playernum = 0, bool local_host = true);
	~Player();

	void init()
	{
		inventoryUI.resetInventory();
	};

	class Hotbar_t;
	Hotbar_t* hotbar;

	view_t& camera() const { return *cam; }
	const int camera_x1() const { return cam->winx; }
	const int camera_x2() const { return cam->winx + cam->winw; }
	const int camera_y1() const { return cam->winy; }
	const int camera_y2() const { return cam->winy + cam->winh; }
	const int camera_width() const { return cam->winw; }
	const int camera_height() const { return cam->winh; }
	const int camera_midx() const { return camera_x1() + camera_width() / 2; }
	const int camera_midy() const { return camera_y1() + camera_height() / 2; }
	const bool isLocalPlayer() const;
	const bool isLocalPlayerAlive() const;

	//All the code that sets shootmode = false. Display chests, inventory, books, shopkeeper, identify, whatever.
	void openStatusScreen(int whichGUIMode, int whichInventoryMode); //TODO: Make all the everything use this. //TODO: Make an accompanying closeStatusScreen() function.
	void closeAllGUIs(CloseGUIShootmode shootmodeAction, CloseGUIIgnore whatToClose);
	bool shootmode = false;
	int inventory_mode = INVENTORY_MODE_ITEM;
	int gui_mode = GUI_MODE_NONE;

	class Inventory_t
	{
		const int sizex = DEFAULT_INVENTORY_SIZEX;
		int sizey = DEFAULT_INVENTORY_SIZEY;
		const int starty = 10;
		Player& player;

		int selectedSlotX = 0;
		int selectedSlotY = 0;
	public:
		static const int DEFAULT_INVENTORY_SIZEX = 12;
		static const int DEFAULT_INVENTORY_SIZEY = 3;
		Inventory_t(Player& p) : player(p) {};
		~Inventory_t() {};
		const int getTotalSize() const { return sizex * sizey; }
		const int getSizeX() const { return sizex; }
		const int getSizeY() const { return sizey; }
		const int getStartX() const {
			return (player.camera_midx() - (sizex) * (getSlotSize()) / 2 - inventory_mode_item_img->w / 2);
		}
		const int getStartY() const { return player.camera_y1() + starty; }
		const int getSlotSize() const { return static_cast<int>(40 * uiscale_inventory); }
		void setSizeY(int size) { sizey = size; }
		void selectSlot(const int x, const int y) { selectedSlotX = x; selectedSlotY = y; }
		const int getSelectedSlotX() const { return selectedSlotX; }
		const int getSelectedSlotY() const { return selectedSlotY; }
		void resetInventory()
		{
			sizey = DEFAULT_INVENTORY_SIZEY;
		}
	} inventoryUI;

	class StatusBar_t
	{
		Player& player;
	public:
		StatusBar_t(Player& p) : player(p)
		{};
		~StatusBar_t() {};

		const int getStartX() const 
		{ 
			return (player.camera_midx() - status_bmp->w * uiscale_chatlog / 2);
		}
		const int getStartY() const
		{
			return (player.camera_y2() - getOffsetY());
		}
		const int getOffsetY() const { return (status_bmp->h * uiscale_chatlog * (hide_statusbar ? 0 : 1)); }
	} statusBarUI;

	class HUD_t
	{
		Player& player;
	public:
		Entity* weapon = nullptr;
		Entity* arm = nullptr;
		Entity* magicLeftHand = nullptr;
		Entity* magicRightHand = nullptr;

		bool weaponSwitch = false;
		bool shieldSwitch = false;

		Sint32 throwGimpTimer = 0; // player cannot throw objects unless zero
		Sint32 pickaxeGimpTimer = 0; // player cannot swap weapons immediately after using pickaxe 
									 // due to multiplayer weapon degrade lag... equipping new weapon before degrade
									 // message hits can degrade the wrong weapon.
		Sint32 swapWeaponGimpTimer = 0; // player cannot swap weapons unless zero
		Sint32 bowGimpTimer = 0; // can't draw bow unless zero.

		bool bowFire = false;
		bool bowIsBeingDrawn = false;
		Uint32 bowStartDrawingTick = 0;
		Uint32 bowDrawBaseTicks = 50;
#ifdef USE_FMOD
		FMOD_CHANNEL* bowDrawingSoundChannel = NULL;
		FMOD_BOOL bowDrawingSoundPlaying = 0;
#elif defined USE_OPENAL
		OPENAL_SOUND* bowDrawingSoundChannel = NULL;
		ALboolean bowDrawingSoundPlaying = 0;
#endif
		HUD_t(Player& p) : player(p)
		{};
		~HUD_t() {};

		void reset()
		{
			swapWeaponGimpTimer = 0;
			bowGimpTimer = 0;
			throwGimpTimer = 0;
			pickaxeGimpTimer = 0;
			bowFire = false;
			bowIsBeingDrawn = false;
			bowStartDrawingTick = 0;
			bowDrawBaseTicks = 50;
			weaponSwitch = false;
			shieldSwitch = false;
		}
	} hud;

	class Magic_t
	{
		Player& player;
		spell_t* selected_spell = nullptr; //The spell the player has currently selected.
	public:
		spell_t* selected_spell_alternate[NUM_HOTBAR_ALTERNATES] = { nullptr, nullptr, nullptr, nullptr, nullptr };
		int selected_spell_last_appearance = -1;
		list_t spellList; //All of the player's spells are stored here.

		Magic_t(Player& p) : player(p)
		{
			spellList.first = nullptr;
			spellList.last = nullptr;
		};
		~Magic_t() {};
		void clearSelectedSpells()
		{
			selected_spell = nullptr;
			for ( int c = 0; c < NUM_HOTBAR_ALTERNATES; ++c )
			{
				selected_spell_alternate[c] = nullptr;
			}
			selected_spell_last_appearance = -1;
		}
		void equipSpell(spell_t* spell) { selected_spell = spell; }
		spell_t* selectedSpell() const { return selected_spell; }

	} magic;
};

class Player::Hotbar_t {
	std::array<hotbar_slot_t, NUM_HOTBAR_SLOTS> hotbar;
	std::array<std::array<hotbar_slot_t, NUM_HOTBAR_SLOTS>, NUM_HOTBAR_ALTERNATES> hotbar_alternate;
	Player& player;
public:
	int current_hotbar = 0;
	bool hotbarShapeshiftInit[NUM_HOTBAR_ALTERNATES] = { false, false, false, false, false };
	int swapHotbarOnShapeshift = 0;
	bool hotbarHasFocus = false;
	int magicBoomerangHotbarSlot = -1;
	Uint32 hotbarTooltipLastGameTick = 0;

	const int getStartX() const
	{
		return (player.camera_midx() - ((NUM_HOTBAR_SLOTS / 2) * getSlotSize()));
	}
	const int getSlotSize() const { return hotbar_img->w * uiscale_hotbar; }

	Player::Hotbar_t(Player& p) : player(p)
	{
		clear();
	}

	enum HotbarLoadouts : int
	{
		HOTBAR_DEFAULT,
		HOTBAR_RAT,
		HOTBAR_SPIDER,
		HOTBAR_TROLL,
		HOTBAR_IMP
	};

	void clear()
	{
		swapHotbarOnShapeshift = 0;
		current_hotbar = 0;
		hotbarHasFocus = false;
		magicBoomerangHotbarSlot = -1;
		hotbarTooltipLastGameTick = 0;
		for ( int j = 0; j < NUM_HOTBAR_ALTERNATES; ++j )
		{
			hotbarShapeshiftInit[j] = false;
		}
		for ( int i = 0; i < NUM_HOTBAR_SLOTS; ++i )
		{
			hotbar[i].item = 0;
			for ( int j = 0; j < NUM_HOTBAR_ALTERNATES; ++j )
			{
				hotbar_alternate[j][i].item = 0;
			}
		}
	}

	auto& slots() { return hotbar; };
	auto& slotsAlternate(int alternate) { return hotbar_alternate[alternate]; };
	auto& slotsAlternate() { return hotbar_alternate;  }
	void selectHotbarSlot(int slot)
	{
		if ( slot < 0 )
		{
			slot = NUM_HOTBAR_SLOTS - 1;
		}
		if ( slot >= NUM_HOTBAR_SLOTS )
		{
			slot = 0;
		}
		current_hotbar = slot;
		hotbarHasFocus = true;
	}
};

void initIdentifyGUIControllerCode();
void initRemoveCurseGUIControllerCode();

extern Player* players[MAXPLAYERS];
//In the process of switching from the old entity player array, all of the old uses of player need to be hunted down and then corrected to account for the new array.
//So, search for the comment:
//TODO: PLAYERSWAP
//and fix and verify that the information is correct.
//Then, once all have been fixed and verified, uncomment this declaration, and the accompanying definition in player.cpp; uncomment all of the TODO: PLAYERSWAP code segments, and attempt compilation and running.
