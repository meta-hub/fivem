#include <StdInc.h>
#include <Hooking.h>

#include <fiDevice.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <MinHook.h>

static std::map<std::tuple<std::string, std::string>, rage::fiDeviceRelative*> g_pathsToDevices;
static std::unordered_map<std::string, size_t> g_nameRefCounts;

static const std::string_view g_basePacks[] =
{
	"ANIMALS",
	"ANIMALS_FAR",
	"ANIMALS_NEAR",
	"CUTSCENE_MASTERED_ONLY",
	"DLC_GTAO",
	"INTERACTIVE_MUSIC",
	"ONESHOT_AMBIENCE",
	"PAIN",
	"POLICE_SCANNER",
	"PROLOGUE",
	"RADIO_01_CLASS_ROCK",
	"RADIO_02_POP",
	"RADIO_03_HIPHOP_NEW",
	"RADIO_04_PUNK",
	"RADIO_05_TALK_01",
	"RADIO_06_COUNTRY",
	"RADIO_07_DANCE_01",
	"RADIO_08_MEXICAN",
	"RADIO_09_HIPHOP_OLD",
	"RADIO_11_TALK_02",
	"RADIO_12_REGGAE",
	"RADIO_13_JAZZ",
	"RADIO_14_DANCE_02",
	"RADIO_15_MOTOWN",
	"RADIO_16_SILVERLAKE",
	"RADIO_17_FUNK",
	"RADIO_18_90S_ROCK",
	"RADIO_ADVERTS",
	"RADIO_NEWS",
	"RESIDENT",
	"SCRIPT",
	"SS_AC",
	"SS_DE",
	"SS_FF",
	"SS_GM",
	"SS_NP",
	"SS_QR",
	"SS_ST",
	"SS_UZ",
	"STREAMED_AMBIENCE",
	"STREAMED_VEHICLES",
	"STREAMED_VEHICLES_GRANULAR",
	"STREAMED_VEHICLES_GRANULAR_NPC",
	"STREAMED_VEHICLES_LOW_LATENCY",
	"STREAMS",
	"S_FULL_AMB_F",
	"S_FULL_AMB_M",
	"S_FULL_GAN",
	"S_FULL_SER",
	"S_MINI_AMB",
	"S_MINI_GAN",
	"S_MINI_SER",
	"S_MISC",
	"WEAPONS_PLAYER",
};

static void (*g_origAddWavePack)(const char* name, const char* path);

static void AddWavePack(const char* name, const char* path)
{
	auto wavePath = fmt::sprintf("awc:/%s/", name);
	
	auto addPath = [](const char* path, const char* mountAt)
	{
		if (g_pathsToDevices.find({ path, mountAt }) == g_pathsToDevices.end())
		{
			auto device = new rage::fiDeviceRelative();
			device->SetPath(path, true);
			device->Mount(mountAt);

			g_pathsToDevices.insert({ { path, mountAt }, device });
		}
	};

	addPath(path, wavePath.c_str());

	if (g_nameRefCounts[name] == 0)
	{
		// add any pack that would match an original pack (as 8-character limit)
		// as audio:/sfx/ would not match a packfile subdevice, we add the actual packfile mounts for each as a submount here
		for (auto pack : g_basePacks)
		{
			if (boost::algorithm::iequals(pack.substr(0, std::min(size_t(8), pack.length())), name))
			{
				addPath(va("audio:/sfx/%s/", pack), va("%s%s/", wavePath, pack));
			}
		}

		g_origAddWavePack(name, wavePath.c_str());
	}

	g_nameRefCounts[name]++;
}

static thread_local std::string g_lastWavePackPath;

static void (*g_origRemoveWavePack)(const char* name);

static void RemoveWavePack(const char* name)
{
	auto wavePath = fmt::sprintf("awc:/%s/", name);

	if (auto it = g_pathsToDevices.find({ g_lastWavePackPath, wavePath }); it != g_pathsToDevices.end())
	{
		rage::fiDevice::Unmount(*it->second);
		g_pathsToDevices.erase(it);
	}

	// remove underlying wavepack, if needed
	g_nameRefCounts[name]--;

	if (g_nameRefCounts[name] == 0)
	{
		g_origRemoveWavePack(name);
	}
}

static bool (*g_origUnmountWavePack)(void* self, const void* entry);

static bool UnmountWavePack(void* self, const char* entry)
{
	// memorize this for RemoveWavePack to use
	g_lastWavePackPath = entry;

	return g_origUnmountWavePack(self, entry);
}

static HookFunction hookFunction([]
{
	{
		auto location = hook::get_pattern("C6 04 02 00 48 8D 4C 24 20 48 8D", 14);
		hook::set_call(&g_origAddWavePack, location);
		hook::call(location, AddWavePack);
	}

	MH_Initialize();

	{
		auto location = hook::get_pattern<char>("48 8D 4C 24 20 C6 44 24 28 00 E8", 10);
		MH_CreateHook(location - 0x3F, UnmountWavePack, (void**)&g_origUnmountWavePack);
		hook::set_call(&g_origRemoveWavePack, location);
		hook::call(location, RemoveWavePack);
	}

	MH_EnableHook(MH_ALL_HOOKS);
});
