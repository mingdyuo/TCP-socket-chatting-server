#include "csv_parser.h"





bool parseCsv(vector<string>& strs, const string filePath)
{
	ifstream file(filePath);

	if (file.fail())
	{
		return (cout << "띠용 ! 파일이 없어요.\n") && 0;
	}

	if(file.good())
		csv_read_row(file, strs);

	file.close();

	return true;
}

 void csv_read_row(istream& in, vector<string>& items)
{
	stringstream ss;

	while (in.good()) {
		char c = in.get();

		if (c == '\r' || c == '\n')
		{
			if (in.eof()) return;
			if (in.peek() == '\n') { in.get(); }
			items.push_back(ss.str());
			ss.str("");
		}
		else {
			ss << c;
		}
	}
	return;
}