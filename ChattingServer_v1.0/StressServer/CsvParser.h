#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <istream>


std::vector<std::string> readCsv(std::istream& file);

bool parseCsv(std::vector<std::string>& strs, const std::string filePath);