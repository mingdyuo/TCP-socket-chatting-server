#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <istream>

using namespace std;

void csv_read_row(std::istream& file, std::vector<std::string>&);

bool parseCsv(std::vector<std::string>& strs, const std::string filePath);