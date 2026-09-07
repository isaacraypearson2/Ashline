#include "Campaign/AshlineMissionCatalog.h"
#include <initializer_list>

namespace AshlineCampaign
{
	static FAshlineMissionObjective Obj(const TCHAR* Id, const TCHAR* Name, const TCHAR* Desc, bool bOptional = false)
	{
		FAshlineMissionObjective O;
		O.ObjectiveId = Id;
		O.DisplayName = FText::FromString(Name);
		O.Description = FText::FromString(Desc);
		O.bOptional = bOptional;
		return O;
	}

	static FAshlineMissionDefinition Make(
		EAshlineMissionId Id,
		const TCHAR* Code,
		const TCHAR* Title,
		const TCHAR* Theme,
		const TCHAR* Location,
		const TCHAR* Map,
		const TCHAR* Briefing,
		int32 Level,
		int32 XP,
		int32 Crates,
		std::initializer_list<FAshlineMissionObjective> Objectives)
	{
		FAshlineMissionDefinition M;
		M.MissionId = Id;
		M.CodeName = Code;
		M.Title = FText::FromString(Title);
		M.Theme = FText::FromString(Theme);
		M.Location = FText::FromString(Location);
		M.MapAsset = Map;
		M.Briefing = FText::FromString(Briefing);
		M.RecommendedOperatorLevel = Level;
		M.XPReward = XP;
		M.CrateTokensReward = Crates;
		for (const FAshlineMissionObjective& O : Objectives)
		{
			M.Objectives.Add(O);
		}
		return M;
	}
}

TArray<FAshlineMissionDefinition> UAshlineMissionCatalog::BuildCampaign()
{
	using namespace AshlineCampaign;

	TArray<FAshlineMissionDefinition> Campaign;
	Campaign.Reserve(12);

	Campaign.Add(Make(
		EAshlineMissionId::ASH_01_WireCut,
		TEXT("ASH-01"),
		TEXT("Wire Cut"),
		TEXT("Night raid / comms sabotage"),
		TEXT("Border relay, eastern highlands"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH01_WireCut"),
		TEXT("A border relay is feeding fire missions onto civilian roads. Infiltrate the compound, cut the trunk fiber, and extract before the QRF seals the valley. Keep the shot count low — the hill has ears."),
		1, 400, 1,
		{
			Obj(TEXT("INFIL"), TEXT("Infiltrate the relay"), TEXT("Reach the compound without raising a general alarm.")),
			Obj(TEXT("CUT"), TEXT("Sever the trunk fiber"), TEXT("Destroy or disconnect the primary uplink in the comms hut.")),
			Obj(TEXT("EXFIL"), TEXT("Extract west"), TEXT("Reach the dry creek exfil before the QRF arrives.")),
			Obj(TEXT("GHOST"), TEXT("Ghost protocol"), TEXT("Complete the raid without a full alert."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_02_DustMarket,
		TEXT("ASH-02"),
		TEXT("Dust Market"),
		TEXT("Urban recon / HVT intercept"),
		TEXT("Old quarter bazaar, coastal city"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH02_DustMarket"),
		TEXT("A broker is moving targeting packets through the dust market. Confirm the HVT, lift the case, and do not turn the bazaar into a massacre. Local police will collapse on gunfire."),
		3, 500, 1,
		{
			Obj(TEXT("CONFIRM"), TEXT("Confirm the broker"), TEXT("Identify the HVT from the safe-house balcony.")),
			Obj(TEXT("CASE"), TEXT("Secure the case"), TEXT("Recover the targeting packet case."), false),
			Obj(TEXT("EXTRACT"), TEXT("Van extract"), TEXT("Reach the covered alley extract with the case.")),
			Obj(TEXT("CIV"), TEXT("No civilian casualties"), TEXT("Keep the market standing."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_03_Holdfast,
		TEXT("ASH-03"),
		TEXT("Holdfast"),
		TEXT("Firebase defense / last stand"),
		TEXT("Forward operating post, river fork"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH03_Holdfast"),
		TEXT("The river post is about to be overrun. Hold the berm, keep the mortar pit alive, and wait for the relief column. If the radio dies, so does everyone behind you."),
		5, 650, 2,
		{
			Obj(TEXT("BERM"), TEXT("Hold the berm"), TEXT("Repel the first two waves at the sandbag line.")),
			Obj(TEXT("MORTAR"), TEXT("Keep the pit online"), TEXT("Prevent the mortar crew from being wiped.")),
			Obj(TEXT("RELIEF"), TEXT("Link with relief"), TEXT("Survive until the column reaches the gate.")),
			Obj(TEXT("NOFALL"), TEXT("No sector collapse"), TEXT("Do not lose any defensive sector."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_04_NightGlass,
		TEXT("ASH-04"),
		TEXT("Night Glass"),
		TEXT("Low-vis overwatch / counter-sniper"),
		TEXT("Glassworks ridge, industrial outskirts"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH04_NightGlass"),
		TEXT("An enemy marksman cell is glassing the approach to the harbor. Take the ridge at night, clear the nests, and own the glass before dawn. Thermals will lie in the kiln heat."),
		6, 700, 1,
		{
			Obj(TEXT("NEST_A"), TEXT("Clear nest Alpha"), TEXT("Eliminate the west kiln overwatch.")),
			Obj(TEXT("NEST_B"), TEXT("Clear nest Bravo"), TEXT("Eliminate the water-tower team.")),
			Obj(TEXT("OWN"), TEXT("Own the glass"), TEXT("Hold the ridge until the recon drone confirms all-clear.")),
			Obj(TEXT("SILENT"), TEXT("No unsuppressed fire"), TEXT("Stay on suppressed weapons."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_05_ConvoyGhost,
		TEXT("ASH-05"),
		TEXT("Convoy Ghost"),
		TEXT("Ambush / mobile assault"),
		TEXT("Highway 9, salt flats"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH05_ConvoyGhost"),
		TEXT("A fuel and munitions convoy is running dark across the flats. Disable the lead, burn the tankers, and vanish before air cover arrives. Do not let the command wagon escape."),
		8, 800, 2,
		{
			Obj(TEXT("LEAD"), TEXT("Kill the lead vehicle"), TEXT("Stop the convoy at the cut.")),
			Obj(TEXT("FUEL"), TEXT("Destroy the tankers"), TEXT("Burn or detonate all fuel carriers.")),
			Obj(TEXT("CMD"), TEXT("Stop the command wagon"), TEXT("Prevent the command vehicle from fleeing south.")),
			Obj(TEXT("GHOST"), TEXT("Ghost extract"), TEXT("Leave the flats before the gunship window."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_06_AshHarbor,
		TEXT("ASH-06"),
		TEXT("Ash Harbor"),
		TEXT("Port assault / shipboard"),
		TEXT("Municipal docks, Ash Harbor"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH06_AshHarbor"),
		TEXT("The harbor is the pipeline. Seize the crane control, board the marked freighter, and recover the sealed cargo. Naval militia will fight room-to-room in the containers."),
		10, 900, 2,
		{
			Obj(TEXT("CRANE"), TEXT("Seize crane control"), TEXT("Take the dock tower and lock the cranes.")),
			Obj(TEXT("BOARD"), TEXT("Board the freighter"), TEXT("Reach cargo hold 3 on the marked ship.")),
			Obj(TEXT("CARGO"), TEXT("Recover sealed cargo"), TEXT("Secure the ashline canisters.")),
			Obj(TEXT("HOSTAGE"), TEXT("Free the dock crew"), TEXT("Release the held civilian stevedores."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_07_Whiteout,
		TEXT("ASH-07"),
		TEXT("Whiteout"),
		TEXT("Arctic storm / navigation"),
		TEXT("High plateau, winter front"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH07_Whiteout"),
		TEXT("A listening post went silent in a whiteout. Follow the wire, restore the beacon, and get the crew out. Visibility will collapse — trust the compass and the thermal, not the horizon."),
		11, 950, 2,
		{
			Obj(TEXT("WIRE"), TEXT("Follow the wire"), TEXT("Trace the buried line to the listening post.")),
			Obj(TEXT("BEACON"), TEXT("Restore the beacon"), TEXT("Bring the nav beacon back online.")),
			Obj(TEXT("CREW"), TEXT("Extract the crew"), TEXT("Evacuate surviving operators to the sled LZ.")),
			Obj(TEXT("STORM"), TEXT("No lost operators"), TEXT("Keep the whole crew alive in the storm."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_08_Catacomb,
		TEXT("ASH-08"),
		TEXT("Catacomb"),
		TEXT("Subterranean CQB / intel"),
		TEXT("Old metro and ossuary, capital"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH08_Catacomb"),
		TEXT("The enemy moved a server farm under the city. Push the catacombs, dump the racks, and collapse the tunnel behind you. Flash and frag discipline — friendlies are stacked in the dark."),
		13, 1100, 2,
		{
			Obj(TEXT("PUSH"), TEXT("Push the metro"), TEXT("Clear the first three platforms.")),
			Obj(TEXT("DUMP"), TEXT("Dump the racks"), TEXT("Copy and destroy the subterranean servers.")),
			Obj(TEXT("COLLAPSE"), TEXT("Collapse the tunnel"), TEXT("Blow the marked supports and extract up-shaft.")),
			Obj(TEXT("INTEL"), TEXT("Full dump"), TEXT("Recover every marked drive."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_09_RidgeWire,
		TEXT("ASH-09"),
		TEXT("Ridge Wire"),
		TEXT("Mountain EW / overwatch"),
		TEXT("Signal ridge, western range"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH09_RidgeWire"),
		TEXT("Someone is spoofing our net from the ridge. Climb, plant the counter-wire, and hold until the spoof dies. Expect drones and a marksman pair on the opposite spur."),
		14, 1200, 2,
		{
			Obj(TEXT("CLIMB"), TEXT("Reach the mast"), TEXT("Climb the ridge to the spoof mast.")),
			Obj(TEXT("PLANT"), TEXT("Plant the counter-wire"), TEXT("Install the EW package on the mast.")),
			Obj(TEXT("HOLD"), TEXT("Hold the ridge"), TEXT("Defend until the spoof collapses.")),
			Obj(TEXT("DRONE"), TEXT("Kill the drone nest"), TEXT("Destroy the UAV launch crate."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_10_FalseFlag,
		TEXT("ASH-10"),
		TEXT("False Flag"),
		TEXT("Deception / urban night"),
		TEXT("Broadcast campus, midtown"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH10_FalseFlag"),
		TEXT("A broadcast is about to pin the last three raids on a unit that was never there. Intercept the feed, swap the packet, and walk out as if you belonged. If the floor goes loud, the lie becomes true."),
		16, 1300, 3,
		{
			Obj(TEXT("INFIL"), TEXT("Enter as staff"), TEXT("Reach the control floor without a weapons alarm.")),
			Obj(TEXT("SWAP"), TEXT("Swap the packet"), TEXT("Replace the false-flag reel with the true intercept.")),
			Obj(TEXT("WALK"), TEXT("Walk out"), TEXT("Exfil through the loading dock before the rewind.")),
			Obj(TEXT("CLEAN"), TEXT("No bodies on camera"), TEXT("Do not leave casualties in studio sightlines."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_11_LastTrain,
		TEXT("ASH-11"),
		TEXT("Last Train"),
		TEXT("Rail extract / running fight"),
		TEXT("Freight corridor, industrial belt"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH11_LastTrain"),
		TEXT("The last civilian train is leaving the belt with witnesses and a drive we cannot lose. Clear the platform, ride the consist, and keep the engine alive through the yards. They will board car by car."),
		17, 1400, 3,
		{
			Obj(TEXT("PLATFORM"), TEXT("Clear the platform"), TEXT("Secure the departure platform and board.")),
			Obj(TEXT("RIDE"), TEXT("Hold the consist"), TEXT("Repel boarders across the freight cars.")),
			Obj(TEXT("ENGINE"), TEXT("Keep the engine alive"), TEXT("Protect the locomotive to the river bridge.")),
			Obj(TEXT("WITNESS"), TEXT("Protect the witnesses"), TEXT("No civilian deaths on the train."), true)
		}));

	Campaign.Add(Make(
		EAshlineMissionId::ASH_12_Ashline,
		TEXT("ASH-12"),
		TEXT("Ashline"),
		TEXT("Finale / decisive strike"),
		TEXT("Ashline terminus, buried facility"),
		TEXT("/Game/Ashline/Maps/Campaign/ASH12_Ashline"),
		TEXT("The ashline is not a road. It is a buried command spine. Cut it at the terminus, collapse the facility, and end the campaign that started with a single wire. There is no second train."),
		18, 2000, 4,
		{
			Obj(TEXT("BREACH"), TEXT("Breach the terminus"), TEXT("Force the buried gate and enter the spine.")),
			Obj(TEXT("CUT"), TEXT("Cut the ashline"), TEXT("Destroy the primary command nodes.")),
			Obj(TEXT("COLLAPSE"), TEXT("Collapse the facility"), TEXT("Arm the structural charges and reach the lift.")),
			Obj(TEXT("END"), TEXT("No one walks the line"), TEXT("Eliminate the remaining command element."), true)
		}));

	return Campaign;
}

bool UAshlineMissionCatalog::FindMission(EAshlineMissionId MissionId, FAshlineMissionDefinition& OutMission)
{
	for (const FAshlineMissionDefinition& Mission : BuildCampaign())
	{
		if (Mission.MissionId == MissionId)
		{
			OutMission = Mission;
			return true;
		}
	}
	return false;
}

EAshlineMissionId UAshlineMissionCatalog::NextMission(EAshlineMissionId Current)
{
	const int32 Index = MissionIndex(Current);
	if (Index < 0 || Index >= 11)
	{
		return EAshlineMissionId::None;
	}
	return static_cast<EAshlineMissionId>(static_cast<uint8>(EAshlineMissionId::ASH_01_WireCut) + Index + 1);
}

int32 UAshlineMissionCatalog::MissionIndex(EAshlineMissionId MissionId)
{
	if (MissionId == EAshlineMissionId::None)
	{
		return -1;
	}
	return static_cast<int32>(MissionId) - static_cast<int32>(EAshlineMissionId::ASH_01_WireCut);
}
