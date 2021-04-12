#include "csv_parser.h"


bool parseCsv(std::vector<std::string>& strs, const std::string filePath)
{
	std::ifstream file(filePath);

	if (file.fail())
	{
		std::cout << "띠용 ! 파일이 없어요.\n";
		return false;
	}

	if(file.good())
		strs = readCsv(file);

	file.close();

	return true;
}

std::vector<std::string> readCsv(std::istream& in)
{
	std::stringstream ss;
	std::vector<std::string> items;

	while (in.good()) {
		char c = in.get();

		if (c == '\r' || c == '\n')
		{
			if (in.eof())
			{
				return items;
			}
			else if (in.peek() == '\n') 
			{ 
				in.get(); 
			}

			items.push_back(ss.str());
			ss.str("");
		}
		else 
		{
			ss << c;
		}
	}
	return items;
}