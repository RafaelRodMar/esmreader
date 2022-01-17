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

void parseTES3(std::vector<char> &buffer){
	std::cout << "Parsing TES3 tag: " << buffer.size() << " bytes" << std::endl;
	//std::pair<std::string, std::vector<char> > p;
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
 			char temp[300];
 			memmove((char*)&temp, v[i].second.data(), v[i].second.size());
 			std::cout << "  Required master file: " << std::string(temp, temp + v[i].second.size());
 		}

 		if (v[i].first == "DATA")
 		{
 			long long int aux = 0;
 			memmove((char*)&aux, v[i].second.data(), sizeof(aux));
 			std::cout << "  Size of the required master file: " << aux << std::endl;
 		}
	}
}

void parseGMST(std::vector<char> &buffer){
	
	std::cout << "Parsing GMST tag: " << buffer.size() << " bytes" << std::endl;
	//std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;

	//parse the sub-records in the vector
	GMST g;
	for(auto x : v){
		if(x.first == "NAME")
		{
			char temp[300];
 			memmove((char*)&temp, x.second.data(), x.second.size());
			int i = 0;
			while(temp[i]!=0 && i < x.second.size()){
				i++;
			} 
			std::cout << "  Name: " << std::string(temp, temp + i) << std::endl;
			g.name = std::string(temp, temp + i);
		}

		if (x.first == "STRV")
 		{
 			char temp[300];
 			memmove((char*)&temp, x.second.data(), x.second.size());
			int i = 0;			 
			while(temp[i]!=0 && i < x.second.size()){
				i++;
			}  
			std::cout << "  String: " << std::string(temp, temp + i) << std::endl;
			g.stringValue = std::string(temp, temp + i);
 		}

		 if (x.first == "INTV")
 		{
 			long int aux = 0;
			memmove((char*)&aux, x.second.data(), sizeof(aux));
 			std::cout << "  Integer Value: " << aux << std::endl;
 			g.intValue = aux;
 		}

 		if (x.first == "FLTV")
 		{
 			float aux = 0;
 			memmove((char*)&aux, x.second.data(), sizeof(aux));
 			std::cout << "  Float Value: " << aux << std::endl;
 			g.floatValue = aux;
 		}
	}
	vgmst.push_back(g);
}
void parseGLOB(std::vector<char> &buffer){
	
	std::cout << "Parsing GLOB tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseCLAS(std::vector<char> &buffer){
	
	std::cout << "Parsing CLAS tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseFACT(std::vector<char> &buffer){
	
	std::cout << "Parsing FACT tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseRACE(std::vector<char> &buffer){
	
	std::cout << "Parsing RACE tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseSOUN(std::vector<char> &buffer){
	
	std::cout << "Parsing SOUN tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseSKIL(std::vector<char> &buffer){
	
	std::cout << "Parsing SKIL tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseMGEF(std::vector<char> &buffer){
	
	std::cout << "Parsing MGEF tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseSCPT(std::vector<char> &buffer){
	
	std::cout << "Parsing SCPT tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseREGN(std::vector<char> &buffer){
	
	std::cout << "Parsing REGN tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseBSGN(std::vector<char> &buffer){
	
	std::cout << "Parsing BSGN tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseLTEX(std::vector<char> &buffer){
	
	std::cout << "Parsing LTEX tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseSTAT(std::vector<char> &buffer){
	
	std::cout << "Parsing STAT tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseDOOR(std::vector<char> &buffer){
	
	std::cout << "Parsing DOOR tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseMISC(std::vector<char> &buffer){
	
	std::cout << "Parsing MISC tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseWEAP(std::vector<char> &buffer){
	
	std::cout << "Parsing WEAP tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseCONT(std::vector<char> &buffer){
	
	std::cout << "Parsing CONT tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseSPEL(std::vector<char> &buffer){
	
	std::cout << "Parsing SPEL tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseCREA(std::vector<char> &buffer){
	
	std::cout << "Parsing CREA tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseBODY(std::vector<char> &buffer){
	
	std::cout << "Parsing BODY tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseLIGH(std::vector<char> &buffer){
	
	std::cout << "Parsing LIGH tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseENCH(std::vector<char> &buffer){
	
	std::cout << "Parsing ENCH tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseNPC_(std::vector<char> &buffer){
	
	std::cout << "Parsing NPC_ tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseARMO(std::vector<char> &buffer){
	
	std::cout << "Parsing ARMO tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseCLOT(std::vector<char> &buffer){
	
	std::cout << "Parsing CLOT tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseREPA(std::vector<char> &buffer){
	
	std::cout << "Parsing REPA tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseACTI(std::vector<char> &buffer){
	
	std::cout << "Parsing ACTI tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseAPPA(std::vector<char> &buffer){
	
	std::cout << "Parsing APPA tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseLOCK(std::vector<char> &buffer){
	
	std::cout << "Parsing LOCK tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parsePROB(std::vector<char> &buffer){
	
	std::cout << "Parsing PROB tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseINGR(std::vector<char> &buffer){
	
	std::cout << "Parsing INGR tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseBOOK(std::vector<char> &buffer){
	
	std::cout << "Parsing BOOK tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseALCH(std::vector<char> &buffer){
	
	std::cout << "Parsing ALCH tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseLEVI(std::vector<char> &buffer){
	
	std::cout << "Parsing LEVI tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseLEVC(std::vector<char> &buffer){
	
	std::cout << "Parsing LEVC tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseCELL(std::vector<char> &buffer){
	
	std::cout << "Parsing CELL tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseLAND(std::vector<char> &buffer){
	
	std::cout << "Parsing LAND tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parsePGRD(std::vector<char> &buffer){
	
	std::cout << "Parsing PGRD tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseSNDG(std::vector<char> &buffer){
	
	std::cout << "Parsing SNDG tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseDIAL(std::vector<char> &buffer){
	
	std::cout << "Parsing DIAL tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
}
void parseINFO(std::vector<char> &buffer){
	
	std::cout << "Parsing INFO tag: " << buffer.size() << " bytes" << std::endl;
	std::pair<std::string, std::vector<char> > p;
	std::vector< std::pair<std::string, std::vector<char>> > v;

	//read the sub-records
	v = getSubRecordData(buffer);
	std::cout << "Tags in vector: " << v.size() << std::endl;
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
			/*if (name == "GLOB") parseGLOB(buffer);
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
			if (name == "INFO") parseINFO(buffer);*/
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