#include<iostream>
#include<vector>
#include<algorithm>
#include <fstream> //read/write files
#include <string>
#include <map>

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

void parseTES3(std::vector<char> &buffer){
	std::cout << "Parsing TES3 tag: " << buffer.size() << " bytes" << std::endl;

	//read the sub-records
	int bytesRead = 0;
	int tags = 0;
	int index = 0;
	while (bytesRead < buffer.size()) {
		std::string subRecordName = std::string(buffer.data() + index, 4);
		long int subRecordSize = 0;
		memmove(&subRecordSize, &buffer[index + 4], sizeof(subRecordSize));
		std::cout << "Name: " << subRecordName << std::endl;
		std::cout << "Size: " << subRecordSize << std::endl;
		bytesRead += 8;
		index += subRecordSize + 8;
		bytesRead += subRecordSize;
		tags++;
	}
	std::cout << tags << " tags found" << std::endl;
}
void parseGMST(std::vector<char> &buffer){}
void parseGLOB(std::vector<char> &buffer){}
void parseCLAS(std::vector<char> &buffer){}
void parseFACT(std::vector<char> &buffer){}
void parseRACE(std::vector<char> &buffer){}
void parseSOUN(std::vector<char> &buffer){}
void parseSKIL(std::vector<char> &buffer){}
void parseMGEF(std::vector<char> &buffer){}
void parseSCPT(std::vector<char> &buffer){}
void parseREGN(std::vector<char> &buffer){}
void parseBSGN(std::vector<char> &buffer){}
void parseLTEX(std::vector<char> &buffer){}
void parseSTAT(std::vector<char> &buffer){}
void parseDOOR(std::vector<char> &buffer){}
void parseMISC(std::vector<char> &buffer){}
void parseWEAP(std::vector<char> &buffer){}
void parseCONT(std::vector<char> &buffer){}
void parseSPEL(std::vector<char> &buffer){}
void parseCREA(std::vector<char> &buffer){}
void parseBODY(std::vector<char> &buffer){}
void parseLIGH(std::vector<char> &buffer){}
void parseENCH(std::vector<char> &buffer){}
void parseNPC_(std::vector<char> &buffer){}
void parseARMO(std::vector<char> &buffer){}
void parseCLOT(std::vector<char> &buffer){}
void parseREPA(std::vector<char> &buffer){}
void parseACTI(std::vector<char> &buffer){}
void parseAPPA(std::vector<char> &buffer){}
void parseLOCK(std::vector<char> &buffer){}
void parsePROB(std::vector<char> &buffer){}
void parseINGR(std::vector<char> &buffer){}
void parseBOOK(std::vector<char> &buffer){}
void parseALCH(std::vector<char> &buffer){}
void parseLEVI(std::vector<char> &buffer){}
void parseLEVC(std::vector<char> &buffer){}
void parseCELL(std::vector<char> &buffer){}
void parseLAND(std::vector<char> &buffer){}
void parsePGRD(std::vector<char> &buffer){}
void parseSNDG(std::vector<char> &buffer){}
void parseDIAL(std::vector<char> &buffer){}
void parseINFO(std::vector<char> &buffer){}

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