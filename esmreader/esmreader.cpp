#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream> //read/write files
#include<string>
#include<map>
#include "esmreader.h"

std::ifstream file;
std::map<std::string,int> elements;
std::map<std::string, int> originals;

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

std::vector< std::pair<std::string, std::vector<char>> > getSubRecordData(std::vector<char> &buffer) {
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	int tags = 0;
	int index = 0;
	while (index < buffer.size()) {
		std::string subRecordName = std::string(buffer.data() + index, 4);
		long int subRecordSize = 0;
		memmove(&subRecordSize, &buffer[index + 4], sizeof(subRecordSize));
		std::cout << "Name: " << subRecordName << " ";
		std::cout << "Size: " << subRecordSize << std::endl;
		index += 8;
		p.first = subRecordName;
		std::vector<char> newVec(buffer.begin() + index, buffer.begin() + index + subRecordSize);
		p.second = newVec;
		index += subRecordSize;
		v.push_back(p);
		tags++;
	}
	return v;
}

//get string from vector
std::string getString(std::vector<char> &v){
	char temp[50000];
	memmove((char*)&temp, v.data(), v.size());
	int i = 0;
	while(temp[i]!=0 && i < v.size()){ i++;	} 
	return std::string(temp, temp + i);
}

std::string getCompleteString(std::vector<char> &v) {
	char temp[50000];
	memmove((char*)&temp, v.data(), v.size());
	return std::string(temp, temp + v.size());
}

long int getLongInt(std::vector<char> &v){
	long int aux = 0;
	memmove((char*)&aux, v.data(), sizeof(aux));
	return aux;
}

float getFloat(std::vector<char> &v){
	float aux = 0;
	memmove((char*)&aux, v.data(), sizeof(aux));
	return aux;
}

uint8_t getuint8_tInt(std::vector<char> &v){
	uint8_t aux = 0;
	memmove((char*)&aux, v.data(), sizeof(aux));
	return aux;
}

uint16_t getuint16_tInt(std::vector<char> &v){
	uint16_t aux = 0;
	memmove((char*)&aux, v.data(), sizeof(aux));
	return aux;
}

void parseTES3(std::vector<char> &buffer){
	std::cout << "Parsing TES3 tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string,std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse the sub-records in the vector
	for (int i = 0; i < v.size(); i++) {
		if (v[i].first == "HEDR")
		{
			Hedr hedr;
			memmove((char*)&hedr, v[i].second.data(), sizeof(hedr));
			std::cout << "File type: " << hedr.fileType << std::endl;
			std::cout << "Version: " << hedr.version << std::endl;
			std::cout << "Company name: " << hedr.companyName << std::endl;
			std::cout << "Description: " << hedr.description << std::endl;
			std::cout << "Number of records: " << hedr.numRecords << std::endl;
		}

		//the next two can be repeated for every required master file.
 		if (v[i].first == "MAST")
 		{
 			std::cout << "  Required master file: " << getString(v[i].second) << std::endl;
 		}

 		if (v[i].first == "DATA")
 		{
 			std::cout << "  Size of the required master file: " << getLongInt(v[i].second) << std::endl;
 		}
	}
}

void parseGMST(std::vector<char> &buffer){
	
	std::cout << "Parsing GMST tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse the sub-records in the vector
	GMST g;
	for(auto x : v){
		if(x.first == "NAME") g.name = getString(x.second);
		if(x.first == "STRV") g.stringValue = getString(x.second);
		if(x.first == "INTV") g.intValue = getLongInt(x.second);
 		if(x.first == "FLTV") g.floatValue = getFloat(x.second);
	}

	std::cout << "  Name: " << g.name << std::endl;
	std::cout << "  String: " << g.stringValue << std::endl;
	std::cout << "  Int value: " << g.intValue << std::endl;
	std::cout << "  Float Value: " << g.floatValue << std::endl;
	vgmst.push_back(g);
}
void parseGLOB(std::vector<char> &buffer){
	
	std::cout << "Parsing GLOB tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse the sub-records in the vector
	GLOB g;
	for(auto x : v){
		if(x.first == "NAME") g.name = getString(x.second);
		if(x.first == "FNAM") g.type = getString(x.second);
		if(x.first == "FLTV" && g.type == "s") g.value = (short)getFloat(x.second);
		if(x.first == "FLTV" && g.type == "l") g.value = (long int)getFloat(x.second);
		if(x.first == "FLTV" && g.type == "f") g.value = getFloat(x.second);
	}

	std::cout << "  Name: " << g.name << std::endl;
	std::cout << "  Type: " << g.type << std::endl;
	std::cout << "  Value: " << g.value << std::endl;
	vglob.push_back(g);
}

void parseCLAS(std::vector<char> &buffer){
	
	std::cout << "Parsing CLAS tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse the sub-records in the vector
	CLAS c;
	for(auto x : v){
		if(x.first == "NAME") c.name = getString(x.second);
		if(x.first == "FNAM") c.fullName = getString(x.second);
		if(x.first == "CLDT") memmove((char*)&c.cd , x.second.data(), sizeof(c.cd));
		if(x.first == "DESC") c.description = getString(x.second);
	}

	std::cout << "  " << c.name << " " << c.fullName << " " << c.description << std::endl;
	vclas.push_back(c);
}

void parseFACT(std::vector<char> &buffer){
	
	std::cout << "Parsing FACT tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse the sub-records in the vector
	FACT f;
	for(int i = 0; i < v.size(); i++){
		std::pair< std::string, std::vector<char> > x = v[i];
		if(x.first == "NAME") f.name = getString(x.second);
		if(x.first == "FNAM") f.fullName = getString(x.second);
		if(x.first == "RNAM") f.rankName.push_back(getString(x.second));
		if(x.first == "FADT") memmove((char*)&f.fd, x.second.data(), sizeof(f.fd));
		if(x.first == "ANAM")
		{
			//read this value and the next INTV value
			f.factionReaction.push_back( std::make_pair(getString(x.second), getLongInt(v[i+1].second)));
		}
	}

	std::cout << f.name << " " << f.fullName << std::endl;
	for(auto s : f.rankName){
		std::cout << s << std::endl;
	}
	vfact.push_back(f);
}

void parseRACE(std::vector<char> &buffer){
	
	std::cout << "Parsing RACE tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse the sub-records in the vector
	RACE r;
	for(auto x : v){
		if(x.first == "NAME") r.name = getString(x.second);
		if(x.first == "FNAM") r.fullName = getString(x.second);
		if(x.first == "RADT") memmove((char*)&r.rd, x.second.data(), sizeof(r.rd));
		if(x.first == "NPCS") r.special.push_back(getString(x.second));
		if(x.first == "DESC") r.description = getString(x.second);
	}

	std::cout << r.fullName << " " << r.description << std::endl;
	std::cout << "special abilities" << std::endl;
	for (auto x : r.special) {
		std::cout << x << std::endl;
	}
	vrace.push_back(r);
}

void parseSOUN(std::vector<char> &buffer){
	
	std::cout << "Parsing SOUN tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	SOUN s;
	for(auto x : v){
		if(x.first == "NAME") s.name = getString(x.second);
		if(x.first == "FNAM") s.fullName = getString(x.second);
		if(x.first == "DATA") memmove((char*)&s.sd, x.second.data(), sizeof(s.sd));
	}

	std::cout << s.fullName << std::endl;
	vsoun.push_back(s);
}

void parseSKIL(std::vector<char> &buffer){
	
	std::cout << "Parsing SKIL tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	SKIL s;
	for (auto x : v) {
		if(x.first == "INDX") s.index = getLongInt(x.second);
		if(x.first == "SKDT") memmove((char*)&s.sd, x.second.data(), sizeof(s.sd));
		if(x.first == "DESC") s.description = getString(x.second);
	}

	std::cout << s.description << std::endl;
	vskil.push_back(s);
}

void parseMGEF(std::vector<char> &buffer){
	
	std::cout << "Parsing MGEF tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	MGEF m;
	for (auto x : v) {
		if(x.first == "INDX") m.index = getLongInt(x.second);
		if(x.first == "MEDT") memmove((char*)&m.md, x.second.data(), sizeof(m.md));
		if(x.first == "ITEX") m.iconTexture = getString(x.second);
		if(x.first == "PTEX") m.particleTexture = getString(x.second);
		if(x.first == "CVFX") m.castingVisual = getString(x.second);
		if(x.first == "BVFX") m.boltVisual = getString(x.second);
		if(x.first == "HVFX") m.hitVisual = getString(x.second);
		if(x.first == "AVFX") m.areaVisual = getString(x.second);
		if(x.first == "DESC") m.description = getString(x.second);
		if(x.first == "CSND") m.castSound = getString(x.second);
		if(x.first == "BSND") m.boltSound = getString(x.second);
		if(x.first == "HSND") m.hitSound = getString(x.second);
		if(x.first == "ASND") m.areaSound = getString(x.second);
	}

	std::cout << m.iconTexture << std::endl;
	std::cout << m.particleTexture << std::endl;
	std::cout << m.castingVisual << " // " << m.castSound << std::endl;
	std::cout << m.boltVisual << " // " << m.boltSound << std::endl;
	std::cout << m.hitVisual << " // " << m.hitSound << std::endl;
	std::cout << m.areaVisual << " // " << m.areaSound << std::endl;
	std::cout << m.description << std::endl;
	vmgef.push_back(m);
}

void parseSCPT(std::vector<char> &buffer){
	
	std::cout << "Parsing SCPT tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	SCPT s;
	for (auto x : v) {
		if (x.first == "SCHD") memmove((char*)&s.sh, x.second.data(), sizeof(s.sh));
		if (x.first == "SCVR") 
		{
			std::string aux = getCompleteString(x.second); //the complete list separated by \0
			std::cout << "Complete list: " << aux << std::endl;
			std::string buf = "";
			for (int i = 0; i < aux.size(); i++) {
				if (aux[i] == 0)
				{
					s.localVars.push_back(buf);
					std::cout << "var: " << buf << std::endl;
					buf = "";
					i++;
				}
				buf += aux[i];
			}
		}
		if (x.first == "SCDT") s.compiledScpt = x.second;
		if (x.first == "SCTX") s.textScpt = getString(x.second);
	}
	vscpt.push_back(s);
}

void parseREGN(std::vector<char> &buffer){
	
	std::cout << "Parsing REGN tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	REGN r;
	for(auto x : v){
		if(x.first == "NAME") r.name = getString(x.second);
		if(x.first == "FNAM") r.fullName = getString(x.second);
		if(x.first == "WEAT") memmove((char*)&r.wd, x.second.data(), sizeof(r.wd));
		if(x.first == "BNAM") r.sleepCreature = getString(x.second);
		if(x.first == "CNAM") memmove((char*)&r.mc, x.second.data(), sizeof(r.mc));
		if(x.first == "SNAM")
		{
			SoundRecord sr;
			memmove((char*)&sr, x.second.data(), sizeof(sr));
			r.sounds.push_back(sr);
		}
	}

	std::cout << r.fullName << std::endl;
	for(int i=0;i<r.sounds.size();i++){
		std::cout << "snd: " << std::string(r.sounds[i].soundName) << std::endl;
	}
	vregn.push_back(r);
}

void parseBSGN(std::vector<char> &buffer){
	
	std::cout << "Parsing BSGN tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	BSGN b;
	for(auto x : v){
		if(x.first == "NAME") b.name = getString(x.second);
		if(x.first == "FNAM") b.fullName = getString(x.second);
		if(x.first == "TNAM") b.textureFileName = getString(x.second);
		if(x.first == "DESC") b.description = getString(x.second);
		if(x.first == "NPCS") b.spell_ability.push_back(getString(x.second));
	}

	std::cout << b.fullName << " : " << b.description << std::endl;
	for(int i=0;i<b.spell_ability.size();i++){
		std::cout << "spell/ability: " << b.spell_ability[i] << std::endl;
	}
	vbsgn.push_back(b);
}

void parseLTEX(std::vector<char> &buffer){
	
	std::cout << "Parsing LTEX tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	LTEX l;
	for (auto x : v) {
		if (x.first == "NAME") l.name = getString(x.second);
		if (x.first == "INTV") l.index = getLongInt(x.second);
		if (x.first == "DATA") l.filename = getString(x.second);
	}

	std::cout << l.name << " : " << l.filename << std::endl;
	vltex.push_back(l);
}

void parseSTAT(std::vector<char> &buffer){
	
	std::cout << "Parsing STAT tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	STAT s;
	for (auto x : v) {
		if (x.first == "NAME") s.name = getString(x.second);
		if (x.first == "MODL") s.model = getString(x.second);
	}
	
	std::cout << s.name << " : " << s.model << std::endl;
	vstat.push_back(s);
}

void parseDOOR(std::vector<char> &buffer){
	
	std::cout << "Parsing DOOR tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	DOOR d;
	for (auto x : v) {
		if (x.first == "NAME") d.name = getString(x.second);
		if (x.first == "MODL") d.model = getString(x.second);
		if (x.first == "FNAM") d.fullName = getString(x.second);
		if (x.first == "SCRI") d.script = getString(x.second);
		if (x.first == "SNAM") d.openSound = getString(x.second);
		if (x.first == "ANAM") d.closeSound = getString(x.second);
	}

	std::cout << d.fullName << " : " << d.model << std::endl;
	vdoor.push_back(d);
}

void parseMISC(std::vector<char> &buffer){
	
	std::cout << "Parsing MISC tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	MISC m;
	for (auto x : v) {
		if (x.first == "NAME") m.name = getString(x.second);
		if (x.first == "MODL") m.model = getString(x.second);
		if (x.first == "FNAM") m.fullName = getString(x.second);
		if (x.first == "MCDT") memmove((char*)&m.mid, x.second.data(), sizeof(m.mid));
		if (x.first == "SCRI") m.script = getString(x.second);
		if (x.first == "ITEX") m.icon = getString(x.second);
		if (x.first == "ENAM") m.enchantment = getString(x.second);
	}

	std::cout << m.fullName << " : " << m.icon << std::endl;
	vmisc.push_back(m);
}

void parseWEAP(std::vector<char> &buffer){
	
	std::cout << "Parsing WEAP tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	WEAP w;
	for (auto x : v) {
		if (x.first == "NAME") w.name = getString(x.second);
		if (x.first == "MODL") w.model = getString(x.second);
		if (x.first == "FNAM") w.fullName = getString(x.second);
		if (x.first == "WPDT") memmove((char*)&w.wd, x.second.data(), sizeof(w.wd));
		if (x.first == "SCRI") w.script = getString(x.second);
		if (x.first == "ITEX") w.icon = getString(x.second);
		if (x.first == "ENAM") w.enchantment = getString(x.second);
	}
	
	std::cout << w.fullName << " : " << w.model << std::endl;
	vweap.push_back(w);
}

void parseCONT(std::vector<char> &buffer){
	
	std::cout << "Parsing CONT tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	CONT c;
	for (auto x : v) {
		if (x.first == "NAME") c.name = getString(x.second);
		if (x.first == "MODL") c.model = getString(x.second);
		if (x.first == "FNAM") c.fullName = getString(x.second);
		if (x.first == "CNDT") c.weight = getFloat(x.second);
		if (x.first == "FLAG") c.flags = getLongInt(x.second);
		if (x.first == "SCRI") c.script = getString(x.second);
		if (x.first == "NPCO")
		{
			Item i;
			memmove((char*)&i, x.second.data(), sizeof(i));
			c.items.push_back(i);
		}
	}

	std::cout << c.fullName << " : " << c.model << std::endl;
	vcont.push_back(c);
}

void parseSPEL(std::vector<char> &buffer){
	
	std::cout << "Parsing SPEL tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	SPEL s;
	for (auto x : v) {
		if (x.first == "NAME") s.name = getString(x.second);
		if (x.first == "FNAM") s.fullName = getString(x.second);
		if (x.first == "SPDT") memmove((char*)&s.sd, x.second.data(), sizeof(s.sd));
		if (x.first == "ENAM") 
		{
			EnchantmentsData ed;
			memmove((char*)&ed, x.second.data(), sizeof(ed));
			s.ed.push_back(ed);
		}
	}

	std::cout << s.fullName << std::endl;
	vspel.push_back(s);
}

void parseCREA(std::vector<char> &buffer){
	
	std::cout << "Parsing CREA tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	CREA c;
	for (auto x : v) {
		if (x.first == "NAME") c.name = getString(x.second);
		if (x.first == "FNAM") c.fullName = getString(x.second);
		if (x.first == "MODL") c.model = getString(x.second);
		if (x.first == "CNAM") c.sound = getString(x.second);
		if (x.first == "NPCS") c.spells.push_back(getCompleteString(x.second));
		if (x.first == "NPDT") memmove((char*)&c.cd, x.second.data(), sizeof(c.cd));
		if (x.first == "FLAG") c.flags = getLongInt(x.second);
		if (x.first == "SCRI") c.script = getString(x.second);
		if (x.first == "NPCO")
		{
			ItemRecord i;
			memmove((char*)&i, x.second.data(), sizeof(i));
			c.ir.push_back(i);
		}
		if (x.first == "AIDT") memmove((char*)&c.ai, x.second.data(), sizeof(c.ai));
		if (x.first == "DODT")
		{
			CellTravelDestination ct;
			std::string dummy = "";
			memmove((char*)&ct, x.second.data(), sizeof(ct));
			c.cellTravel.push_back(std::make_pair(ct, dummy));
		}
		//if there is a DODT then there is a DNAM after.
		if (x.first == "DNAM") c.cellTravel.back().second = getString(x.second);
		if (x.first == "AI_W")
		{
			WanderPackage wp;
			memmove((char*)&wp, x.second.data(), sizeof(wp));
			c.ai_w.push_back(wp);
		}
		if (x.first == "AI_T")
		{
			TravelPackage tp;
			memmove((char*)&tp, x.second.data(), sizeof(tp));
			c.ai_t.push_back(tp);
		}
		if (x.first == "AI_F")
		{
			FollowPackage fp;
			memmove((char*)&fp, x.second.data(), sizeof(fp));
			c.ai_f.push_back(fp);
		}
		if (x.first == "AI_E")
		{
			EscortPackage ep;
			memmove((char*)&ep, x.second.data(), sizeof(ep));
			c.ai_e.push_back(ep);
		}
		if (x.first == "AI_A")
		{
			ActivatePackage ap;
			memmove((char*)&ap, x.second.data(), sizeof(ap));
			c.ai_a.push_back(ap);
		}
		if (x.first == "XSCL") c.scale = getFloat(x.second);
	}

	std::cout << c.fullName << " " << c.model << std::endl;
	vcrea.push_back(c);
}

void parseBODY(std::vector<char> &buffer){
	
	std::cout << "Parsing BODY tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	BODY b;
	for (auto x : v) {
		if (x.first == "NAME") b.name = getString(x.second);
		if (x.first == "FNAM") b.fullName = getString(x.second);
		if (x.first == "MODL") b.model = getString(x.second);
		if (x.first == "BYDT") memmove((char*)&b.bd, x.second.data(), sizeof(b.bd));
	}

	std::cout << b.fullName << " " << b.model << std::endl;
	vbody.push_back(b);
}

void parseLIGH(std::vector<char> &buffer){
	
	std::cout << "Parsing LIGH tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	LIGH l;
	for (auto x : v) {
		if (x.first == "NAME") l.name = getString(x.second);
		if (x.first == "MODL") l.model = getString(x.second);
		if (x.first == "FNAM") l.fullName = getString(x.second);
		if (x.first == "ITEX") l.icon = getString(x.second);
		if (x.first == "SNAM") l.sound = getString(x.second);
		if (x.first == "SCRI") l.script = getString(x.second);
		if (x.first == "LHDT") memmove((char*)&l.ld, x.second.data(), sizeof(l.ld));
	}
	
	std::cout << l.fullName << " " << l.model << std::endl;
	vligh.push_back(l);
}

void parseENCH(std::vector<char> &buffer){
	
	std::cout << "Parsing ENCH tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records of the vector
	ENCH e;
	for (auto x : v) {
		if (x.first == "NAME") e.name = getString(x.second);
		if (x.first == "ENDT") memmove((char*)&e.ed, x.second.data(), sizeof(e.ed));
		if (x.first == "ENAM")
		{
			Enchantments en;
			memmove((char*)&en, x.second.data(), sizeof(en));
			e.enchantments.push_back(en);
		}
	}
	
	std::cout << e.name << std::endl;
	vench.push_back(e);
}

void parseNPC_(std::vector<char> &buffer){
	
	std::cout << "Parsing NPC_ tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	NPC_ n;
	for (auto x : v) {
		if (x.first == "NAME") n.name = getString(x.second);
		if (x.first == "FNAM") n.fullName = getString(x.second);
		if (x.first == "MODL") n.model = getString(x.second);
		if (x.first == "RNAM") n.race = getString(x.second);
		if (x.first == "CNAM") n.className = getString(x.second);
		if (x.first == "ANAM") n.faction = getString(x.second);
		if (x.first == "BNAM") n.headModel = getString(x.second);
		if (x.first == "KNAM") n.hairModel = getString(x.second);
		if (x.first == "NPCS") n.spells.push_back(getCompleteString(x.second));
		if (x.first == "NPDT")
		{
			if (x.second.size() == 12)
			{
				n.dataSize = false;
				memmove((char*)&n.cd12, x.second.data(), sizeof(n.cd12));
			}
			else
			{
				n.dataSize = true;
				memmove((char*)&n.cd52, x.second.data(), sizeof(n.cd52));
			}
		}
		if (x.first == "FLAG") n.flags = getLongInt(x.second);
		if (x.first == "SCRI") n.script = getString(x.second);
		if (x.first == "NPCO")
		{
			ItemRecord i;
			memmove((char*)&i, x.second.data(), sizeof(i));
			n.ir.push_back(i);
		}
		if (x.first == "AIDT") memmove((char*)&n.ai, x.second.data(), sizeof(n.ai));
		if (x.first == "DODT")
		{
			CellTravelDestination ct;
			std::string dummy = "";
			memmove((char*)&ct, x.second.data(), sizeof(ct));
			n.cellTravel.push_back(std::make_pair(ct, dummy));
		}
		//if there is a DODT then there is a DNAM after.
		if (x.first == "DNAM") n.cellTravel.back().second = getString(x.second);
		if (x.first == "AI_W")
		{
			WanderPackage wp;
			memmove((char*)&wp, x.second.data(), sizeof(wp));
			n.ai_w.push_back(wp);
		}
		if (x.first == "AI_T")
		{
			TravelPackage tp;
			memmove((char*)&tp, x.second.data(), sizeof(tp));
			n.ai_t.push_back(tp);
		}
		if (x.first == "AI_F")
		{
			FollowPackage fp;
			memmove((char*)&fp, x.second.data(), sizeof(fp));
			n.ai_f.push_back(fp);
		}
		if (x.first == "AI_E")
		{
			EscortPackage ep;
			memmove((char*)&ep, x.second.data(), sizeof(ep));
			n.ai_e.push_back(ep);
		}
		if (x.first == "AI_A")
		{
			ActivatePackage ap;
			memmove((char*)&ap, x.second.data(), sizeof(ap));
			n.ai_a.push_back(ap);
		}
		if (x.first == "XSCL") n.scale = getFloat(x.second);
	}

	std::cout << n.fullName << " " << n.model << std::endl;
	vnpc_.push_back(n);
}
void parseARMO(std::vector<char> &buffer){
	
	std::cout << "Parsing ARMO tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	ARMO a;
	for(auto x : v){
		if(x.first == "NAME") a.name = getString(x.second);
		if(x.first == "FNAM") a.fullName = getString(x.second);
		if(x.first == "MODL") a.model = getString(x.second);
		if(x.first == "AODT") memmove((char*)&a.ad, x.second.data(), sizeof(a.ad));
		if(x.first == "ITEX") a.icon = getString(x.second);
		if(x.first == "INDX")
		{
			BodyPart bp;
			bp.bodyPartIndex = getuint8_tInt(x.second);
			a.bp.push_back(bp);
		}
		//if there is a INDX record then may be a BNAM or CNAM record after it.
		//So every BNAM / CNAM records belong to the las INDX record.
		if(x.first == "BNAM") a.bp.back().maleArmorName = getString(x.second);
		if(x.first == "CNAM") a.bp.back().femaleArmorName = getString(x.second);
		if(x.first == "ENAM") a.enchantment = getString(x.second);
		if(x.first == "SCRI") a.script = getString(x.second);
	}

	std::cout << a.fullName << " " << a.model << std::endl;
	varmo.push_back(a);
}

void parseCLOT(std::vector<char> &buffer){
	
	std::cout << "Parsing CLOT tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	CLOT c;
	for(auto x : v){
		if(x.first == "NAME") c.name = getString(x.second);
		if(x.first == "FNAM") c.fullName = getString(x.second);
		if(x.first == "MODL") c.model = getString(x.second);
		if(x.first == "CTDT") memmove((char*)&c.cd, x.second.data(), sizeof(c.cd));
		if(x.first == "ITEX") c.icon = getString(x.second);
		if(x.first == "INDX")
		{
			BodyPartClothing bp;
			bp.bodyPartIndex = getuint8_tInt(x.second);
			c.bp.push_back(bp);
		}
		//if there is a INDX record then may be a BNAM or CNAM record after it.
		//So every BNAM / CNAM records belong to the las INDX record.
		if(x.first == "BNAM") c.bp.back().maleBodyPartName = getString(x.second);
		if(x.first == "CNAM") c.bp.back().femaleBodyPartName = getString(x.second);
		if(x.first == "ENAM") c.enchantment = getString(x.second);
		if(x.first == "SCRI") c.script = getString(x.second);
	}

	std::cout << c.fullName << " " << c.model << std::endl;
	vclot.push_back(c);
}

void parseREPA(std::vector<char> &buffer){
	
	std::cout << "Parsing REPA tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	REPA r;
	for(auto x : v){
		if(x.first == "NAME") r.name = getString(x.second);
		if(x.first == "FNAM") r.fullName = getString(x.second);
		if(x.first == "MODL") r.model = getString(x.second);
		if(x.first == "RIDT") memmove((char*)&r.rd, x.second.data(), sizeof(r.rd));
		if(x.first == "ITEX") r.icon = getString(x.second);
		if(x.first == "SCRI") r.script = getString(x.second);
	}

	std::cout << r.fullName << " " << r.model << std::endl;
	vrepa.push_back(r);
}

void parseACTI(std::vector<char> &buffer){
	
	std::cout << "Parsing ACTI tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	ACTI a;
	for(auto x : v){
		if(x.first == "NAME") a.name = getString(x.second);
		if(x.first == "FNAM") a.fullName = getString(x.second);
		if(x.first == "MODL") a.model = getString(x.second);
		if(x.first == "SCRI") a.script = getString(x.second);
	}

	std::cout << a.fullName << " " << a.model << std::endl;
	vacti.push_back(a);
}

void parseAPPA(std::vector<char> &buffer){
	
	std::cout << "Parsing APPA tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	APPA a;
	for(auto x : v){
		if(x.first == "NAME") a.name = getString(x.second);
		if(x.first == "FNAM") a.fullName = getString(x.second);
		if(x.first == "MODL") a.model = getString(x.second);
		if(x.first == "SCRI") a.script = getString(x.second);
		if(x.first == "ITEX") a.icon = getString(x.second);
		if(x.first == "AADT") memmove((char*)&a.ad, x.second.data(), sizeof(a.ad));
	}

	std::cout << a.fullName << " " << a.model << std::endl;
	vappa.push_back(a);
}

void parseLOCK(std::vector<char> &buffer){
	
	std::cout << "Parsing LOCK tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	LOCK l;
	for(auto x : v){
		if(x.first == "NAME") l.name = getString(x.second);
		if(x.first == "FNAM") l.fullName = getString(x.second);
		if(x.first == "MODL") l.model = getString(x.second);
		if(x.first == "SCRI") l.script = getString(x.second);
		if(x.first == "ITEX") l.icon = getString(x.second);
		if(x.first == "LKDT") memmove((char*)&l.ld, x.second.data(), sizeof(l.ld));
	}

	std::cout << l.fullName << " " << l.model << std::endl;
	vlock.push_back(l);
}

void parsePROB(std::vector<char> &buffer){
	
	std::cout << "Parsing PROB tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	PROB p;
	for(auto x : v){
		if(x.first == "NAME") p.name = getString(x.second);
		if(x.first == "FNAM") p.fullName = getString(x.second);
		if(x.first == "MODL") p.model = getString(x.second);
		if(x.first == "SCRI") p.script = getString(x.second);
		if(x.first == "ITEX") p.icon = getString(x.second);
		if(x.first == "LKDT") memmove((char*)&p.pd, x.second.data(), sizeof(p.pd));
	}

	std::cout << p.fullName << " " << p.model << std::endl;
	vprob.push_back(p);
}

void parseINGR(std::vector<char> &buffer){
	
	std::cout << "Parsing INGR tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	INGR i;
	for(auto x : v){
		if(x.first == "NAME") i.name = getString(x.second);
		if(x.first == "FNAM") i.fullName = getString(x.second);
		if(x.first == "MODL") i.model = getString(x.second);
		if(x.first == "SCRI") i.script = getString(x.second);
		if(x.first == "ITEX") i.icon = getString(x.second);
		if(x.first == "LKDT") memmove((char*)&i.id, x.second.data(), sizeof(i.id));
	}

	std::cout << i.fullName << " " << i.model << std::endl;
	vingr.push_back(i);
}

void parseBOOK(std::vector<char> &buffer){
	
	std::cout << "Parsing BOOK tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	BOOK b;
	for (auto x : v) {
		if (x.first == "NAME") b.name = getString(x.second);
		if (x.first == "FNAM") b.fullName = getString(x.second);
		if (x.first == "MODL") b.model = getString(x.second);
		if (x.first == "SCRI") b.script = getString(x.second);
		if (x.first == "TEXT") b.text = getString(x.second);
		if (x.first == "ITEX") b.icon = getString(x.second);
		if (x.first == "ENAM") b.enchantment = getString(x.second);
		if (x.first == "LKDT") memmove((char*)&b.bd, x.second.data(), sizeof(b.bd));
	}

	std::cout << b.fullName << " " << b.model << std::endl;
	vbook.push_back(b);
}

void parseALCH(std::vector<char> &buffer){
	
	std::cout << "Parsing ALCH tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	ALCH a;
	for (auto x : v) {
		if (x.first == "NAME") a.name = getString(x.second);
		if (x.first == "FNAM") a.fullName = getString(x.second);
		if (x.first == "MODL") a.model = getString(x.second);
		if (x.first == "SCRI") a.script = getString(x.second);
		if (x.first == "TEXT") a.icon = getString(x.second);
		if (x.first == "ALDT") memmove((char*)&a.ad, x.second.data(), sizeof(a.ad));
		if (x.first == "ENAM")
		{
			AlchemyEnchantments en;
			memmove((char*)&en, x.second.data(), sizeof(en));
			a.ed.push_back(en);
		}
	}

	std::cout << a.fullName << " " << a.model << std::endl;
	valch.push_back(a);
}
void parseLEVI(std::vector<char> &buffer){
	
	std::cout << "Parsing LEVI tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	LEVI l;
	for(auto x : v){
		if(x.first == "NAME") l.name = getString(x.second);
		if(x.first == "DATA") l.listData = getLongInt(x.second);
		if(x.first == "NNAM") l.chanceNone = getuint8_tInt(x.second);
		if(x.first == "INDX") l.count = getLongInt(x.second);
		if(x.first == "INAM") l.item.push_back( std::make_pair(getString(x.second), 0));
		//every INTV record belongs to the previous INAM record.
		if(x.first == "INTV") l.item.back().second = getuint16_tInt(x.second);
	}
	std::cout << l.name << " " << l.count << std::endl;
	vlevi.push_back(l);
}

void parseLEVC(std::vector<char> &buffer){
	
	std::cout << "Parsing LEVC tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	LEVC c;
	for(auto x : v){
		if(x.first == "NAME") c.name = getString(x.second);
		if(x.first == "DATA") c.listData = getLongInt(x.second);
		if(x.first == "NNAM") c.chanceNone = getuint8_tInt(x.second);
		if(x.first == "INDX") c.count = getLongInt(x.second);
		if(x.first == "CNAM") c.item.push_back( std::make_pair(getString(x.second), 0));
		//every INTV record belongs to the previous INAM record.
		if(x.first == "INTV") c.item.back().second = getuint16_tInt(x.second);
	}
	std::cout << c.name << " " << c.count << std::endl;
	vlevc.push_back(c);
}

void parseCELL(std::vector<char> &buffer){
	
	std::cout << "Parsing CELL tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	CELL c;
	bool inReference = false; //while false it's reading the cell data,
								//otherwise it's reading a reference's data.
	bool inMovedRef = false;
	bool inPersistentRef = false;
	bool inTemporaryRef = false;
	for(int i=0;i<v.size();i++){
		if( !inReference )
		{
			//reading the cell data
			if(v[i].first == "NAME") c.name = getString(v[i].second);
			if(v[i].first == "DATA") memmove((char*)&c.cd, v[i].second.data(), sizeof(c.cd));
			if(v[i].first == "RGNN") c.regionName = getString(v[i].second);
			if(v[i].first == "NAM5") c.mapColor = getLongInt(v[i].second);
			if(v[i].first == "WHGT") c.waterHeight = getFloat(v[i].second);
			if(v[i].first == "AMBI") memmove((char*)&c.al, v[i].second.data(), sizeof(c.al));
			if(v[i].first == "NAM0") c.temporaryRefsCount = getLongInt(v[i].second);
			if(v[i].first == "MVRF")
			{
				MovedReference mr;
				mr.reference = getLongInt(v[i].second);
				c.movedRefs.push_back(mr);
				inReference = true;
				inMovedRef = true;
			}
			if(v[i].first == "FRMR")
			{
				FormReference fr;
				fr.referenceID = getLongInt(v[i].second);
				c.persistentRefs.push_back(fr);
				inReference = true;
				inPersistentRef = true;
			}
		}
		else
		{
			//reading a reference data
			if( inMovedRef )
			{
				//moved references
				if(v[i].first == "CNAM")
				{
					c.movedRefs.back().name = getString(v[i].second);
				}
				else if(v[i].first == "CNDT")
				{
					struct coords{
						long int x,y;
					}crd;
					memmove((char*)&crd, v[i].second.data(), sizeof(crd));
					c.movedRefs.back().GridX = crd.x;
					c.movedRefs.back().GridY = crd.y;
				} else if(v[i].first == "")
				{
					c.movedRefs.back().formReference = getLongInt(v[i].second);
				} else
				{
					//is not a moved reference tag
					inReference = false;
					inMovedRef = false;
					if(i < v.size() - 1) i--;
				}
			}

			if( inPersistentRef )
			{
				//persistent references
				if(v[i].first == "NAME")
				{
					c.persistentRefs.back().name = getString(v[i].second);
					std::cout << std::endl << "object: " << c.persistentRefs.back().name << std::endl;
				} else if(v[i].first == "UNAM")
				{
					c.persistentRefs.back().referenceBocked = getuint8_tInt(v[i].second);
				} else if(v[i].first == "XSCL")
				{
					c.persistentRefs.back().scale = getFloat(v[i].second);
				} else if(v[i].first == "ANAM")
				{
					c.persistentRefs.back().npcID = getString(v[i].second);
				} else if(v[i].first == "BNAM")
				{
					c.persistentRefs.back().globalVariableName = getString(v[i].second);
				} else if(v[i].first == "CNAM")
				{
					c.persistentRefs.back().factionID = getString(v[i].second);
				} else if(v[i].first == "INDX")
				{
					c.persistentRefs.back().factionRank = getLongInt(v[i].second);
				} else if(v[i].first == "XSOL")
				{
					c.persistentRefs.back().soulID = getString(v[i].second);
				} else if(v[i].first == "XCHG")
				{
					c.persistentRefs.back().charge = getFloat(v[i].second);
				} else if(v[i].first == "INTV")
				{
					//depends of object type
					//Health(weapons and armor), uses(locks, probes, repair items)
					c.persistentRefs.back().usesRemaining = getLongInt(v[i].second);

					//time remaining(lights)
					//c.persistentRefs.back().timeRemaining = getFloat(v[i].second);
				} else if(v[i].first == "NAM9")
				{
					c.persistentRefs.back().value = getLongInt(v[i].second);
				} else if(v[i].first == "DODT")
				{
					memmove((char*)&c.persistentRefs.back().ctd, v[i].second.data(), sizeof(c.persistentRefs.back().ctd));
				} else if(v[i].first == "DNAM")
				{
					c.persistentRefs.back().destinationCellName = getString(v[i].second);
				} else if(v[i].first == "FLTV")
				{
					c.persistentRefs.back().lockDifficulty = getLongInt(v[i].second);
				} else if(v[i].first == "KNAM")
				{
					c.persistentRefs.back().keyName = getString(v[i].second);
				} else if(v[i].first == "TNAM")
				{
					c.persistentRefs.back().trapName = getString(v[i].second);
				} else if(v[i].first == "ZNAM")
				{
					c.persistentRefs.back().disabled = getuint8_tInt(v[i].second);
				} else if(v[i].first == "DATA")
				{
					memmove((char*)&c.persistentRefs.back().rpos, v[i].second.data(), sizeof(c.persistentRefs.back().rpos));
				} else
				{
					//is not a persistent reference tag
					inReference = false;
					inPersistentRef = false;
					if(i < v.size() - 1) i--;
				}
			}

			if( inTemporaryRef )
			{
				//Temporary references
			}
		}
	}

	std::cout << c.name << " " << c.regionName << std::endl;
	vcell.push_back(c);
}
void parseLAND(std::vector<char> &buffer){
	
	std::cout << "Parsing LAND tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	LAND l;
	for(auto x : v){
		if(x.first == "INTV") memmove((char*)&l.coord, x.second.data(), sizeof(l.coord));
		if(x.first == "DATA") l.data = getLongInt(x.second);
		if(x.first == "VNML") memmove((char*)&l.vertexNormals, x.second.data(), sizeof(l.vertexNormals));
		if(x.first == "VHGT") memmove((char*)&l.heightData, x.second.data(), sizeof(l.heightData));
		if(x.first == "WNAM") memmove((char*)&l.heights, x.second.data(), sizeof(l.heights));
		if(x.first == "VCLR") memmove((char*)&l.vertexColors, x.second.data(), sizeof(l.vertexColors));
		if(x.first == "VTEX") memmove((char*)&l.textureIndices, x.second.data(), sizeof(l.textureIndices));
	}
	vland.push_back(l);
}

void parsePGRD(std::vector<char> &buffer){
	
	std::cout << "Parsing PGRD tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	PGRD p;
	for(auto x : v){
		std::cout << "TAG:" << x.first << " SIZE:" << x.second.size() << std::endl;
		if(x.first == "DATA") memmove((char*)&p.pathData, x.second.data(), sizeof(p.pathData));
		if(x.first == "NAME") p.name = getString(x.second);
		if(x.first == "PGRP") memmove((char*)&p.pathPoints, x.second.data(), subRecordHeader.size);
		if(x.first == "PGRC") memmove((char*)&p.connectionList, x.second.data(), subRecordHeader.size);
	}
	std::cout << "BEWARE : probably losing some connection points" << std::endl;
	vpgrd.push_back(p);
}

void parseSNDG(std::vector<char> &buffer){
	
	std::cout << "Parsing SNDG tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	SNDG s;
	for(auto x : v){
		if(x.first == "NAME") s.name = getString(x.second);
		if(x.first == "DATA") memmove((char*)&s.soundTypeData, x.second.data(), sizeof(s.soundTypeData));
		if(x.first == "CNAM") s.creatureName = getString(x.second);
		if(x.first == "SNAM") s.soundID = getString(x.second);
	}

	std::cout << s.soundID << " " << s.name << " " << s.creatureName << std::endl;
	vsndg.push_back(s);
}

void parseDIAL(std::vector<char> &buffer){
	
	std::cout << "Parsing DIAL tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	DIAL d;
	for(auto x : v){
		if(x.first == "NAME") d.name = getString(x.second);
		if(x.first == "DATA") d.dialogueType = getuint8_tInt(x.second);
	}
	std::cout << d.name << " " << d.dialogueType << std::endl;
	vdial.push_back(d);
}

void parseINFO(std::vector<char> &buffer){
	
	std::cout << "Parsing INFO tag: " << buffer.size() << " bytes" << std::endl;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse sub-records in the vector
	INFO i;
	for(auto x : v){
		if(x.first == "INAM") i.name = getString(x.second);
		if(x.first == "PNAM") i.previousName = getString(x.second);
		if(x.first == "NNAM") i.nextName = getString(x.second);
		if(x.first == "DATA") memmove((char*)&i.infoData, x.second.data(), sizeof(i.infoData));
		if(x.first == "ONAM") i.actor = getString(x.second);
		if(x.first == "RNAM") i.race = getString(x.second);
		if(x.first == "CNAM") i.actorClass = getString(x.second);
		if(x.first == "FNAM") i.faction = getString(x.second);
		if(x.first == "ANAM") i.cell = getString(x.second);
		if(x.first == "DNAM") i.pcFaction = getString(x.second);
		if(x.first == "SNAM") i.sound = getString(x.second);
		if(x.first == "NAME") i.responseText = getString(x.second);
		if(x.first == "SCVR") i.funcVarlist.push_back(getString(x.second));
		if(x.first == "INTV") i.iscvrComparison = getLongInt(x.second);
		if(x.first == "FLTV") i.fscvrComparison = getFloat(x.second);
		if(x.first == "BNAM") i.resultText = getString(x.second);
		if(x.first == "QSTN") i.questName = getuint8_tInt(x.second);
		if(x.first == "QSTF") i.questFinished = getuint8_tInt(x.second);
		if(x.first == "QSTR") i.questRestart = getuint8_tInt(x.second);
	}
	std::cout << i.name << " " << i.actor << std::endl;
	vdial.back().vinfo.push_back(i);
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

void readESM(const std::string &filename){
    file.open(filename, std::ios::binary);

	while (!file.eof()) {
		//read the record
		readRecordHeader();
		std::string name = std::string(recordHeader.name, recordHeader.name + 4);
		if ( !isValid( name ) )
		{
			std::cout << "Unknown record: " << std::string(recordHeader.name, recordHeader.name + 4) << std::endl;
			break;
		}
		elements[std::string(recordHeader.name, recordHeader.name + 4)] = elements[std::string(recordHeader.name, recordHeader.name + 4)] + 1;

        //read the whole record and go to the next
		std::vector<char> buffer(recordHeader.size);
		if (file.read(buffer.data(), recordHeader.size))
		{
			std::cout << "Data loaded: " << buffer.size() << " bytes" << std::endl;
			
			if (name == "TES3") parseTES3(buffer);
			if (name == "GMST") parseGMST(buffer);
			if (name == "GLOB") parseGLOB(buffer);
			if (name == "CLAS") parseCLAS(buffer);
			if (name == "FACT") parseFACT(buffer);
			if (name == "RACE") parseRACE(buffer);
			if (name == "SOUN") parseSOUN(buffer);
			if (name == "SKIL") parseSKIL(buffer);
			if (name == "MGEF") parseMGEF(buffer);
			if (name == "SCPT") parseSCPT(buffer);
			if (name == "REGN") parseREGN(buffer);
			if (name == "BSGN") parseBSGN(buffer);
			if (name == "LTEX") parseLTEX(buffer);
			if (name == "STAT") parseSTAT(buffer);
			if (name == "DOOR") parseDOOR(buffer);
			if (name == "MISC") parseMISC(buffer);
			if (name == "WEAP") parseWEAP(buffer);
			if (name == "CONT") parseCONT(buffer);
			if (name == "SPEL") parseSPEL(buffer);
			if (name == "CREA") parseCREA(buffer);
			if (name == "BODY") parseBODY(buffer);
			if (name == "LIGH") parseLIGH(buffer);
			if (name == "ENCH") parseENCH(buffer);
			if (name == "NPC_") parseNPC_(buffer);
			if (name == "ARMO") parseARMO(buffer);
			if (name == "CLOT") parseCLOT(buffer);
			if (name == "REPA") parseREPA(buffer);
			if (name == "ACTI") parseACTI(buffer);
			if (name == "APPA") parseAPPA(buffer);
			if (name == "LOCK") parseLOCK(buffer);
			if (name == "PROB") parsePROB(buffer);
			if (name == "INGR") parseINGR(buffer);
			if (name == "BOOK") parseBOOK(buffer);
			if (name == "ALCH") parseALCH(buffer);
			if (name == "LEVI") parseLEVI(buffer);
			if (name == "LEVC") parseLEVC(buffer);
			if (name == "CELL") parseCELL(buffer);
			if (name == "LAND") parseLAND(buffer);
			if (name == "PGRD") parsePGRD(buffer);
			if (name == "SNDG") parseSNDG(buffer);
			if (name == "DIAL") parseDIAL(buffer);
			if (name == "INFO") parseINFO(buffer);
		}
		else
		{
			std::cout << "Error loading data" << std::endl;
			break;
		}
    }

    file.close();

	//Morrowind original number of records:
	originals["TES3"] = 1;
	originals["GMST"] = 1428;
	originals["GLOB"] = 73;
	originals["CLAS"] = 77;
	originals["FACT"] = 22;
	originals["RACE"] = 10;
	originals["SOUN"] = 430;
	originals["SKIL"] = 27;
	originals["MGEF"] = 137;
	originals["SCPT"] = 631;
	originals["REGN"] = 9;
	originals["BSGN"] = 13;
	originals["LTEX"] = 107;
	originals["STAT"] = 2788;
	originals["DOOR"] = 140;
	originals["MISC"] = 536;
	originals["WEAP"] = 485;
	originals["CONT"] = 890;
	originals["SPEL"] = 982;
	originals["CREA"] = 260;
	originals["BODY"] = 1125;
	originals["LIGH"] = 574;
	originals["ENCH"] = 708;
	originals["NPC_"] = 2675;
	originals["ARMO"] = 280;
	originals["CLOT"] = 510;
	originals["REPA"] = 6;
	originals["ACTI"] = 697;
	originals["APPA"] = 22;
	originals["LOCK"] = 6;
	originals["PROB"] = 6;
	originals["INGR"] = 95;
	originals["BOOK"] = 574;
	originals["ALCH"] = 258;
	originals["LEVI"] = 227;
	originals["LEVC"] = 116;
	originals["CELL"] = 2538;
	originals["LAND"] = 1390;
	originals["PGRD"] = 1194;
	originals["SNDG"] = 168;
	originals["DIAL"] = 772;
	originals["INFO"] = 3408;

	std::map<std::string, int>::iterator it;
	for (it = elements.begin(); it != elements.end(); it++) {
		std::cout << it->first << " : " << it->second << " (" << originals[it->first] << ")" << std::endl;
	}

}

int main(){
    readESM("c:/JuegosEstudio/Morrowind/Data Files/morrowind.esm");

	return 0;
}