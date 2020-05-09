#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

struct Line{
	std::vector< std::string > words;
};

struct File {
	std::string path;
	std::vector< Line > lines;
	bool touched;

	File() : touched(false) {}
};

std::vector< File > files;

int HexStrToInt(const std::string& str)
{
	int ret;
	std::stringstream ss;
	ss << std::hex << str;
	ss >> ret;
	return ret;
}

std::string IntToHexStr(int value)
{
	std::stringstream stream;
	stream.width(2);
	stream.fill('0');
	stream << std::hex << value;
	return stream.str();
}

struct Symbol {
public:
	std::string path;
	std::string area;
	std::string name;
	int rel_address;

	int GetBank() {
		if(area.find_first_of("_CODE") == 0) {
			return atoi(area.substr(area.find_last_of('_') + 1).c_str());
		}
		return 0;
	}

public:
	Symbol() : rel_address(0) {}
	
	Symbol(const std::string _path, const std::string& _area, const std::string& _name, int _rel_address) : 
		path(_path), area(_area), name(_name), rel_address(_rel_address) 
	{
	}
};

std::map< std::string, Symbol > symbols;

File Parse(const char* path) {
	std::string line;
	File ret;
	ret.path = path;
	
	std::fstream file(path);
	if(file.is_open()){
		while(!file.eof()) {
			getline(file, line);

			if(line.empty())
				continue;

			Line l;

			std::istringstream iss(line);
			for(std::string word; iss >> word;) {
				l.words.push_back(word);
			}

			ret.lines.push_back(l);
		}
		file.close();
	}

	return ret;
}

void Write(const File& _file) {
	std::ofstream file(_file.path);
	if(file.is_open()) {
		for(int line_idx = 0; line_idx < _file.lines.size(); ++ line_idx) {
			const Line& line = _file.lines[line_idx];
			for(int word_idx = 0; word_idx < line.words.size(); ++ word_idx) {
				if(word_idx != 0) {
 					file << ' ';
				}
				file << line.words[word_idx];
			}
			file << "\n";
		}
		file.close();
	}
}

void LoadSymbols(const File& file) {
	std::string current_area = "";
	for(size_t l = 0; l < file.lines.size(); ++ l) {
		const Line&  line = file.lines[l];
			
		if(line.words[0] == "A") {
			current_area = line.words[1];
		}

		if(line.words[0] == "S") {
			if(line.words[2].find_first_of("Def") == 0) {
				//Symbol is defined here
				Symbol s(file.path, current_area, line.words[1], HexStrToInt(line.words[2].substr(3)));
				symbols[line.words[1]] = s;
			}
		}
	}
}

struct Area
{
	std::vector< Symbol > symbols;
};

Symbol GetOrAddSymbol(const std::string symbol_name) {
	std::map< std::string, Symbol >::iterator it = symbols.find(symbol_name);
	if(it == symbols.end()) {
		//Let's assume symbols not found are in bank 0... for now
		Symbol s("", "_CODE", symbol_name, 0);
		symbols[symbol_name] = s;

		return s;
	}
	else {
		return (*it).second;
	}
}

void FixBankedCall(File& file) {
	if(file.lines[0].words[0] != "XL2")
	{
		printf("Error: Format(%s) Only XL2 format is supported\n", file.lines[0].words[0].c_str());
		return;
	}

	std::vector< Area > areas;
	areas.push_back(Area());
	bool far_call_detected = false;

	for(size_t l = 1; l < file.lines.size(); ++ l) {
		Line&  line = file.lines[l];
			
		if(line.words[0] == "A") {
			areas.push_back(Area());
		}

		if(line.words[0] == "S") {
			Area& current_area = areas[areas.size() - 1];
			current_area.symbols.push_back(GetOrAddSymbol(line.words[1]));
		}

		if(line.words[0] == "T") {
		}

		if(line.words[0] == "R") {
			int current_area_idx = HexStrToInt(line.words[4] + line.words[3]);

			size_t idx = 5;

			while(idx < line.words.size())
			{
				int rmode_oformat = HexStrToInt(line.words[idx]);
				if((rmode_oformat & 0xF0) == 0xF0) {
					rmode_oformat = 0xFFF & HexStrToInt(line.words[idx] + line.words[idx + 1]);
					idx += 2;
				} else {
					idx += 1;
				}
				
				unsigned int byte_idx = HexStrToInt(line.words[idx]) + 1; //First character is 'T'
				idx += 1;

				int area_symbol_idx = HexStrToInt(line.words[idx + 1] + line.words[idx]);
				idx += 2;

				Symbol *symbol = 0;
				if(rmode_oformat & 2) {
					//Relocatable symbol
					symbol = &areas[0].symbols[area_symbol_idx];
				} else {
					//Relocatable area
					int rel_address = HexStrToInt(file.lines[l - 1].words[byte_idx + 1] + file.lines[l - 1].words[byte_idx]);
					std::vector< Symbol >& symbols = areas[area_symbol_idx + 1].symbols;
					for(std::vector< Symbol >::iterator it = symbols.begin(); it != symbols.end(); ++ it) {
						if((*it).rel_address == rel_address) {
							symbol = &(*it);
						}
					}
				}

				if(!far_call_detected) {
					if(symbol && symbol->name == "banked_call") {
						far_call_detected = true;
					}
				} else {
					if(symbol) {
						int patch_line = l - 1;
						byte_idx += 2; //currently pointing to the first byte of a short value 
						//Locate the line to make the patch
						while(byte_idx > file.lines[patch_line].words.size()) {
							byte_idx -= file.lines[patch_line].words.size();
							byte_idx += 1; //each new line adds 1 extra char 'T' at the beggining
						
							//Search next line "T"
							do {
								patch_line ++;
							} while(file.lines[patch_line].words[0] != "T");
						}

						//Patch
						file.lines[patch_line].words[byte_idx] = IntToHexStr(symbol->GetBank());
						file.touched = true;
					}

					far_call_detected = false;
				}
			}
		}
	}
}

void main(int argc, char* argv[]){
	//Parse files
	for(int i = 1; i < argc; ++ i) {
		files.push_back(Parse(argv[i]));
	}

	//Load symbols from all files
	for(size_t i = 0; i < files.size(); ++i) {
		LoadSymbols(files[i]);
	}

	//Fix files
	for(size_t i = 0; i < files.size(); ++i) {
		FixBankedCall(files[i]);
	}

	//Write fixed files
	for(size_t i = 0; i < files.size(); ++i) {
		if(files[i].touched) {
			Write(files[i]);
		}
	}
}