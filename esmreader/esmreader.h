#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>

//**************** DATA STRUCTS *************************************

//used by TES3 records
struct Hedr {
 	float version;
 	long int fileType; //(0=esp, 1=esm, 32=ess)
 	char companyName[32];
 	char description[256];
 	long int numRecords; //48227
};

//Game settings records. the first letter of "name" tells what
//type of data it is.
struct GMST{
	std::string name = "";
	std::string stringValue = "";
	long int intValue = 0;
	float floatValue = 0.0;
};

std::vector<GMST> vgmst; //vector of game settings

//Global values records. type = short,long,float = s,l,f
struct GLOB{
	std::string name = "";
	std::string type = "";
	float value = 0.0;
};

std::vector<GLOB> vglob; //vector of global values

//used by CLAS records
struct ClassData {
	long int attributeID1 = 0;  //can be uint32[2] 
	long int attributeID2 = 0;
							/* 0 = strength
							1 = intelligence
							2 = willpower
							3 = agility
							4 = speed
							5 = endurance
							6 = personality
							7 = luck
							*/
	long int specialization; //0 = combat, 1 = magic, 2 = stealth
	long int minorID1 = 0;  //can be uint32[5][2]
	long int majorID1 = 0;
	long int minorID2 = 0;
	long int majorID2 = 0;
	long int minorID3 = 0;
	long int majorID3 = 0;
	long int minorID4 = 0;
	long int majorID4= 0;
	long int minorID5 = 0;
	long int majorID5 = 0;
	long int flags = 0; //0x0001 = playable
	long int autoCalcFlags = 0;
								/*0x00001 = Weapon
								0x00002 = Armor
								0x00004 = Clothing
								0x00008 = Books
								0x00010 = Ingrediant
								0x00020 = Picks
								0x00040 = Probes
								0x00080 = Lights
								0x00100 = Apparatus
								0x00200 = Repair
								0x00400 = Misc
								0x00800 = Spells
								0x01000 = Magic Items
								0x02000 = Potions
								0x04000 = Training
								0x08000 = Spellmaking
								0x10000 = Enchanting
								0x20000 = Repair Item*/
};

//class records
struct CLAS{
	std::string name = "";
	std::string fullName = "";
	ClassData cd;
	std::string description = "";
};

std::vector<CLAS> vclas;  //vector of character classes

//used by FACT records
struct RankData {
	long attribute1;
	long attribute2;
	long firstSkill;
	long secondSkill;
	long faction;
};

struct FactionData {
	long int attributeID1 = 0;
	long int attributeID2 = 0;
	RankData rd[10];
	long skillId[6];
	long unknown1;
	long flags; //1 = hidden from player
};

//faction records
struct FACT {
	std::string name = "";
	std::string fullName = "";
	std::vector<std::string> rankName;
	FactionData fd;
	std::vector< std::pair<std::string, long int> > factionReaction;
};

std::vector<FACT> vfact;

//used by RACE records
struct SkillBonuses {
	long int sillID;
	long int bonus;
};

struct RaceData {
	SkillBonuses sb[7];
	long int strength[2]; //male/female
	long int intelligence[2];
	long int willpower[2];
	long int agility[2];
	long int speed[2];
	long int endurance[2];
	long int personality[2];
	long int luck[2];
	float height[2];
	float weight[2];
	long flags; //1 = playable, 2 = beast race
};

//race records
struct RACE{
	std::string name = "";
	std::string fullName = "";
	RaceData rd;
	std::vector<std::string> special;  //special power/ability
	std::string description = "";
};

std::vector<RACE> vrace;

//used by SOUN records
struct SoundData {
	char volume; //(0 = 0.00, 255 = 1.00)
	char minRange;
	char maxRange;
};

//sound records
struct SOUN{
	std::string name = "";
	std::string fullName = "";
	SoundData sd;
};

std::vector<SOUN> vsoun;

//used by SKIL records
struct SkillData {
 	long int attribute;
 	long int specialization; //0 = combat, 1 = magic, 2 = stealth
 	float useValue[4]; //the use types for each skill are hard-coded.
};

//skill records
struct SKIL {
	long int index = 0;
	SkillData sd;
	std::string description = "";
};

std::vector<SKIL> vskil;


//used by MGEF records
struct MagicEffectData {
 	long int spellSchool; /*0 = alteration
 						1 = conjuration
 						2 = destruction
 						3 = illosion
 						4 = mysticism
 						5 = restoration*/
 	float baseCost;
 	long int flags; //0x0200 = spellmaking, 0x0400 = enchanting, 0x0800 = negative
 	long int red;
 	long int blue;
 	long int green;
 	float speedX;
 	float SizeX;
 	float sizeCap;
};

//magic effect records
struct MGEF {
	long int index = 0;
	MagicEffectData md;
	std::string iconTexture = "";
	std::string particleTexture = "";
	std::string castingVisual = "";
	std::string boltVisual = "";
	std::string hitVisual = "";
	std::string areaVisual = "";
	std::string description = "";
	std::string castSound = "";
	std::string boltSound = "";
	std::string hitSound = "";
	std::string areaSound = "";
};

std::vector<MGEF> vmgef;

//used by SCPT records
struct ScriptHeader {
 	char name[32];
 	long int numShorts;
 	long int numLongs;
 	long int numFloats;
 	long int scriptDataSize;
 	long int localVarSize;
};

//script records
struct SCPT {
	ScriptHeader sh;
	std::vector<std::string> localVars;
	std::vector<char> compiledScpt;
	std::string textScpt;
};

std::vector<SCPT> vscpt;

//used by REGN registers
struct WeatherData {
	uint8_t clear, cloudy, foggy, overcast, rain, thunder, ash, blight;
	//version 1.3 adds snow and blizzard
};

struct MapColor {
	char red, green, blue, null;
};

struct SoundRecord {
	char soundName[32];
	uint8_t chance;
};

//region records
struct REGN{
	std::string name = "";
	std::string fullName = "";
	WeatherData wd;
	std::string sleepCreature = "";
	MapColor mc;
	std::vector<SoundRecord> sounds;
};

std::vector<REGN> vregn;

//birth sign records
struct BSGN{
	std::string name = "";
	std::string fullName = "";
	std::string textureFileName = "";
	std::string description = "";
	std::vector<std::string> spell_ability;
};

std::vector<BSGN> vbsgn;

//landscape textures records
struct LTEX {
	std::string name = "";
	long int index = 0;
	std::string filename = "";
};

std::vector<LTEX> vltex;

//static records
struct STAT {
	std::string name = "";
	std::string model = "";
};

std::vector<STAT> vstat;

//door definition records
struct DOOR {
	std::string name = "";
	std::string model = "";
	std::string fullName = "";
	std::string script = "";
	std::string openSound = "";
	std::string closeSound = "";
};

std::vector<DOOR> vdoor;

//used by MISC records
struct MiscItemData {
 	float weight;
 	long int value;
 	long int unknown;
};

//misc items records
struct MISC {
	std::string name = "";
	std::string model = "";
	std::string fullName = "";
	MiscItemData mid;
	std::string script = "";
	std::string enchantment = "";
	std::string icon = "";
};

std::vector<MISC> vmisc;

//used by WEAP records
struct WeaponData { //0x20 bytes binary
 	float weight;
 	long int value;
 	short type; /*0 = ShortBladeOneHand
 				1 = LongBladeOneHand
 				2 = LongBladeTwoClose
 				3 = BluntOneHand
 				4 = BluntTwoClose
 				5 = BluntTwoWide
 				6 = SpearTwoWide
 				7 = AxeOneHand
 				8 = AxeTwoHand
 				9 = MarksmanBow
 				10 = MarksmanCrossbow
 				11 = MarksmanThrown
 				12 = Arrow
 				13 = Bolt*/
 	short health;
 	float speed;
 	float reach;
 	short enchantPts;
 	char chopMin;
 	char chopMax;
 	char slashMin;
 	char slashMax;
 	char thrustMin;
 	char thrustMax;
 	long flags; //0 = ?
 				//1 = ignore normal weapon resistance
 				//2 = silver
};

//weapon records
struct WEAP {
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	WeaponData wd;
	std::string icon = "";
	std::string enchantment = "";
	std::string script = "";
};

std::vector<WEAP> vweap;

//used by CONT records
struct Item {
 	long int count;
 	char name[32]; //id of the item
};

//container records
struct CONT {
	std::string name = "";
	std::string model = "";
	std::string fullName = "";
	float weight = 0.0;
	long int flags = 0;
	std::string script = "";
	std::vector<Item> items;
};

std::vector<CONT> vcont;

//used by SPEL records
struct SpellData {
 	long int type; /*0 = Spell
 					1 = Ability
 					2 = Blight
 					3 = Disease
 					4 = Curse
 					5 = Power*/
 	long int spellCost;
 	long int flags; /*0x0001 = AutoCalc
 					0x0002 = PC Start
 					0x0004 = Always Succeeds*/
};

struct EnchantmentsData {
 	uint16_t effectIndex;
 	int8_t skillAffected; //-1 if not applicable
 	int8_t attributeAffected; //-1 if not applicable
 	uint32_t range; //0=self, 1=touch, 2=target
 	uint32_t area;
 	uint32_t duration;
 	uint32_t magnitudeMin;
 	uint32_t magnitudeMax;
};

//spell records
struct SPEL {
	std::string name = "";
	std::string fullName = "";
	SpellData sd;
	std::vector<EnchantmentsData> ed;
};

std::vector<SPEL> vspel;

//used by CREA and NPC_ records
struct CreatureData {
 	long int type; //creature, daedra, undead, humanoid
 	long int level;
 	long int strength;
 	long int intelligence;
 	long int willpower;
 	long int agility;
 	long int speed;
 	long int endurance;
 	long int personality;
 	long int luck;
 	long int health;
 	long int spellPts;
 	long int fatigue;
 	long int soul;
 	long int combat;
 	long int magic;
 	long int stealth;
 	long int attackMin1;
 	long int attackMax1;
 	long int attackMin2;
 	long int attackMax2;
 	long int attackMin3;
 	long int attackMax3;
 	long int gold;
};

struct ItemRecord{
 	long int count; //quantity of the item
 	char name[32]; //the ID of the item
};

struct AiData{
 	uint8_t hello;
 	uint8_t unknown1;
 	uint8_t fight;
 	uint8_t flee;
 	uint8_t alarm;
 	uint8_t unknown2;
 	uint8_t unknown3;
 	uint8_t unknown4;
 	uint32_t flags;
 			/*0x00001 = Weapon
 			0x00002 = Armor
 			0x00004 = Clothing
 			0x00008 = Books
 			0x00010 = Ingredient
 			0x00020 = Picks
 			0x00040 = Probes
 			0x00080 = Lights
 			0x00100 = Apparatus
 			0x00200 = Repair Items
 			0x00400 = Misc
 			0x00800 = Spells
 			0x01000 = Magic Items
 			0x02000 = Potions
 			0x04000 = Training
 			0x08000 = Spellmaking
 			0x10000 = Enchanting
 			0x20000 = Repair
 			Remaining bits appear to be filled with junk data */
};

struct CellTravelDestination{
 	float posx,posy,posz;
 	float rotx,roty,rotz;  //in radians  
};

struct WanderPackage{
 	uint16_t distance;
 	uint16_t duration;
 	uint8_t timeOfDay;
 	uint8_t idles[8];
 	uint8_t unknown; //always 1
};

struct TravelPackage{
 	float x;
 	float y;
 	float z;
 	uint8_t unknown; //always 1
 	uint8_t unused[3];
};

struct FollowPackage{
 	float x;
 	float y;
 	float z;
 	uint16_t duration;
 	char ID[32];
 	uint8_t unknown; //always 1
 	uint8_t unused;
};

struct EscortPackage{
 	float x;
 	float y;
 	float z;
 	uint16_t duration;
 	char ID[32];
 	uint8_t unknown; //always 1
 	uint8_t unused;
};

struct ActivatePackage{
 	char name[32];
 	uint8_t unknown; //always 1
};

//creatures records
struct CREA {
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	std::string sound = "";
	std::vector<std::string> spells;
	CreatureData cd;
	long int flags;
	std::string script = "";
	std::vector<ItemRecord> ir;
	AiData ai;
	std::vector< std::pair< CellTravelDestination, std::string > > cellTravel;
		/*AI Packages - the following fields can appear in any order, one per AI package,
			with the order defining the package priority.
			Note: duration parameters in all packages are in hours.
			Any value greater than 24 should be divided by 100, and set to 24 if
			still greater than 24. The unknown value for each package seems to be
			an end-of-data marker; it is always a byte value set to 1 with any remaining
			data in the structure undefined and ignored.*/
	std::vector< WanderPackage > ai_w;
	std::vector< TravelPackage > ai_t;
	std::vector< ActivatePackage > ai_a;
	std::vector< FollowPackage > ai_f;
	std::vector< EscortPackage > ai_e;
	float scale = 0.0;
};

std::vector<CREA> vcrea;

//used by BODY registers
struct BodyPartData{
 	uint8_t part; 			/*0 = Head
 							1 = Hair
 							2 = Neck
 							3 = Chest
 							4 = Groin
 							5 = Hand
 							6 = Wrist
 							7 = Forearm
 							8 = Upperarm
 							9 = Foot
 							10 = Ankle
 							11 = Knee
 							12 = Upperleg
 							13 = Clavicle
 							14 = Tail*/
 	uint8_t vampire;
 	uint8_t flags; //1 = female, 2 = playable
 	uint8_t partType; //0 = skin, 1 = clothing, 2 = armor
};

//body parts registers
struct BODY {
	std::string name = "";
	std::string model = "";
	std::string fullName = "";
	BodyPartData bd;
};

std::vector<BODY> vbody;

//used by LIGH records
struct LightData{
 	float weight;
 	uint32_t value;
 	uint32_t time;
 	uint32_t radius;
 	uint8_t red;
 	uint8_t green;
 	uint8_t blue;
 	uint8_t null;
 	uint32_t flags;  /*0x0001 = Dynamic
 					0x0002 = Can Carry
 					0x0004 = Negative
 					0x0008 = Flicker
 					0x0010 = Fire
 					0x0020 = Off Default
 					0x0040 = Flicker Slow
 					0x0080 = Pulse
 					0x0100 = Pulse Slow*/
};

//lights recods
struct LIGH {
	std::string name = "";
	std::string model = "";
	std::string fullName = "";
	std::string icon = "";
	std::string sound = "";
	std::string script = "";
	LightData ld;
};

std::vector<LIGH> vligh;

//used by ENCH records
struct EnchantData{
 	uint32_t type; //0 = cast once, 1 = cast strikes, 2 = cast when used
 					//3 = constant effect
 	uint32_t enchantmentCost;
 	uint32_t charge;
 	uint32_t flags; //0x1 = autocalc
};

struct Enchantments{
 	uint16_t effectIndex;
 	int8_t skillAffected; //-1 if not applicable
 	int8_t attributeAffected; //-1 if not applicable
 	uint32_t range; //0 = self, 1 = touch, 2 = target
 	uint32_t area;
 	uint32_t duration;
 	uint32_t magnitudeMin;
 	uint32_t magnitudeMax;
};

//enchanting effects
struct ENCH {
	std::string name = "";
	EnchantData ed;
	std::vector< Enchantments > enchantments;
};

std::vector<ENCH> vench;

//used by NPC_ records

//12 bytes version , autocalc flag set
struct NpcData12 {
 	uint16_t level;
 	uint8_t disposition;
 	uint8_t reputation;
 	uint8_t rank;
 	uint8_t unknown[3]; //junk data
 	uint32_t gold;
};

//52 bytes version, autocalc flag clear
struct NpcData52 {
 	uint16_t level;
 	uint8_t attributes[8]; //in order of attribute ID
 	uint8_t skills[27]; //in order of skill ID
 	uint8_t unknown; //always 0
 	uint16_t health;
 	uint16_t spellPoints;
 	uint16_t fatigue;
 	uint8_t disposition;
 	uint8_t reputation;
 	uint8_t rank;
 	uint8_t unknown2; //junk data
 	uint32_t gold;
};

//creatures records
struct NPC_ {
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	std::string race = "";
	std::string faction = "";
	std::string headModel = "";
	std::string hairModel = "";
	std::string className = "";
	std::vector<std::string> spells;
	bool dataSize = false; //false = 12, true = 52
	NpcData12 cd12;
	NpcData52 cd52;
	long int flags;
	std::string script = "";
	std::vector<ItemRecord> ir;
	AiData ai;
	std::vector< std::pair< CellTravelDestination, std::string > > cellTravel;
	/*AI Packages - the following fields can appear in any order, one per AI package,
		with the order defining the package priority.
		Note: duration parameters in all packages are in hours.
		Any value greater than 24 should be divided by 100, and set to 24 if
		still greater than 24. The unknown value for each package seems to be
		an end-of-data marker; it is always a byte value set to 1 with any remaining
		data in the structure undefined and ignored.*/
	std::vector< WanderPackage > ai_w;
	std::vector< TravelPackage > ai_t;
	std::vector< ActivatePackage > ai_a;
	std::vector< FollowPackage > ai_f;
	std::vector< EscortPackage > ai_e;
	float scale = 0.0;
};

std::vector<NPC_> vnpc_;

//used by ARMO records
struct ArmourData {
	long int type; /*0 = Helmet
					1 = Cuirass
					2 = L. Pauldron
					3 = R. Pauldron
					4 = Greaves
					5 = Boots
					6 = L. Gauntlet
					7 = R. Gauntlet
					8 = Shield
					9 = L. Bracer
					10 = R. Bracer*/
	float weight;
	long int value;
	long int health;
	long int enchantPts;
	long int armour;
};

struct BodyPart{
	uint8_t bodyPartIndex = -1;
	std::string maleArmorName = "";
	std::string femaleArmorName = "";
				//0 = Head
				//1 = Hair
				//2 = Neck
				//3 = Cuirass
				//4 = Groin
				//5 = Skirt
				//6 = Right Hand
				//7 = Left Hand
				//8 = Right Wrist
				//9 = Left Wrist
				//10 = Shield
				//11 = Right Forearm
				//12 = Left Forearm
				//13 = Right Upper Arm
				//14 = Left Upper Arm
				//15 = Right Foot
				//16 = Left Foot
				//17 = Right Ankle
				//18 = Left Ankle
				//19 = Right Knee
				//20 = Left Knee
				//21 = Right Upper Leg
				//22 = Left Upper Leg
				//23 = Right Pauldron
				//24 = Left Pauldron
				//25 = Weapon
				//26 = Tail	
};

//armour records
struct ARMO{
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	ArmourData ad;
	std::string icon = "";
	std::vector<BodyPart> bp;
	std::string enchantment = "";
	std::string script = "";
};

std::vector<ARMO> varmo;

//used by CLOT records
struct ClothingData {
	long int type; /*0 = Pants
					1 = Shoes
					2 = Shirt
					3 = Belt
					4 = Robe
					5 = Right Glove
					6 = Left Glove
					7 = Skirt
					8 = Ring
					9 = Amulet*/
	float weight;
	uint16_t value;
	uint16_t enchantmentPts;
};

struct BodyPartClothing{
	uint8_t bodyPartIndex = -1;
	std::string maleBodyPartName = "";
	std::string femaleBodyPartName = "";
				//0 = Head
				//1 = Hair
				//2 = Neck
				//3 = Cuirass
				//4 = Groin
				//5 = Skirt
				//6 = Right Hand
				//7 = Left Hand
				//8 = Right Wrist
				//9 = Left Wrist
				//10 = Shield
				//11 = Right Forearm
				//12 = Left Forearm
				//13 = Right Upper Arm
				//14 = Left Upper Arm
				//15 = Right Foot
				//16 = Left Foot
				//17 = Right Ankle
				//18 = Left Ankle
				//19 = Right Knee
				//20 = Left Knee
				//21 = Right Upper Leg
				//22 = Left Upper Leg
				//23 = Right Pauldron
				//24 = Left Pauldron
				//25 = Weapon
				//26 = Tail	
};

//clothing records
struct CLOT{
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	ClothingData cd;
	std::string icon = "";
	std::vector<BodyPartClothing> bp;
	std::string enchantment = "";
	std::string script = "";
};

std::vector<CLOT> vclot;

//used by REPA records
struct RepairData {
	float weight;
	long int value;
	long int uses;
	float quality;
};

//repair items records
struct REPA{
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	RepairData rd;
	std::string icon = "";
	std::string script = "";
};

std::vector<REPA> vrepa;

//activator records
struct ACTI{
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	std::string script = "";
};

std::vector<ACTI> vacti;

//used by APPA records
struct ApparatusData {
	long int type; //0 = mortar and pestle, 1 = alembic, 2 = calcinator, 3 = retort
	float quality;
	float weight;
	long int value;
};

//alchemy apparatus records
struct APPA{
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	std::string script = "";
	std::string icon = "";
	ApparatusData ad;
};

std::vector<APPA> vappa;

//used by LOCK records
struct LockpickingData {
	float weight;
	long int value;
	float quality;
	long int uses;
};

//lockpicking items records
struct LOCK{
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	std::string script = "";
	std::string icon = "";
	LockpickingData ld;
};

std::vector<LOCK> vlock;

//used by PROB records
struct ProbeData {
	float weight;
	long int value;
	float quality;
	long int uses;
};

//probe items records
struct PROB{
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	std::string script = "";
	std::string icon = "";
	ProbeData pd;
};

std::vector<PROB> vprob;

//used by INGR records
struct IngredientData {
	float weight;
	long int value;
	long int effectID[4]; //0 or -1 means no effect
	long int skillID[4]; //only for skill related effects, 0 or -1 otherwise
	long int attributeID[4]; //only for attribute related effects, 0 or -1 otherwise
};

//ingredients records
struct INGR{
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	std::string script = "";
	std::string icon = "";
	IngredientData id;
};

std::vector<INGR> vingr;

//used by BOOK records
struct BookData {
 	float weight;
 	long int value;
 	long int scroll; //0 = not scroll, 1 = scroll
 	long int skillID; //-1 = not skill
 	long int enchantPts;
};

//books records
struct BOOK {
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	std::string script = "";
	std::string icon = "";
	std::string text = "";
	std::string enchantment = "";
	BookData bd;
};

std::vector<BOOK> vbook;

//used by ALCH records
struct AlchemyData {
 	float weight;
 	long int value;
 	long int flags; //0x1 = autocalc
};

struct AlchemyEnchantments {
 	uint16_t effectID;
 	int8_t sillAffected; //-1 if not applicable
 	int8_t attributeAffected; //-1 if not applicable
 	long int range; //0 = self, 1 = touch, 2 = target
 	long int area;
 	long int duration;
 	long int magnitudeMin;
 	long int magnitudeMax;
};

//potions records
struct ALCH {
	std::string name = "";
	std::string fullName = "";
	std::string model = "";
	std::string script = "";
	std::string icon = "";
	AlchemyData ad;
	std::vector<AlchemyEnchantments> ed;
};

std::vector<ALCH> valch;

//leveld items records
struct LEVI{
	std::string name = "";
	long int listData = 0;
	uint8_t chanceNone = 0;
	long int count = 0;
	std::vector< std::pair<std::string, uint16_t> > item;
};

std::vector<LEVI> vlevi;

//leveld creatures records
struct LEVC{
	std::string name = "";
	long int listData = 0;
	uint8_t chanceNone = 0;
	long int count = 0;
	std::vector< std::pair<std::string, uint16_t> > item;
};

std::vector<LEVC> vlevc;

//used by CELL records
struct CellData{
	long int flags; /*0x01 = interior?
					0x02 = has water
					0x04 = illegal to sleep here
					0x80 = behave like exterior (Tribunal)*/
	long int gridX;
	long int gridY;
};

struct AmbientLightLevel{
	long int ambientColor;
	long int sunlightColor;
	long int fogColor;
	float fogDensity;
};

struct ReferencePosition {
	float posx, posy, posz;
	float rotx, roty, rotz; //in radians
};

//used into CELL records
struct MovedReference{
	//only used in mods when creatures or NPCs are moved from one cell to another.
	//Commonly appear in saved game files as things move around.
	long int reference = 0; //same as the attached FRMR value
	std::string name = ""; //name of the cell the reference was moved to(interior cells only)
	long int GridX = 0; //coordinates of the cell the reference was moved to
	long int GridY = 0; 
	long int formReference = 0; //reference to the form that was moved
};

struct FormReference{
	long int referenceID = 0;
	std::string name = ""; //object ID or "PlayerSaveGame"
	uint8_t referenceBocked = 0; //present if blocked is set in the reference's record header
	float scale = 0.0; //if applicable and not 1.0
	std::string npcID = ""; //if applicable(npc-only)
	std::string globalVariableName = "";
	std::string factionID = "";
	long int factionRank = 0;
	std::string soulID = ""; //id of soul in gem (soul gems only)
	float charge = 0.0; //enchantment charge
	long int healthRemaining = 0; //weapons and armor
	long int usesRemaining = 0; //locks, probes, repair items
	float timeRemaining = 0; //lights
	long int value = 0; //unknown use
	CellTravelDestination ctd;
	std::string destinationCellName = "";
	long int lockDifficulty = 0;
	std::string keyName = "";
	std::string trapName = "";
	uint8_t disabled = 0; //reference is disabled (always 0).
	ReferencePosition rpos;
};

struct CELL{
	std::string name = ""; //this is the full name. Exterior regions have empty
							//strings; the region name is used instead.
	CellData cd;
	std::string regionName = ""; //exterior and like-exterior only.
	long int mapColor = 0; //exterior and like-exterior only.
	float waterHeight = 0.0; //interior only
	AmbientLightLevel al;
	std::vector<MovedReference> movedRefs;
	std::vector<FormReference> persistentRefs;
	long int temporaryRefsCount = 0;
	std::vector<FormReference> temporaryRefs;
};

std::vector<CELL> vcell;

//used by LAND registers
struct Coord{
	long int x,y;
};

struct Point {
	int8_t x, y, z; //y direction of the data is from the bottom up.
};

struct HeightData {
	float heightOffset; //Decreasing this value will shift the entire cell land down (by 8 units).
	int8_t hData[65][65]; //Contains the height data for the cell in the form of a 65×65 pixel array.
						//The height data is not absolute values but uses differences between 
						//adjacent pixels. Thus a pixel value of 0 means it has the same height 
						//as the last pixel. Note that the Y-direction of the data is from the bottom up.
	uint8_t junkData[3]; //ignored.
};

struct RGB {
	int8_t r, g, b; //Vertex Colors.RGB triples repeated for each vertex(1 byte per color per vertex, or 3 bytes per vertex).
};

//landscape registers
struct LAND{
	Coord coord;
	long int data; //data types included. If the relevant bit isn't set, the related fields
					//will not be loaded, even if present.
					//0x01 = includes VNML, VHGT and WNAM
					//0x02 = includes VCLR
					//0x03 = includes VTEX
	Point vertexNormals[65][65];
	HeightData heightData;
	uint8_t heights[9][9]; //heights for world map. Derived from VHGT data.
	RGB vertexColors[65][65];
	uint16_t textureIndices[16][16]; //each value corresponds to the index INTV value from a LTEX record.
};

std::vector<LAND> vland;

//used by PGRD records
struct PathData {
	long int gridx; //exterior only
	long int gridy; //exterior only
	uint16_t flags; /*The values below are the only ones accessible via the TESCS; other values exist 
					between the range of 0x1 and 0x4000 (with the exception of 0x4) but appear to be deprecated outside of cell [0, 0].
						0x80 = 128 granularity
						0x100 = 256 granularity
						0x200 = 512 granularity
						0x400 = 1024 granularity (Appears to be the default)
						0x800 = 2048 granularity
						0x1000 = 4096 granularity*/
	uint16_t pathPointCount; //see PGRP
};
/*Note: the Construction Set allows for a 44-byte version of the structure where NAME 
is a 32-byte null-terminated string immediately after Grid Y, rather than a field of 
its own. Only the 12-byte version is currently in use; the 44-byte version appears to 
be an older version.*/

struct PathPoint {
	long int x, y, z;
	uint8_t flags; //0x01 = autogenerated
	uint8_t connectionCount; //number of entries in PGRC
	uint16_t unknown;
};

//path grids records
struct PGRD{
	PathData pathData;
	std::string name = ""; //cell name the path grid belongs to
	PathPoint pathPoints[10000]; //there are around 650 entries max
	long int connectionList[10000];
	/*Connection list(also known as edges), where TotalConnCount is the sum of all Connection 
	count values in the PGRP node. For each path point listed in PGRP, there are Connection 
	count edges in this list, each an index into PGRP.Using Azura's Coast (12, 20) as an 
	example, the first entry in the PGRP (index 0) the Connection count value is 1, so you would 
	read one uint32 from this list. That has a value of 1, so index 0 connects to index 1. For 
	the second entry (index = 1), Connection count is 4, so you would read the next four uint32
	values: 0, 6, 3, and 2, meaning that the point at index 1 connects to the points at 
	indices 0, 6, 3, and 2.*/
};

std::vector<PGRD> vpgrd;

//sound generator registers
struct SNDG{
	std::string name = ""; /*ID - this appears to be generated from the creature name 
 							  (or DEFAULT) combined with the type formatted as a four-digit 
 							  number with leading zeroes (e.g., the ID for an alit scream is alit0006).
 								Note: there is one record in Bloodmoon which has an empty string here 
 								(BM_horker, Left Foot).*/
	long int soundTypeData; /*0 = Left Foot
							1 = Right Foot
							2 = Swim Left
							3 = Swim Right
							4 = Moan
							5 = Roar
							6 = Scream
							7 = Land*/
	std::string creatureName = "";
	std::string soundID = "";
};

std::vector<SNDG> vsndg;

//used by INFO records
struct InfoData {
	uint8_t dialogueType;
	uint8_t unused[3];
	long int dispositionOrJournalIndex;
	uint8_t rank;
	int8_t gender; //-1 = none, 0 = male, 1 = female
	uint8_t pcrank;
	uint8_t unusedData;
};

//Dialogue response record that belongs to previous DIAL record.
struct INFO{
	/*The three IDs form a linked list of INFOs for the DIAL. 
	The first INFO has an empty PNAM; the last has an empty NNAM. 
	Note: for override records, next/prev topic IDs can refer to 
	topics in the base record or the current one.*/
	std::string name = "";
	std::string previousName = "";
	std::string nextName = "";
	InfoData infoData;
	std::string actor = "";
	std::string race = "";
	std::string actorClass = "";
	std::string faction = ""; //FFFF = no faction
	std::string cell = "";
	std::string pcFaction = "";
	std::string sound = "";
	std::string responseText = "";
	std::vector<std::string> funcVarlist;
				/*Strings for the Function/Variable list
				char - Index ('0' to '5')
				char - Type ('1' to 'C', hex digit)
				'1' = Function
				'2' = Global
				'3' = Local
				'4' = Journal
				'5' = Item
				'6' = Dead
				'7' = Not ID
				'8' = Not Faction
				'9' = Not Class
				'A' = Not Race
				'B' = Not Cell
				'C' = Not Local
				char[2] - Details
				'00' to '72' = Function: function index
				'fX' = Global/Local/Not Local: Value is a float in FLTV
				'lX' = Global/Local/Not Local: Value is a long in INTV
				'sX' = Global/Local/Not Local: Value is a short in INTV (serves only as a value range - INTV is always a uint32)
				'JX' = Journal
				'IX' = Item
				'DX' = Dead
				'XX' = Not ID
				'FX' = Not Faction
				'CX' = Not Class
				'RX' = Not Race
				'LX' = Not Cell
				char - Operator
				'0' = '='
				'1' = '!='
				'2' = '>'
				'3' = '>='
				'4' = '<'
				'5' = '<='
				char[] - Name: Except for functions (which require no further information), everything after the fifth character is the ID for the global, local, journal, etc.*/
	long int iscvrComparison = 0; //integer value for SCVR comparison
	float fscvrComparison = 0.0; //float value for SCVR comparison
	std::string resultText = "";
	//the next three are only for journal entries
	uint8_t questName = 0;
	uint8_t questFinished = 0;
	uint8_t questRestart = 0;
};

//Dialogue topic (including journals) records
struct DIAL{
	/*DIAL records contain information on dialogue. Dialogue records are followed immediately by the 
	INFO records (topics) that belong to it. Unlike later games, there is no concept of parent-child 
	groups, so readers should simply read subsequent INFO records until they come to something that
	isn't an INFO record.*/
	std::string name = "";
	uint8_t dialogueType = 0; /*0 = Regular Topic
							1 = Voice
							2 = Greeting
							3 = Persuasion
							4 = Journal*/
	std::vector<INFO> vinfo;
};

std::vector<DIAL> vdial;