#include <filesystem>
#include <fstream> 
#include <iostream>
#include <thread>
#include <vector>

#include <boost/format.hpp>

#include "messages.h"

namespace fs = std::filesystem;
short result = 0;	
int all_rows = 0;


void read_file(const fs::path& filename)
{
	const char delim = '\n';
	size_t end = 0;	
	short count = 0;
	
	std::string buf(
		(std::istreambuf_iterator<char>(
			*(std::unique_ptr<std::ifstream>(
				new std::basic_ifstream<char>(filename)
			)).get()
		)),
		std::istreambuf_iterator<char>()
	);
			
	while ((end = buf.find(delim, end)) != std::string::npos) {
		while (buf[end++] == delim) {
			++count;
		}
	}
	all_rows += count;
}


int main(int argc, char *argv[]) {

	try {
		if ((argc != 2) or !fs::is_directory(argv[1])) {
			throw((boost::format(Err_dir) % argv[0]).str());
		}		
		
		std::vector<std::thread> thread_group; 
		
		for (const auto dir_entry : fs::directory_iterator{argv[1]}) {		
			thread_group.push_back(std::thread(&read_file, dir_entry.path()));
		}

		for (auto &t : thread_group) {
			if (t.joinable()) {
				t.join();
			}
		}
		std::cout << all_rows; 
	}
	catch(const std::string s) {
		std::cout << s;
		result = -1;
	}	
	return result;
}
