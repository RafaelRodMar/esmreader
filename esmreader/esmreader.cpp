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
		out << "Sub-Record name: " << std::string(name, name + 4) << " | Size: " << size;
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
	std::cout << "Sub-Record header bytes read: " << sizeof(subRecordHeader) << std::endl;
	std::cout << subRecordHeader;
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
			std::cout << " Version: " << hedr.version << std::endl;
			std::cout << " File Type: " << hedr.fileType << std::endl;
			std::string str2(hedr.companyName, hedr.companyName + 32);
			std::cout << " Company Name: " << str2 << std::endl;
			std::string str3(hedr.description, hedr.description + 256);
			std::cout << " Description: " << str3 << std::endl;
			std::cout << " Number of records: " << hedr.numRecords << std::endl;
			std::cout << " Sub-Record data bytes read: " << sizeof(hedr) << std::endl;
			bytesRead += sizeof(hedr);
		}

		if (name == "MAST")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Required master file: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "DATA")
		{
			long long int aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << " Size of the required master file: " << aux << std::endl;
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
			std::cout << " Name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "STRV")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " String: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INTV")
		{
			long int aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << " Integer Value: " << aux << std::endl;
			bytesRead += sizeof(aux);
		}

		if (name == "FLTV")
		{
			float aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << " Float Value: " << aux << std::endl;
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
			std::cout << " Global ID: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Type of global (short,long,float): " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FLTV")
		{
			float aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << " Float Value: " << aux << std::endl;
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
			std::cout << " Class ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Class name string: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Class Data: " << classData.attributeID1 << " " << classData.attributeID2 << " "
				<< classData.specialization << " " << classData.minorID1 << " " << classData.majorID1 << " " << classData.minorID2 << " " << classData.majorID2
				<< " " << classData.minorID3 << " " << classData.majorID3 << " " << classData.minorID4 << " " << classData.majorID4 << " "
				<< classData.minorID5 << " " << classData.majorID5 << " " << classData.flags << " " << classData.autoCalcFlags << std::endl;
		}

		if (name == "DESC")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Class description: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << " Faction ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Faction name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "RNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Rank name string: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Faction Data: " << factionData.attributeID1 << " " << factionData.attributeID2 << " "
				<< factionData.rd[0].attribute1 << " " << factionData.skillId[0] << " " << factionData.unknown1 << " "
				<< factionData.flags << std::endl;
		}

		if (name == "ANAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Faction name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INTV")
		{
			long int aux = 0;
			file.read((char*)&aux, sizeof(aux));
			std::cout << " Faction reaction value: " << aux;
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
			std::cout << " Race ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Race name string: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << " Race data read " << std::endl;
			bytesRead += sizeof(raceData);
		}

		if (name == "NPCS")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Special power/ability: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "DESC")
		{
			char buffer[1000];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Race description: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << " Sound ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Sound filename relative to Sounds/ : " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << " Sound data: " << " vol: " << std::to_string(soundData.volume) << " minRange: " << std::to_string(soundData.minRange) <<
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
			std::cout << " Skill: " << aux << std::endl;
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
			std::cout << " Skill data read " << std::endl;
			bytesRead += sizeof(skillData);
		}

		if (name == "DESC")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Skill description string: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << " Magic effect: " << aux << std::endl;
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
			std::cout << " Magic effect data read " << std::endl;
			bytesRead += sizeof(magicEffectData);
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Effect icon string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "PTEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Particle texture string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "CVFX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Casting visual string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "BVFX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Bolt visual string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "HVFX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Hit visual string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "AVFX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Area visual string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "DESC")
		{
			char buffer[500];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Description text string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "CSND")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Cast Sound string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "BSND")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Bolt sound string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "HSND")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Hit sound string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "ASND")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Area sound string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
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
			std::cout << " Nombre del script: " << std::string(scriptHeader.name, scriptHeader.name + 32) << std::endl;
			std::cout << " Tamaño script: " << scriptHeader.scriptDataSize << std::endl;
			std::cout << " Local var size: " << scriptHeader.localVarSize << std::endl;
			bytesRead += sizeof(scriptHeader);
		}

		if (name == "SCVR") //todas las variables separadas por '\0'
		{
			char buffer[10000];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " List of all the local variables: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCDT")
		{
			char buffer[10000];
			file.read((char*)&buffer, subRecordHeader.size);
			/*std::streamsize bytes = file.gcount();
			std::cout << "bytes read: " << bytes << std::endl;*/
			std::cout << " Compiled script data read " << subRecordHeader.size << " bytes" << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCTX")
		{
			char buffer[10000];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Script : " << std::endl << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
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
			std::cout << " Region ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Region name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "WEAT")
		{
			struct WeatherData {
				char clear, cloudy, foggy, overcast, rain, thunder, ash, blight;
			}weatherData;

			file.read((char*)&weatherData, sizeof(weatherData));
			std::cout << " Weather data read " << std::endl;
			bytesRead += sizeof(weatherData);
		}

		if (name == "BNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << " Sleep creature string: " << std::string(buffer, buffer + subRecordHeader.size) << std::endl;
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNAM")
		{
			struct MapColor {
				char red, green, blue, null;
			}mapColor;

			file.read((char*)&mapColor, sizeof(mapColor));
			std::cout << " map color read " << std::endl;
			bytesRead += sizeof(mapColor);
		}

		if (name == "SNAM")
		{
			struct SoundRecord {
				char soundName[32];
				char chance;
			}soundRecord;

			file.read((char*)&soundRecord, sizeof(soundRecord));
			std::cout << "Sound record: " << std::string(soundRecord.soundName, 32) << std::endl;
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
			std::cout << "Sign ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Sign name string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "TNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Texture filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "DESC")
		{
			char buffer[500];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Description string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "NPCS")
		{
			char spell_ability[32];
			file.read((char*)&spell_ability, 32);
			std::cout << "Spell/ability read" << std::endl;
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
			std::cout << "ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "INTV")
		{
			//uint32 but uint16 are used as indices in LAND records so are restricted to uint16
			long int index;
			file.read((char*)&index, sizeof(index));
			std::cout << "Index: " << index << std::endl;
			bytesRead += sizeof(index);
		}

		if (name == "DATA")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Filename: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "NIF model: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Door ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Door name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCIP")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Script: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Sound name open: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ANAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Sound name close: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Misc item ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Misc item name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Misc item data read" << std::endl;
			bytesRead += sizeof(miscItemData);
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Inventory icon filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ENAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Enchantment ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Script ID string: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Weapon ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Weapon name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Weapon data read" << std::endl;
			bytesRead += sizeof(weaponData);
		}

		if (name == "ITEX")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Inventory icon filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "ENAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Enchantment ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "SCRI")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Script ID string: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Container ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Container name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "CNDT")
		{
			float weight;
			file.read((char*)&weight, sizeof(weight));
			std::cout << "Container data read" << std::endl;
			bytesRead += sizeof(weight);
		}

		if (name == "FLAG")
		{
			long int containerFlags;
			//0x0001 = organic
			//0x0002 = respawns, organic only
			//0x0008 = default, unknown
			file.read((char*)&containerFlags, sizeof(containerFlags));
			std::cout << "Container flags: " << containerFlags << std::endl;
			bytesRead += sizeof(containerFlags);
		}

		if (name == "NPCO")
		{
			struct Item {
				long int count;
				char name[32]; //id of the item
			}item;
			file.read((char*)&item, 32+4);
			std::cout << "Item: " << std::string(item.name, item.name + 32) << " quantity: " << item.count << std::endl;
			bytesRead += 32+4;
		}
	}

	//Spell
	if (std::string(recordHeader.name, recordHeader.name + 4) == "SPEL")
	{
		if (name == "NAME")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Spell ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Spell name: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Spell data read" << std::endl;
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
			std::cout << "Enchantments read" << std::endl;
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
			std::cout << "Creature ID string: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "FNAM")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "Creature name: " << std::string(buffer, buffer + subRecordHeader.size);
			bytesRead += subRecordHeader.size;
		}

		if (name == "MODL")
		{
			char buffer[300];
			file.read((char*)&buffer, subRecordHeader.size);
			std::cout << "NIF model filename: " << std::string(buffer, buffer + subRecordHeader.size);
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
			std::cout << "Creature data read" << std::endl;
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
			std::cout << "Creature flags read" << std::endl;
			bytesRead += sizeof(creatureFlags);
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
			bytesRead = bytesRead + readSubRecordHeader();
			bytesRead = bytesRead + readSubRecordData(std::string(subRecordHeader.name, subRecordHeader.name + 4));
			std::cout << " Total bytes read: " << bytesRead << std::endl;
		}
		std::cout << "Sub-Records read finished" << std::endl;
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
