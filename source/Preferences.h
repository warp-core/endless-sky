/* Preferences.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef PREFERENCES_H_
#define PREFERENCES_H_

#include <cstdint>
#include <string>
#include <vector>



class Preferences {
public:
	enum class AlertIndicator : int_fast8_t {
		NONE = 0,
		AUDIO,
		VISUAL,
		BOTH
	};

	enum class AutoAim : int_fast8_t {
		OFF = 0,
		ALWAYS_ON,
		WHEN_FIRING
	};

	enum class AutoFire : int_fast8_t {
		OFF = 0,
		ON,
		GUNS_ONLY,
		TURRETS_ONLY
	};

	enum class BackgroundParallax : int_fast8_t {
		OFF = 0,
		FANCY,
		FAST
	};

	enum class BoardingPriority : int_fast8_t {
		PROXIMITY = 0,
		VALUE,
		MIXED
	};

	enum class DateFormat : int_fast8_t {
		DMY = 0, // Day-first format. (Sat, 4 Oct 1941)
		MDY,     // Month-first format. (Sat, Oct 4, 1941)
		YMD      // All-numeric ISO 8601. (1941-10-04)
	};

	enum class ExtendedJumpEffects : int_fast8_t {
		OFF = 0,
		MEDIUM,
		HEAVY
	};

	enum class FlotsamCollection : int_fast8_t {
		OFF = 0,
		ON,
		FLAGSHIP,
		ESCORT
	};

	enum class OverlayState : int_fast8_t {
		OFF = 0,
		ON,
		DAMAGED,
		DISABLED,
		ON_HIT,
	};

	enum class OverlayType : int_fast8_t {
		ALL = 0,
		FLAGSHIP,
		ESCORT,
		ENEMY,
		NEUTRAL
	};

	enum class VSync : int_fast8_t {
		off = 0,
		on,
		adaptive,
	};


public:
	static void Load();
	static void Save();

	static bool Has(const std::string &name);
	static void Set(const std::string &name, bool on = true);

	static void ToggleSpecial(const std::string &name);
	static const std::string &GetSpecialString(const std::string &name);

	static AlertIndicator GetAlertIndicator();

	static AutoAim GetAutoAim();

	static AutoFire GetAutoFire();

	static BackgroundParallax GetBackgroundParallax();

	static BoardingPriority GetBoardingPriority();

	static DateFormat GetDateFormat();

	static ExtendedJumpEffects GetExtendedJumpEffects();

	static FlotsamCollection GetFlotsamCollection();

	// Toggle the ammo usage preferences, cycling between "never," "frugally,"
	// and "always."
	static void ToggleAmmoUsage();
	static std::string AmmoUsage();

	// Scroll speed preference.
	static int ScrollSpeed();
	static void SetScrollSpeed(int speed);

	// View zoom.
	static double ViewZoom();
	static bool ZoomViewIn();
	static bool ZoomViewOut();
	static double MinViewZoom();
	static double MaxViewZoom();
	static const std::vector<double> &Zooms();

	static void ToggleScreenMode();
	static const std::string &ScreenModeSetting();

	// VSync setting, either "on", "off", or "adaptive".
	static bool ToggleVSync();
	static VSync VSyncState();
	static const std::string &VSyncSetting();

	static void CycleStatusOverlays(OverlayType type);
	static OverlayState StatusOverlaysState(OverlayType type);
	static const std::string &StatusOverlaysSetting(OverlayType type);

	// Red alert siren and symbol.
	static bool PlayAudioAlert();
	static bool DisplayVisualAlert();
	static bool DoAlertHelper(AlertIndicator toDo);

	static int GetPreviousSaveCount();
};



#endif
