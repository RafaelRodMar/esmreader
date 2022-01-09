#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream> //read/write files
#include <string>
#include <map>

std::ifstream file;
std::map<std::string,int> elements;

struct RecordHeader {
	char name[4];     //not null-terminated.
	long int size;	  //not included the 16 bytes of header data.
	long int unknown; //deleted/ignored flag.
	long int flags;   //0x00002000 = Blocked
						//0x00000400 = Persistent

	std::ostream& serialize(std::ostream &out) const {
		out << "Record name: " << std::string(name, name + 4) << " | Size: " << size;
		out << " | Unknown data: " << unknown << " | Flags: " << flags << std::endl;
		return out;
	}

}recordHeader;

std::ostream& operator<<(std::ostream& out, const RecordHeader &obj) {
	obj.serialize(out);
	return out;
}

struct SubRecordHeader {
	char name[4];    //not null-terminated.
	long int size;   //not included the 8 bytes of header data.

	std::ostream& serialize(std::ostream &out) const {
		out << " Sub-Record name: " << std::string(name, name + 4) << " | Size: " << size;
		return out;
	}
}subRecordHeader;

std::ostream& operator<<(std::ostream& out, const SubRecordHeader &obj) {
	obj.serialize(out);
	return out;
}

void readRecordHeader() {
	file.read((char*)&recordHeader, sizeof(recordHeader));
	std::cout << recordHeader << std::endl;
}

int readSubRecordHeader() {
	file.read((char*)&subRecordHeader, sizeof(subRecordHeader));
	//std::cout << "Sub-Record header bytes read: " << sizeof(subRecordHeader) << std::endl;
	std::cout << subRecordHeader << std::endl;
	return sizeof(subRecordHeader);
}

int readSubRecordData(const std::string &name) {
	int bytesRead = 0;

	//main header record
	if (std::string(recordHeader.name, recordHeader.name+4) == "TES3")
	{
		if (name == "HEDR")
		{
			struct Hedr {
				float version;
				long int fileType; //(0=esp, 1=esm, 32=ess)
				char companyName[32];
				char description[256];
				long int numRecords; //48227
			}hedr;

			file.read((char*)&hedr, sizeof(hedr));
			std::cout << "  Version: " << hedr.version << std::endl;
			std::cout << "  File Type: " << hedr.fileType << std::endl;
			std::cout << "  Company Name: " << std::string(hedr.companyName, hedr.companyName + 32) << std::endl;
			std::cout << "  Description: " << std::string(hedr.description, hedr.description + 256) << std::endl;
			std::cout << "  Number of records: " << hedr.numRecords << std::endl;
			std::cout << "  Sub-Record data bytes read: " << sizeof(hedr) << std::endl;
			bytesRead += sizeof(hedr);
		}

		if (name == "MAST")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Required master file: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "DATA")
		{
			long long int aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << "  Size of the required master file: " << aux << std::endl;
			bytesRead += sizeof(aux);
		}
	}

	//Game setting record
	if (std::string(recordHeader.name, recordHeader.name + 4) == "GMST")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "STRV")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  String: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INTV")
		{
			long int aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << "  Integer Value: " << aux << std::endl;
			bytesRead += sizeof(aux);
		}

		if (name == "FLTV")
		{
			float aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << "  Float Value: " << aux << std::endl;
			bytesRead += sizeof(aux);
		}
	}

	//global variable
	if (std::string(recordHeader.name, recordHeader.name + 4) == "GLOB")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Global ID: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Type of global (short,long,float): " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FLTV")
		{
			float aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << "  Float Value: " << aux << std::endl;
			bytesRead += sizeof(aux);
		}
	}

	//class definition
	if (std::string(recordHeader.name, recordHeader.name + 4) == "CLAS")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Class ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Class name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "CLDT")
		{
			struct ClassData {
				long int attributeID1 = 0;
				long int attributeID2 = 0;
				long int specialization; //0 = combat, 1 = magic, 2 = stealth
				long int minorID1 = 0;
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
			}classData;

			file.read((char*)&classData, sizeof(classData));

			bytesRead += sizeof(classData);
			std::cout << "  Class Data: " << classData.attributeID1 << " " << classData.attributeID2 << " "
				<< classData.specialization << " " << classData.minorID1 << " " << classData.majorID1 << " " << classData.minorID2 << " " << classData.majorID2
				<< " " << classData.minorID3 << " " << classData.majorID3 << " " << classData.minorID4 << " " << classData.majorID4 << " "
				<< classData.minorID5 << " " << classData.majorID5 << " " << classData.flags << " " << classData.autoCalcFlags << std::endl;
		}

		if (name == "DESC")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Class description: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//faction definition
	if (std::string(recordHeader.name, recordHeader.name + 4) == "FACT")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Faction ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Faction name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "RNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Rank name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FADT")
		{
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
			}factionData;

			file.read((char*)&factionData, sizeof(factionData));

			bytesRead += sizeof(factionData);
			std::cout << "  Faction Data: " << factionData.attributeID1 << " " << factionData.attributeID2 << " "
				<< factionData.rd[0].attribute1 << " " << factionData.skillId[0] << " " << factionData.unknown1 << " "
				<< factionData.flags << std::endl;
		}

		if (name == "ANAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Faction name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INTV")
		{
			long int aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << "  Faction reaction value: " << aux;
			bytesRead += sizeof(aux);
		}
	}

	//race definition
	if (std::string(recordHeader.name, recordHeader.name + 4) == "RACE")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Race ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Race name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "RADT")
		{
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
			}raceData;

			file.read((char*)&raceData, sizeof(raceData));
			std::cout << "  Race data read " << std::endl;
			bytesRead += sizeof(raceData);
		}

		if (name == "NPCS")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Special power/ability: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "DESC")
		{
			char buffer[1000];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Race description: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//sound definition
	if (std::string(recordHeader.name, recordHeader.name + 4) == "SOUN")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sound ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sound filename relative to Sounds/ : " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "DATA")
		{
			struct SoundData {
				char volume; //(0 = 0.00, 255 = 1.00)
				char minRange;
				char maxRange;
			}soundData;

			file.read((char*)&soundData, sizeof(soundData));
			std::cout << "  Sound data: " << " vol: " << std::to_string(soundData.volume) << " minRange: " << std::to_string(soundData.minRange) <<
				" maxRange: " << std::to_string(soundData.maxRange) << std::endl;
			bytesRead += sizeof(soundData);
		}
	}

	//skill
	if (std::string(recordHeader.name, recordHeader.name + 4) == "SKIL")
	{
		if (name == "INDX")
		{
			long int aux; //0 to 26 hard-coded in the game
			file.read((char*)&aux, sizeof(aux));
			std::cout << "  Skill: " << aux << std::endl;
			bytesRead += sizeof(aux);
		}

		if (name == "SKDT")
		{
			struct SkillData {
				long int attribute;
				long int specialization; //0 = combat, 1 = magic, 2 = stealth
				float useValue[4]; //the use types for each skill are hard-coded.
			}skillData;

			file.read((char*)&skillData, sizeof(skillData));
			std::cout << "  Skill data read " << std::endl;
			bytesRead += sizeof(skillData);
		}

		if (name == "DESC")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Skill description string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//magic effect
	if (std::string(recordHeader.name, recordHeader.name + 4) == "MGEF")
	{
		if (name == "INDX")
		{
			long int aux; //0 to 137
			file.read((char*)&aux, sizeof(aux));
			std::cout << "  Magic effect: " << aux << std::endl;
			bytesRead += sizeof(aux);
		}

		if (name == "MEDT")
		{
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
			}magicEffectData;

			file.read((char*)&magicEffectData, sizeof(magicEffectData));
			std::cout << "  Magic effect data read " << std::endl;
			bytesRead += sizeof(magicEffectData);
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Effect icon string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "PTEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Particle texture string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "CVFX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Casting visual string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "BVFX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Bolt visual string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "HVFX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Hit visual string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "AVFX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Area visual string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "DESC")
		{
			char buffer[500];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Description text string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "CSND")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Cast Sound string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "BSND")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Bolt sound string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "HSND")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Hit sound string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "ASND")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Area sound string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}
	}

	//Script
	if (std::string(recordHeader.name, recordHeader.name + 4) == "SCPT")
	{
		if (name == "SCHD")
		{
			struct ScriptHeader {
				char name[32];
				long int numShorts;
				long int numLongs;
				long int numFloats;
				long int scriptDataSize;
				long int localVarSize;
			}scriptHeader;

			file.read((char*)&scriptHeader, sizeof(scriptHeader));
			std::cout << "  Nombre del script: " << std::string(scriptHeader.name, scriptHeader.name + 32) << std::endl;
			std::cout << "  Tama�o script: " << scriptHeader.scriptDataSize << std::endl;
			std::cout << "  Local var size: " << scriptHeader.localVarSize << std::endl;
			bytesRead += sizeof(scriptHeader);
		}

		if (name == "SCVR") //todas las variables separadas por '\0'
		{
			char buffer[10000];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  List of all the local variables: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCDT")
		{
			char buffer[10000];
			file.read((char*)&buffer, subRecordHeader.size);
			/*std::streamsize bytes = file.gcount();
			std::cout << "bytes read: " << bytes << std::endl;*/
			std::cout << "  Compiled script data read " << subRecordHeader.size << " bytes" << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCTX")
		{
			char buffer[10000];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script : " << std::endl << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}
	}

	//Region
	if (std::string(recordHeader.name, recordHeader.name + 4) == "REGN")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Region ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Region name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "WEAT")
		{
			struct WeatherData {
				char clear, cloudy, foggy, overcast, rain, thunder, ash, blight;
			}weatherData;

			file.read((char*)&weatherData, sizeof(weatherData));
			std::cout << "  Weather data read " << std::endl;
			bytesRead += sizeof(weatherData);
		}

		if (name == "BNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sleep creature string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNAM")
		{
			struct MapColor {
				char red, green, blue, null;
			}mapColor;

			file.read((char*)&mapColor, sizeof(mapColor));
			std::cout << "  map color read " << std::endl;
			bytesRead += sizeof(mapColor);
		}

		if (name == "SNAM")
		{
			struct SoundRecord {
				char soundName[32];
				char chance;
			}soundRecord;

			file.read((char*)&soundRecord, sizeof(soundRecord));
			std::cout << "  Sound record: " << std::string(soundRecord.soundName, 32) << std::endl;
			bytesRead += sizeof(soundRecord);
		}
	}

	//Birth sign
	if (std::string(recordHeader.name, recordHeader.name + 4) == "BSGN")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sign ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sign name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "TNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Texture filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "DESC")
		{
			char buffer[500];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Description string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "NPCS")
		{
			char spell_ability[32];
			file.read((char*)&spell_ability, 32);
			std::cout << "  Spell/ability read" << std::endl;
			bytesRead += 32;
		}
	}

	//landscape textures
	if (std::string(recordHeader.name, recordHeader.name + 4) == "LTEX")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INTV")
		{
			//uint32 but uint16 are used as indices in LAND records so are restricted to uint16
			long int index;
			file.read((char*)&index, sizeof(index));
			std::cout << "  Index: " << index << std::endl;
			bytesRead += sizeof(index);
		}

		if (name == "DATA")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//static
	if (std::string(recordHeader.name, recordHeader.name + 4) == "STAT")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//door definition
	if (std::string(recordHeader.name, recordHeader.name + 4) == "DOOR")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Door ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Door name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCIP" || name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sound name open: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ANAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sound name close: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//misc items
	if (std::string(recordHeader.name, recordHeader.name + 4) == "MISC")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Misc item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Misc item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MCDT")
		{
			struct MiscItemData {
				float weight;
				long int value;
				long int unknown;
			}miscItemData;

			file.read((char*)&miscItemData, sizeof(miscItemData));
			std::cout << "  Misc item data read" << std::endl;
			bytesRead += sizeof(miscItemData);
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ENAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Enchantment ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//Weapons
	if (std::string(recordHeader.name, recordHeader.name + 4) == "WEAP")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Weapon ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Weapon name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MCDT")
		{
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
			}weaponData;

			file.read((char*)&weaponData, sizeof(weaponData));
			std::cout << "  Weapon data read" << std::endl;
			bytesRead += sizeof(weaponData);
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ENAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Enchantment ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//Container
	if (std::string(recordHeader.name, recordHeader.name + 4) == "CONT")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Container ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Container name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNDT")
		{
			float weight;
			file.read((char*)&weight, sizeof(weight));
			std::cout << "  Container weight: " << weight << std::endl;
			bytesRead += sizeof(weight);
		}

		if (name == "FLAG")
		{
			long int containerFlags;
			//0x0001 = organic
			//0x0002 = respawns, organic only
			//0x0008 = default, unknown
			file.read((char*)&containerFlags, sizeof(containerFlags));
			std::cout << "  Container flags: " << containerFlags << std::endl;
			bytesRead += sizeof(containerFlags);
		}

		if (name == "NPCO")
		{
			struct Item {
				long int count;
				char name[32]; //id of the item
			}item;
			file.read((char*)&item, 32+4);
			std::cout << "  Item: " << std::string(item.name, item.name + 32) << " quantity: " << item.count << std::endl;
			bytesRead += 32+4;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//Spell
	if (std::string(recordHeader.name, recordHeader.name + 4) == "SPEL")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Spell ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Spell name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SPDT")
		{
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
			}spellData;
			file.read((char*)&spellData, sizeof(spellData));
			std::cout << "  Spell data read" << std::endl;
			bytesRead += sizeof(spellData);
		}

		if (name == "ENAM")
		{
			struct EnchantmentsData {
				uint16_t effectIndex;
				int8_t skillAffected; //-1 if not applicable
				int8_t attributeAffected; //-1 if not applicable
				uint32_t range; //0=self, 1=touch, 2=target
				uint32_t area;
				uint32_t duration;
				uint32_t magnitudeMin;
				uint32_t magnitudeMax;
			}enchantmentsData;

			file.read((char*)&enchantmentsData, sizeof(enchantmentsData));
			std::cout << "  Enchantments read" << std::endl;
			bytesRead += sizeof(enchantmentsData);
		}
	}

	//Creatures
	if (std::string(recordHeader.name, recordHeader.name + 4) == "CREA")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Creature ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Creature name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sound Gen name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "NPCS")
		{
			char buffer[300];  //32 bytes
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Spells: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "NPDT")
		{
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
			}creatureData;
			file.read((char*)&creatureData, sizeof(creatureData));
			std::cout << "  Creature data read" << std::endl;
			bytesRead += sizeof(creatureData);
		}

		if (name == "FLAG")
		{
			long int creatureFlags;
			/*0x0001 = Biped
			0x0002 = Respawn
			0x0004 = Weapon and shield
			0x0008 = None
			0x0010 = Swims
			0x0020 = Flies
			0x0040 = Walks
			0x0048 = Default flags
			0x0080 = Essential
			0x0400 = Skeleton Blood
			0x0800 = Metal Blood*/
			file.read((char*)&creatureFlags, sizeof(creatureFlags));
			std::cout << "  Creature flags read" << std::endl;
			bytesRead += sizeof(creatureFlags);
		}

		if(name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "NPCO")
		{
			struct ItemRecord{
				long int count; //quantity of the item
				char name[32]; //the ID of the item
			}itemRecord;

			file.read((char*)&itemRecord, sizeof(itemRecord));
			std::cout << "  Item Record: " << std::string(itemRecord.name, itemRecord.name + 32) << std::endl;
			bytesRead += 36;
		}

		if(name == "AIDT")
		{
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
			}aiData;

			file.read((char*)&aiData, sizeof(aiData));
			std::cout << "  AI data read" << std::endl;
			bytesRead += sizeof(aiData);
		}

		if(name == "DODT")
		{
			struct CellTravelDestination{
				float posx,posy,posz;
				float rotx,roty,rotz;
			}cellTravelDestination;

			file.read((char*)&cellTravelDestination, sizeof(cellTravelDestination));
			std::cout << "  Cell travel destination read" << std::endl;
			bytesRead += sizeof(cellTravelDestination);
		}

		if(name == "DNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Cell name for previous DODT, if interior: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "AI_W")
		{
			struct WanderPackage{
				uint16_t distance;
				uint16_t duration;
				uint8_t timeOfDay;
				uint8_t idles[8];
				uint8_t unknown; //always 1
			}wanderPackage;

			file.read((char*)&wanderPackage, sizeof(wanderPackage));
			std::cout << "  AI wander package read" << std::endl;
			bytesRead += sizeof(wanderPackage);
		}

		/*AI Packages - the following fields can appear in any order, one per AI package, 
		with the order defining the package priority.
		Note: duration parameters in all packages are in hours. 
		Any value greater than 24 should be divided by 100, and set to 24 if 
		still greater than 24. The unknown value for each package seems to be 
		an end-of-data marker; it is always a byte value set to 1 with any remaining 
		data in the structure undefined and ignored.*/

		if(name == "AI_T")
		{
			struct TravelPackage{
				float x;
				float y;
				float z;
				uint8_t unknown; //always 1
				uint8_t unused[3];
			}travelPackage;

			file.read((char*)&travelPackage, sizeof(travelPackage));
			std::cout << "  AI travel package read" << std::endl;
			bytesRead += sizeof(travelPackage);
		}

		if(name == "AI_F")
		{
			struct FollowPackage{
				float x;
				float y;
				float z;
				uint16_t duration;
				char ID[32];
				uint8_t unknown; //always 1
				uint8_t unused;
			}followPackage;

			file.read((char*)&followPackage, sizeof(followPackage));
			std::cout << "  AI follow package read" << std::endl;
			bytesRead += sizeof(followPackage);
		}

		if(name == "AI_E")
		{
			struct EscortPackage{
				float x;
				float y;
				float z;
				uint16_t duration;
				char ID[32];
				uint8_t unknown; //always 1
				uint8_t unused;
			}escortPackage;

			file.read((char*)&escortPackage, sizeof(escortPackage));
			std::cout << "  AI escort package read" << std::endl;
			bytesRead += sizeof(escortPackage);
		}

		if(name == "CNDT")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Cell scort/follow string(optional): " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "AI_A")
		{
			struct ActivatePackage{
				char name[32];
				uint8_t unknown; //always 1
			}activatePackage;

			file.read((char*)&activatePackage, sizeof(activatePackage));
			std::cout << "  AI activate package read" << std::endl;
			bytesRead += sizeof(activatePackage);
		}

		if(name == "XSCL")
		{
			float scale; //1.0 default
			file.read((char*)&scale, sizeof(scale));
			std::cout << "  Scale: " << scale << std::endl;
			bytesRead += sizeof(scale);
		}
	}

	//Body parts
	if (std::string(recordHeader.name, recordHeader.name + 4) == "BODY")
	{
		if(name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Body part ID string string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model file name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Race: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "BYDT")
		{
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
			}bodyPartData;

			file.read((char*)&bodyPartData, sizeof(bodyPartData));
			std::cout << "  Body part data read" << std::endl;
			bytesRead += sizeof(bodyPartData);
		}
	}

	//lights
	if (std::string(recordHeader.name, recordHeader.name + 4) == "LIGH")
	{
		if(name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Light ID string string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model file name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Light name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "SNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sound name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "SCRI" || name == "SCPT")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "LHDT")
		{
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
			}lightData;

			file.read((char*)&lightData, sizeof(lightData));
			std::cout << "  Light data read" << std::endl;
			bytesRead += sizeof(lightData);
		}
	}

	//Enchanting effects
	if (std::string(recordHeader.name, recordHeader.name + 4) == "ENCH")
	{
		if(name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Enchant ID string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "ENDT")
		{
			struct EnchantData{
				uint32_t type; //0 = cast once, 1 = cast strikes, 2 = cast when used
								//3 = constant effect
				uint32_t enchantmentCost;
				uint32_t charge;
				uint32_t flags; //0x1 = autocalc
			}enchantData;

			file.read((char*)&enchantData, sizeof(enchantData));
			std::cout << "  Enchant data read" << std::endl;
			bytesRead += sizeof(enchantData);
		}

		if(name == "ENAM")
		{
			struct Enchantments{
				uint16_t effectIndex;
				int8_t skillAffected; //-1 if not applicable
				int8_t attributeAffected; //-1 if not applicable
				uint32_t range; //0 = self, 1 = touch, 2 = target
				uint32_t area;
				uint32_t duration;
				uint32_t magnitudeMin;
				uint32_t magnitudeMax;
			}enchantments;

			file.read((char*)&enchantments, sizeof(enchantments));
			std::cout << "  Enchantments data read" << std::endl;
			bytesRead += sizeof(enchantments);
		}
	}

	//NPCs
	if (std::string(recordHeader.name, recordHeader.name + 4) == "NPC_")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NPC ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NPC name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "RNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Race name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ANAM")
		{
			char buffer[300];  //32 bytes
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Faction name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "BNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Head model: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Class name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "KNAM")
		{
			char buffer[300];  //32 bytes
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Hair model: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FLAG")
		{
			uint32_t npcFlags;
			    /*0x0001 = Female
				0x0002 = Essential
				0x0004 = Respawn
				0x0008 = Unknown (always set)
				0x0010 = Autocalc
				0x0400 = Blood Texture: Skeleton
				0x0800 = Blood Texture: Metal Sparks */
			file.read((char*)&npcFlags, sizeof(npcFlags));
			std::cout << "  NPC flags read" << std::endl;
			bytesRead += sizeof(npcFlags);
		}

		if (name == "NPDT")
		{
			if (subRecordHeader.size == 12)
			{
				//12 bytes version , autocalc flag set
				struct NpcData {
					uint16_t level;
					uint8_t disposition;
					uint8_t reputation;
					uint8_t rank;
					uint8_t unknown[3]; //junk data
					uint32_t gold;
				}npcData;
				file.read((char*)&npcData, sizeof(npcData));
				std::cout << "  NPC data read (12 bytes)" << std::endl;
				bytesRead += sizeof(npcData);
			}
			else
			{
				//52 bytes version, autocalc flag clear
				struct NpcData {
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
				}npcData;
				file.read((char*)&npcData, sizeof(npcData));
				std::cout << "  NPC data read (52 bytes)" << std::endl;
				bytesRead += sizeof(npcData);
			}
		}

		

		if(name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "NPCO")
		{
			struct ItemRecord{
				long int count; //quantity of the item
				char name[32]; //the ID of the item
			}itemRecord;

			file.read((char*)&itemRecord, sizeof(itemRecord));
			std::cout << "  Item Record: " << std::string(itemRecord.name, itemRecord.name + 32) << std::endl;
			bytesRead += 36;
		}

		if(name == "AIDT")
		{
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
			}aiData;

			file.read((char*)&aiData, sizeof(aiData));
			std::cout << "  AI data read" << std::endl;
			bytesRead += sizeof(aiData);
		}

		if(name == "DODT")
		{
			struct CellTravelDestination{
				float posx,posy,posz;
				float rotx,roty,rotz;
			}cellTravelDestination;

			file.read((char*)&cellTravelDestination, sizeof(cellTravelDestination));
			std::cout << "  Cell travel destination read" << std::endl;
			bytesRead += sizeof(cellTravelDestination);
		}

		if(name == "DNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Cell name for previous DODT, if interior: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "AI_W")
		{
			struct WanderPackage{
				uint16_t distance;
				uint16_t duration;
				uint8_t timeOfDay;
				uint8_t idles[8];
				uint8_t unknown; //always 1
			}wanderPackage;

			file.read((char*)&wanderPackage, sizeof(wanderPackage));
			std::cout << "  AI wander package read" << std::endl;
			bytesRead += sizeof(wanderPackage);
		}

		/*AI Packages - the following fields can appear in any order, one per AI package, 
		with the order defining the package priority.
		Note: duration parameters in all packages are in hours. 
		Any value greater than 24 should be divided by 100, and set to 24 if 
		still greater than 24. The unknown value for each package seems to be 
		an end-of-data marker; it is always a byte value set to 1 with any remaining 
		data in the structure undefined and ignored.*/

		if(name == "AI_T")
		{
			struct TravelPackage{
				float x;
				float y;
				float z;
				uint8_t unknown; //always 1
				uint8_t unused[3];
			}travelPackage;

			file.read((char*)&travelPackage, sizeof(travelPackage));
			std::cout << "  AI travel package read" << std::endl;
			bytesRead += sizeof(travelPackage);
		}

		if(name == "AI_F")
		{
			struct FollowPackage{
				float x;
				float y;
				float z;
				uint16_t duration;
				char ID[32];
				uint8_t unknown; //always 1
				uint8_t unused;
			}followPackage;

			file.read((char*)&followPackage, sizeof(followPackage));
			std::cout << "  AI follow package read" << std::endl;
			bytesRead += sizeof(followPackage);
		}

		if(name == "AI_E")
		{
			struct EscortPackage{
				float x;
				float y;
				float z;
				uint16_t duration;
				char ID[32];
				uint8_t unknown; //always 1
				uint8_t unused;
			}escortPackage;

			file.read((char*)&escortPackage, sizeof(escortPackage));
			std::cout << "  AI escort package read" << std::endl;
			bytesRead += sizeof(escortPackage);
		}

		if(name == "CNDT")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Cell scort/follow string(optional): " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if(name == "AI_A")
		{
			struct ActivatePackage{
				char name[32];
				uint8_t unknown; //always 1
			}activatePackage;

			file.read((char*)&activatePackage, sizeof(activatePackage));
			std::cout << "  AI activate package read" << std::endl;
			bytesRead += sizeof(activatePackage);
		}

		if(name == "XSCL")
		{
			float scale; //1.0 default
			file.read((char*)&scale, sizeof(scale));
			std::cout << "  Scale: " << scale << std::endl;
			bytesRead += sizeof(scale);
		}
	}

	//armour
	if (std::string(recordHeader.name, recordHeader.name + 4) == "ARMO")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Armour ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Armour name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "AODT")
		{
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
			}armourData;

			file.read((char*)&armourData, sizeof(armourData));
			std::cout << "  Armour data read" << std::endl;
			bytesRead += sizeof(armourData);
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Icon filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INDX")
		{
			uint8_t bodyPartIndex;
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


			file.read((char*)&bodyPartIndex, sizeof(bodyPartIndex));
			std::cout << "  Body part index: " << bodyPartIndex << std::endl;
			bytesRead += sizeof(bodyPartIndex);
		}

		if (name == "BNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Male armor name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Female armor name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ENAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Enchantment name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//Clothing
	if (std::string(recordHeader.name, recordHeader.name + 4) == "CLOT")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Clothing ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Clothing name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "CTDT")
		{
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
			}clothingData;


			file.read((char*)&clothingData, sizeof(clothingData));
			std::cout << "  Clothing data read: " << std::endl;
			bytesRead += sizeof(clothingData);
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INDX")
		{
			uint8_t bodyPartIndex; /*0 = Head
									1 = Hair
									2 = Neck
									3 = Cuirass
									4 = Groin
									5 = Skirt
									6 = Right Hand
									7 = Left Hand
									8 = Right Wrist
									9 = Left Wrist
									10 = Shield
									11 = Right Forearm
									12 = Left Forearm
									13 = Right Upper Arm
									14 = Left Upper Arm
									15 = Right Foot
									16 = Left Foot
									17 = Right Ankle
									18 = Left Ankle
									19 = Right Knee
									20 = Left Knee
									21 = Right Upper Leg
									22 = Left Upper Leg
									23 = Right Pauldron
									24 = Left Pauldron
									25 = Weapon
									26 = Tail*/
			file.read((char*)&bodyPartIndex, sizeof(bodyPartIndex));
			std::cout << "  Body part index: " << bodyPartIndex << std::endl;
			bytesRead += sizeof(bodyPartIndex);
		}

		if (name == "BNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Male body part name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Female body part name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		/*INDX and BNAM/CNAM are grouped together.
         INDX first followed by an optional BNAM
         (no BNAM indicates a NULL field for that index).*/

		if (name == "ENAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Enchantment name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//Repair items
	if (std::string(recordHeader.name, recordHeader.name + 4) == "REPA")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "RIDT")
		{
			struct RepairData {
				float weight;
				long int value;
				long int uses;
				float quality;
			}repairData;

			file.read((char*)&repairData, sizeof(repairData));
			std::cout << "  Repair data read" << std::endl;
			bytesRead += sizeof(repairData);
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//Activator
	if (std::string(recordHeader.name, recordHeader.name + 4) == "ACTI")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}
	}

	//Alchemy apparatus
	if (std::string(recordHeader.name, recordHeader.name + 4) == "APPA")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "AADT")
		{
			struct AlchemyData {
				long int type; //0 = mortar and pestle, 1 = alembic, 2 = calcinator, 3 = retort
				float quality;
				float weight;
				long int value;
			}alchemyData;

			file.read((char*)&alchemyData, sizeof(alchemyData));
			std::cout << "  Alchemy data read" << std::endl;
			bytesRead += sizeof(alchemyData);
		}
	}

	//Lockpicking items
	if (std::string(recordHeader.name, recordHeader.name + 4) == "LOCK")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "LKDT")
		{
			struct LockpickingData {
				float weight;
				long int value;
				float quality;
				long int uses;
			}lockpickingData;

			file.read((char*)&lockpickingData, sizeof(lockpickingData));
			std::cout << "  Lockpicking data read" << std::endl;
			bytesRead += sizeof(lockpickingData);
		}
	}

	//Probe items
	if (std::string(recordHeader.name, recordHeader.name + 4) == "PROB")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "PBDT")
		{
			struct ProbeData {
				float weight;
				long int value;
				float quality;
				long int uses;
			}probeData;

			file.read((char*)&probeData, sizeof(probeData));
			std::cout << "  Probe data read" << std::endl;
			bytesRead += sizeof(probeData);
		}
	}

	//Ingredients
	if (std::string(recordHeader.name, recordHeader.name + 4) == "INGR")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "IRDT")
		{
			struct IngredientData {
				float weight;
				long int value;
				long int effectID[4]; //0 or -1 means no effect
				long int skillID[4]; //only for skill related effects, 0 or -1 otherwise
				long int attributeID[4]; //only for attribute related effects, 0 or -1 otherwise
			}ingredientData;

			file.read((char*)&ingredientData, sizeof(ingredientData));
			std::cout << "  Ingredient data read" << std::endl;
			bytesRead += sizeof(ingredientData);
		}
	}

	//Books
	if (std::string(recordHeader.name, recordHeader.name + 4) == "BOOK")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "TEXT")
		{
			char buffer[50000];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Text of the book: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ENAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Enchantment name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "BKDT")
		{
			struct BookData {
				float weight;
				long int value;
				long int scroll; //0 = not scroll, 1 = scroll
				long int skillID; //-1 = not skill
				long int enchantPts;
			}bookData;

			file.read((char*)&bookData, sizeof(bookData));
			std::cout << "  Book data read" << std::endl;
			bytesRead += sizeof(bookData);
		}
	}

	//Potions
	if (std::string(recordHeader.name, recordHeader.name + 4) == "ALCH")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "TEXT")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Inventory icon: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ALDT")
		{
			struct AlchemyData {
				float weight;
				long int value;
				long int flags; //0x1 = autocalc
			}alchemyData;

			file.read((char*)&alchemyData, sizeof(alchemyData));
			std::cout << "  Alchemy data read" << std::endl;
			bytesRead += sizeof(alchemyData);
		}

		if (name == "ENAM")
		{
			struct Enchantments {
				uint16_t effectID;
				int8_t sillAffected; //-1 if not applicable
				int8_t attributeAffected; //-1 if not applicable
				long int range; //0 = self, 1 = touch, 2 = target
				long int area;
				long int duration;
				long int magnitudeMin;
				long int magnitudeMax;
			}enchantments;
			file.read((char*)&enchantments, sizeof(enchantments));
			std::cout << "  Enchantment data read" << std::endl;
			bytesRead += sizeof(enchantments);
		}
	}

	//Leveled items
	if (std::string(recordHeader.name, recordHeader.name + 4) == "LEVI")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Leveled list ID: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "DATA")
		{
			long int listData; //0x1 = calculate from all levels <= pc's level
								//0x2 = calculate for each item in count

			file.read((char*)&listData, sizeof(listData));
			std::cout << "  Leveled item data read" << std::endl;
			bytesRead += sizeof(listData);
		}

		if (name == "NNAM")
		{
			uint8_t chanceNone;

			file.read((char*)&chanceNone, sizeof(chanceNone));
			std::cout << "  Chance none : " << chanceNone << std::endl;
			bytesRead += sizeof(chanceNone);
		}

		if (name == "INDX")
		{
			long int count; //count of following items

			file.read((char*)&count, sizeof(count));
			std::cout << "  Count : " << count << std::endl;
			bytesRead += sizeof(count);
		}

		if (name == "INAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Item name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INTV")
		{
			uint16_t pcLevel; //pc level for previous CNAM

			file.read((char*)&pcLevel, sizeof(pcLevel));
			std::cout << "  PC level: " << pcLevel << std::endl;
			bytesRead += sizeof(pcLevel);
		}
		//The INAM/INTV can occur many times in pairs
	}

	//Leveled creatures
	if (std::string(recordHeader.name, recordHeader.name + 4) == "LEVC")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Leveled list ID: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "DATA")
		{
			long int listData; //0x1 = calculate from all levels <= pc's level
								//0x2 = calculate for each item in count

			file.read((char*)&listData, sizeof(listData));
			std::cout << "  Leveled creature data read" << std::endl;
			bytesRead += sizeof(listData);
		}

		if (name == "NNAM")
		{
			uint8_t chanceNone;

			file.read((char*)&chanceNone, sizeof(chanceNone));
			std::cout << "  Chance none : " << chanceNone << std::endl;
			bytesRead += sizeof(chanceNone);
		}

		if (name == "INDX")
		{
			long int count; //count of following items

			file.read((char*)&count, sizeof(count));
			std::cout << "  Count : " << count << std::endl;
			bytesRead += sizeof(count);
		}

		if (name == "CNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Creature name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INTV")
		{
			uint16_t pcLevel; //pc level for previous CNAM

			file.read((char*)&pcLevel, sizeof(pcLevel));
			std::cout << "  PC level: " << pcLevel << std::endl;
			bytesRead += sizeof(pcLevel);
		}
		//The CNAM/INTV can occur many times in pairs
	}

	//Cell definitions
	if (std::string(recordHeader.name, recordHeader.name + 4) == "CELL")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Cell list ID: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
			//Can be an empty string for exterior cells in which case the region name is used instead.
		}

		if (name == "DATA" && subRecordHeader.size == 12)
		{
			struct CellData{
				long int flags; /*0x01 = interior?
								0x02 = has water
								0x04 = illegal to sleep here
								0x80 = behave like exterior (Tribunal)*/
				long int gridX;
				long int gridY;
			}cellData;

			file.read((char*)&cellData, sizeof(cellData));
			std::cout << "  Cell data read" << std::endl;
			bytesRead += sizeof(cellData);
		}

		if (name == "RGNN")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Region name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "NAM0")
		{
			long int numnberOfObjectsInTheCell;

			file.read((char*)&numnberOfObjectsInTheCell, sizeof(numnberOfObjectsInTheCell));
			std::cout << "  Number of objects in the cell: " << numnberOfObjectsInTheCell << std::endl;
			bytesRead += sizeof(numnberOfObjectsInTheCell);
		}

		//exterior cell sub-records (NAM5)
		if(name == "NAM5")
		{
			long int mapColor;

			file.read((char*)&mapColor, sizeof(mapColor));
			std::cout << "  Map color: " << mapColor << std::endl;
			bytesRead += sizeof(mapColor);
		}

		//interior cell sub-records (WHGT and AMBI)
		if(name == "WHGT")
		{
			float waterHeight;

			file.read((char*)&waterHeight, sizeof(waterHeight));
			std::cout << "  Water height: " << waterHeight << std::endl;
			bytesRead += sizeof(waterHeight);
		}

		if(name == "AMBI")
		{
			struct AmbientLightLevel{
				long int ambientColor;
				long int sunlightColor;
				long int fogColor;
				float fogDensity;
			}ambientLightLevel;

			file.read((char*)&ambientLightLevel, sizeof(ambientLightLevel));
			std::cout << "  Ambient light level read" << std::endl;
			bytesRead += sizeof(ambientLightLevel);
		}

		//Referenced object data grouping (until end)
		if(name == "FRMR")
		{
			long int objectIndex; /*(starts at 1) (4 bytes, long)
			This is used to uniquely identify objects in the cell.
			For new files the index starts at 1 and is incremented
			for each new object added.  For modified objects the
			index is kept the same.*/

			file.read((char*)&objectIndex, sizeof(objectIndex));
			std::cout << "  Object Index : " << objectIndex << std::endl;
			bytesRead += sizeof(objectIndex);
		}

		//repeated tag into CELL record, but this one belongs to FRMR
		/*if (name == "NAME")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}*/

		if (name == "UNAM")
		{
			uint8_t referenceBlocked; //always 0, present if Blocked is set in the reference's record header,
										//otherwise absent.

			file.read((char*)&referenceBlocked, sizeof(referenceBlocked));
			std::cout << "  Reference blocked: " << referenceBlocked << std::endl;
			bytesRead += sizeof(referenceBlocked);
		}

		if (name == "XSCL")
		{
			float referenceScale; //if applicable.

			file.read((char*)&referenceScale, sizeof(referenceScale));
			std::cout << "  Reference Scale: " << referenceScale << std::endl;
			bytesRead += sizeof(referenceScale);
		}

		if (name == "ANAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  NPC ID: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "BNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Global variable name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Faction ID: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "INDX")
		{
			long int factionRank;

			file.read((char*)&factionRank, sizeof(factionRank));
			std::cout << "  Faction rank: " << factionRank << std::endl;
			bytesRead += sizeof(factionRank);
		}

		if (name == "XSOL")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Soul gem ID: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "XCHG")
		{
			float enchantmentCharge;

			file.read((char*)&enchantmentCharge, sizeof(enchantmentCharge));
			std::cout << "  Enchantment charge: " << enchantmentCharge << std::endl;
			bytesRead += sizeof(enchantmentCharge);
		}

		if (name == "INTV")
		{
			//it can be long int or float depending of type of object
			long int remainingUsage; //health(weapons and armor)
									//uses(locks, probes, repair items)
			float remUs;			//time remaining(lights)

			file.read((char*)&remUs, sizeof(remUs));
			std::cout << "  Remaining usage: " << remUs << std::endl;
			bytesRead += sizeof(remUs);
		}

		if (name == "NAM9")
		{
			long int value;

			file.read((char*)&value, sizeof(value));
			std::cout << "  Value: " << value << std::endl;
			bytesRead += sizeof(value);
		}

		if (name == "DODT")
		{
			struct CellTravelDestination {
				float posx, posy, posz;
				float rotx, roty, rotz; //in radians
			}cellTD;

			file.read((char*)&cellTD, sizeof(cellTD));
			std::cout << "  Cell travel destination read" << std::endl;
			bytesRead += sizeof(cellTD);
		}

		if (name == "DNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Cell name for previous DODT, if interior: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "FLTV")
		{
			long int lockDifficulty;

			file.read((char*)&lockDifficulty, sizeof(lockDifficulty));
			std::cout << "  Lock difficulty: " << lockDifficulty << std::endl;
			bytesRead += sizeof(lockDifficulty);
		}

		if (name == "KNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Key name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "TNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Trap name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "ZNAM")
		{
			uint8_t referenceDisabled; /*Reference is disabled (always 0). Like UNAM, this will be emitted 
									   if the relevant flag is set in the reference's record header. This 
									   may only be possible via scripting. Also, even if present in the 
									   file, the field appears to be ignored on loading.*/

			file.read((char*)&referenceDisabled, sizeof(referenceDisabled));
			std::cout << "  Ref disabled: " << referenceDisabled << std::endl;
			bytesRead += sizeof(referenceDisabled);
		}

		if (name == "DATA" && subRecordHeader.size == 24)
		{
			struct ReferencePosition {
				float posx, posy, posz;
				float rotx, roty, rotz; //in radians
			}refPos;

			file.read((char*)&refPos, sizeof(refPos));
			std::cout << "  Reference position read" << std::endl;
			bytesRead += sizeof(refPos);
		}

		if (bytesRead == 0)
		{
			std::cout << "<<<<<<<<<<<<<<<  UNKNOWN TAG >>>>>>>>>>>>>>>>>  " << name << std::endl;
		}
	}

	//landscape
	if (std::string(recordHeader.name, recordHeader.name + 4) == "LAND")
	{
		if (name == "INTV")
		{
			struct Coord {
				long int x, y;
			}coord;

			file.read((char*)&coord, sizeof(coord));
			std::cout << "  Coords: " << coord.x << "," << coord.y << std::endl;
			bytesRead += sizeof(coord);
		}

		if (name == "DATA")
		{
			long int data; //data types included. If the relevant bit isn't set, the related fields
							//will not be loaded, even if present.
							//0x01 = includes VNML, VHGT and WNAM
							//0x02 = includes VCLR
							//0x03 = includes VTEX

			file.read((char*)&data, sizeof(data));
			std::cout << "  Data: " << data << std::endl;
			bytesRead += sizeof(data);
		}

		if (name == "VNML")
		{
			struct Point {
				int8_t x, y, z; //y direction of the data is from the bottom up.
			};

			Point vertexNormals[65][65];

			file.read((char*)&vertexNormals, sizeof(vertexNormals));
			std::cout << "  Vertex Normals array loaded. Size: " << sizeof(vertexNormals) << std::endl;
			bytesRead += sizeof(vertexNormals);
		}

		if (name == "VHGT")
		{
			struct HeightData {
				float heightOffset; //Decreasing this value will shift the entire cell land down (by 8 units).
				int8_t hData[65][65]; //Contains the height data for the cell in the form of a 65×65 pixel array.
									//The height data is not absolute values but uses differences between 
									//adjacent pixels. Thus a pixel value of 0 means it has the same height 
									//as the last pixel. Note that the Y-direction of the data is from the bottom up.
				uint8_t junkData[3]; //ignored.
			}heightData;

			file.read((char*)&heightData, sizeof(heightData));
			std::cout << "  Height data read" << std::endl;
			bytesRead += sizeof(heightData);
		}

		if (name == "WNAM")
		{
			uint8_t heights[9][9]; //heights for world map. Derived from VHGT data.

			file.read((char*)&heights, sizeof(heights));
			std::cout << "  Read heights for world map" << std::endl;
			bytesRead += sizeof(heights);
		}

		if (name == "VCLR")
		{
			struct RGB {
				int8_t r, g, b; //Vertex Colors.RGB triples repeated for each vertex(1 byte per color per vertex, or 3 bytes per vertex).
			};

			RGB vertexColors[65][65];
			file.read((char*)&vertexColors, sizeof(vertexColors));
			std::cout << "  Array of vertex colors loaded. Size: " << sizeof(vertexColors) << std::endl;
			bytesRead += sizeof(vertexColors);
		}

		if (name == "VTEX")
		{
			uint16_t textureIndices[16][16]; //each value corresponds to the index INTV value from a LTEX record.

			file.read((char*)&textureIndices, sizeof(textureIndices));
			std::cout << "  Texture Indices read. Size: " << sizeof(textureIndices) << std::endl;
			bytesRead += sizeof(textureIndices);
		}
	}

	//Path grids
	if (std::string(recordHeader.name, recordHeader.name + 4) == "PGRD")
	{
		if (name == "DATA")
		{
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
			}pathData;
			/*Note: the Construction Set allows for a 44-byte version of the structure where NAME 
			is a 32-byte null-terminated string immediately after Grid Y, rather than a field of 
			its own. Only the 12-byte version is currently in use; the 44-byte version appears to 
			be an older version.*/

			file.read((char*)&pathData, sizeof(pathData));
			std::cout << "  Path data read" << std::endl;
			bytesRead += sizeof(pathData);
		}

		if (name == "NAME")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Cell name the path grid belongs to: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "PGRP")
		{
			struct PathPoint {
				long int x, y, z;
				uint8_t flags; //0x01 = autogenerated
				uint8_t connectionCount; //number of entries in PGRC
				uint16_t unknown;
			};

			PathPoint pathPoint[10000]; //there are around 650 entries max

			file.read((char*)&pathPoint, subRecordHeader.size);
			std::cout << "  Path points read" << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "PGRC")
		{
			long int connectionList[10000];
			/*Connection list(also known as edges), where TotalConnCount is the sum of all Connection 
			count values in the PGRP node. For each path point listed in PGRP, there are Connection 
			count edges in this list, each an index into PGRP.Using Azura's Coast (12, 20) as an 
			example, the first entry in the PGRP (index 0) the Connection count value is 1, so you would 
			read one uint32 from this list. That has a value of 1, so index 0 connects to index 1. For 
			the second entry (index = 1), Connection count is 4, so you would read the next four uint32
			values: 0, 6, 3, and 2, meaning that the point at index 1 connects to the points at 
			indices 0, 6, 3, and 2.*/

			file.read((char*)&connectionList, subRecordHeader.size);
			std::cout << "  Connection List read" << std::endl;
			bytesRead += subRecordHeader.size;
		}
	}

	//Sound generator
	if (std::string(recordHeader.name, recordHeader.name + 4) == "SNDG")
	{
		if (name == "NAME")
		{
			char buffer[300]; /*ID - this appears to be generated from the creature name 
							  (or DEFAULT) combined with the type formatted as a four-digit 
							  number with leading zeroes (e.g., the ID for an alit scream is alit0006).
								Note: there is one record in Bloodmoon which has an empty string here 
								(BM_horker, Left Foot).*/

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "DATA")
		{
			long int soundTypeData; /*0 = Left Foot
									1 = Right Foot
									2 = Swim Left
									3 = Swim Right
									4 = Moan
									5 = Roar
									6 = Scream
									7 = Land*/

			file.read((char*)&soundTypeData, sizeof(soundTypeData));
			std::cout << "  Sound type: " << soundTypeData << std::endl;
			bytesRead += sizeof(soundTypeData);
		}

		if (name == "CNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Creture name: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "SNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sound ID string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}
	}

	//Dialogue topic (including journals)
	if (std::string(recordHeader.name, recordHeader.name + 4) == "DIAL")
	{
		/*DIAL records contain information on dialogue. Dialogue records are followed immediately by the 
		INFO records (topics) that belong to it. Unlike later games, there is no concept of parent-child 
		groups, so readers should simply read subsequent INFO records until they come to something that
		isn't an INFO record.*/
		if (name == "NAME")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Dialogue id: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "DATA")
		{
			uint8_t dialogueType; /*0 = Regular Topic
									1 = Voice
									2 = Greeting
									3 = Persuasion
									4 = Journal*/
			file.read((char*)&dialogueType, sizeof(dialogueType));
			std::cout << "  Dialogue type: " << dialogueType << std::endl;
			bytesRead += sizeof(dialogueType);
		}
	}

	//Dialogue response record that belongs to previous DIAL record.
	if (std::string(recordHeader.name, recordHeader.name + 4) == "INFO")
	{
		/*The three IDs form a linked list of INFOs for the DIAL. 
		The first INFO has an empty PNAM; the last has an empty NNAM. 
		Note: for override records, next/prev topic IDs can refer to 
		topics in the base record or the current one.*/

		if (name == "INAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Info name string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "PNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Previous info id: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "NNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Next info id: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "DATA")
		{
			struct InfoData {
				uint8_t dialogueType;
				uint8_t unused[3];
				long int dispositionOrJournalIndex;
				uint8_t rank;
				int8_t gender; //-1 = none, 0 = male, 1 = female
				uint8_t pcrank;
				uint8_t unusedData;
			}infoData;

			file.read((char*)&infoData, sizeof(infoData));
			std::cout << "  Info data read" << std::endl;
			bytesRead += sizeof(infoData);
		}

		if (name == "ONAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Actor: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "RNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Race: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Class: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Faction('FFFF' = no faction): " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "ANAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Cell: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "DNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  PC faction: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "SNAM")
		{
			char buffer[300];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Sound: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "NAME")
		{
			char buffer[1000];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Response text: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCVR")
		{
			char buffer[1000];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Strings for the Function-Variable list: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
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
		}

		if (name == "INTV")
		{
			long int scvrComparison; //integer value for SCVR comparison

			file.read((char*)&scvrComparison, sizeof(scvrComparison));
			std::cout << "  scvr comparison integer: " << scvrComparison << std::endl;
			bytesRead += sizeof(scvrComparison);
		}

		if (name == "FLTV")
		{
			float scvrComparison; //float value for SCVR comparison

			file.read((char*)&scvrComparison, sizeof(scvrComparison));
			std::cout << "  scvr comparison float: " << scvrComparison << std::endl;
			bytesRead += sizeof(scvrComparison);
		}

		if (name == "BNAM")
		{
			char buffer[1000];

			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "  Result text: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		//the next three are only for journal entries.
		if (name == "QSTN")
		{
			uint8_t	questName;

			file.read((char*)&questName, sizeof(questName));
			std::cout << "  Quest name: " << questName << std::endl;
			bytesRead += sizeof(questName);
		}

		if (name == "QSTF")
		{
			uint8_t	questFinished;

			file.read((char*)&questFinished, sizeof(questFinished));
			std::cout << "  Quest finished: " << questFinished << std::endl;
			bytesRead += sizeof(questFinished);
		}

		if (name == "QSTR")
		{
			uint8_t	questRestart;

			file.read((char*)&questRestart, sizeof(questRestart));
			std::cout << "  Quest restart: " << questRestart << std::endl;
			bytesRead += sizeof(questRestart);
		}
	}

	return bytesRead;
}

bool isValid(std::string name) {
	if (name == "TES3") return true;
	if (name == "GMST") return true;
	if (name == "GLOB") return true;
	if (name == "CLAS") return true;
	if (name == "FACT") return true;
	if (name == "RACE") return true;
	if (name == "SOUN") return true;
	if (name == "SKIL") return true;
	if (name == "MGEF") return true;
	if (name == "SCPT") return true;
	if (name == "REGN") return true;
	if (name == "BSGN") return true;
	if (name == "LTEX") return true;
	if (name == "STAT") return true;
	if (name == "DOOR") return true;
	if (name == "MISC") return true;
	if (name == "WEAP") return true;
	if (name == "CONT") return true;
	if (name == "SPEL") return true;
	if (name == "CREA") return true;
	if (name == "BODY") return true;
	if (name == "LIGH") return true;
	if (name == "ENCH") return true;
	if (name == "NPC_") return true;
	if (name == "ARMO") return true;
	if (name == "CLOT") return true;
	if (name == "REPA") return true;
	if (name == "ACTI") return true;
	if (name == "APPA") return true;
	if (name == "LOCK") return true;
	if (name == "PROB") return true;
	if (name == "INGR") return true;
	if (name == "BOOK") return true;
	if (name == "ALCH") return true;
	if (name == "LEVI") return true;
	if (name == "LEVC") return true;
	if (name == "CELL") return true;
	if (name == "LAND") return true;
	if (name == "PGRD") return true;
	if (name == "SNDG") return true;
	if (name == "DIAL") return true;
	if (name == "INFO") return true;
	return false;
}

void readESM(const std::string &filename) {
	file.open(filename, std::ios::binary);

	while (!file.eof()) {
		//read the record
		readRecordHeader();
		if ( !isValid( std::string(recordHeader.name, recordHeader.name + 4) ) )
		{
			std::cout << "Unknown record: " << std::string(recordHeader.name, recordHeader.name + 4) << std::endl;
			break;
		}
		elements[std::string(recordHeader.name, recordHeader.name + 4)] = elements[std::string(recordHeader.name, recordHeader.name + 4)] + 1;

		//read the sub-records
		int bytesRead = 0;
		while (bytesRead < recordHeader.size) {
			int read = readSubRecordHeader(); //always 8 bytes
			if (read == 0) break;
			bytesRead += read;
			bytesRead = bytesRead + readSubRecordData(std::string(subRecordHeader.name, subRecordHeader.name + 4));
			std::cout << " Total bytes read: " << bytesRead << std::endl;
		}
		std::cout << " Sub-Records read finished" << std::endl;
		std::cout << "*************************************************************************" << std::endl;
		std::cout << std::endl;
	}

	file.close();

	std::map<std::string, int>::iterator it;
	for (it = elements.begin(); it != elements.end(); it++) {
		std::cout << it->first << " : " << it->second << std::endl;
	}
}


int main()
{
	readESM("c:/JuegosEstudio/Morrowind/Data Files/morrowind.esm");

	return 0;
}
