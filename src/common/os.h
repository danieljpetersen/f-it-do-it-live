#ifndef FI_OS_H
#define FI_OS_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>
#include "filesystem/fwd.h"
#include "filesystem/path.h"
#include "filesystem/resolver.h"
#include "util.h"

#if __linux
	#include <unistd.h>
	#include <sys/stat.h>
	#include <dirent.h>

#elif _WIN32
	#include  <cctype>
	#include <windows.h>
	#include <minwindef.h>
	#include <libloaderapi.h>
	#include <math.h>	
	#include "dirent.h"
#endif

#include <iostream>

namespace fi
{
	class Operating_System
	{
	private:
		std::string _PathToExecutableDirectory;

		void _calculatePathToExecutableDirectory()
		{
			// ---- init path to executable directory
			{
				#ifdef _WIN32
					char ownPth[MAX_PATH];

					HMODULE hModule = GetModuleHandle(NULL);
					GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
					_PathToExecutableDirectory = std::string(ownPth);

					// strip filename
					size_t lastindex = _PathToExecutableDirectory.find_last_of("\\");
					std::string rawname = _PathToExecutableDirectory.substr(0, lastindex);
					_PathToExecutableDirectory = rawname + "\\";
						
				#elif __linux
					// get executable path, then remove executable to get the directory path
					#define PATH_MAX 4096    /* # chars in a path name including nul */
					char result[PATH_MAX];
					ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
					_PathToExecutableDirectory = std::string(result, (count > 0) ? count : 0);

					sf::String s = _PathToExecutableDirectory;
                    int Count = 0, SlashCount = 0, DesiredSlashCount = 1;
                    for (int i = (signed)s.getSize() - 1; i >= 0; i--)
                    {
                        if ((_PathToExecutableDirectory.at(i) == '/') || (_PathToExecutableDirectory.at(i) == '\\'))
                        {
                            SlashCount++;
                            if (SlashCount == DesiredSlashCount)
                            {
                                break;
                            }
                        }
                        Count++;
                    }

                    // remove executable name.  we're just trying to get the executable dir
                    for (int i = 0; i < Count; i++)
                    {
                        _PathToExecutableDirectory.pop_back();
                    }
				#endif
			}
		}

	public:
		std::string getPathToExecutableDirectory()
		{
			if (_PathToExecutableDirectory.empty())
			{
				_calculatePathToExecutableDirectory();
			}

			return _PathToExecutableDirectory;
		}

		////////////////////////////////////////////////////////////

		std::string getPathToSaveDirectory()
		{
			return getPathToExecutableDirectory() + "saves/";
		}
		
		////////////////////////////////////////////////////////////

		bool isLinux()
		{
			#if __linux
				return true;
			#endif

			return false;
		}

		////////////////////////////////////////////////////////////

		bool isWindows()
		{
			#if _WIN32
				return true;
			#endif

			return false;
		}

		////////////////////////////////////////////////////////////

		bool fileExists(const std::string &Path)
		{
			struct stat buffer{};
			return (stat (Path.c_str(), &buffer) == 0);
		}

		////////////////////////////////////////////////////////////

		bool removeFile(const std::string &Path)
		{
			return remove(Path.c_str()) == 0;
		}

		////////////////////////////////////////////////////////////

		std::vector<std::string> getFilesInDirectory(const std::string &Path)
		{
			std::vector<std::string> ReturnValue;
			if (auto dir = opendir(Path.c_str()))
			{
				while (auto f = readdir(dir))
				{
					if (!f->d_name || f->d_name[0] == '.')
					{
						continue;
					}

					ReturnValue.emplace_back(f->d_name);
				}

				closedir(dir);
			}

			return ReturnValue;
		}

		////////////////////////////////////////////////////////////

		bool extensionCheck (std::string const &path, std::string const &ending) {
			if (path.length() >= ending.length()) {
				return (0 == path.compare (path.length() - ending.length(), ending.length(), ending));
			} else {
				return false;
			}
		}

		////////////////////////////////////////////////////////////
		
		void createFolderIfNotExist(const std::string &Path)
		{
			filesystem::path path(Path);
			if (!path.exists())
			{
				filesystem::create_directory(Path);
			}
		}

		////////////////////////////////////////////////////////////

		std::string readTextFile(const std::string &FilePath)
		{
			if (!fileExists(FilePath))
				return "";

			std::ifstream fin(FilePath);

			if (!fin.is_open())
			{
				return "";
			}

			std::string Result;
			std::string line;

			int i = 0;
			while(getline(fin, line))
			{
				Result += line + "\n";
				i++;
			}

			return Result;
		}

		////////////////////////////////////////////////////////////

		std::vector<std::string> readTextFile_returnLines(const std::string &FilePath)
		{
			if (!fileExists(FilePath))
				return std::vector<std::string>();

			std::ifstream fin(FilePath);

			if (!fin.is_open())
			{
				return std::vector<std::string>();
			}

			std::vector<std::string> Result;
			std::string line;

			int i = 0;
			while(getline(fin, line))
			{
				Result.push_back(line);
				i++;
			}

			return Result;
		}

		////////////////////////////////////////////////////////////

		std::string readLine(int LineIndex, const std::string &FilePath)
		{
			if (!fileExists(FilePath))
				return "";

			std::ifstream fin(FilePath);

			if (!fin.is_open())
			{
				return "";
			}

			std::string line;

			int i = 0;
			while(getline(fin, line))
			{
				if (i == LineIndex)
				{
					return line;
				}

				i++;
			}

			return "";
		}

		////////////////////////////////////////////////////////////

		bool writeToFile(const std::string &Path, const std::string &Value)
		{
			std::ofstream out(Path);
			out << Value;
			out.close();
			return true;
		}
		
		////////////////////////////////////////////////////////////
		
		bool appendToFile(const std::string &Path, std::string Value)
		{
			std::ofstream file;

			//can't _enable exception now because of gcc bug that raises ios_base::failure with useless message
			//file.exceptions(file.exceptions() | std::ios::failbit);
			file.open(Path, std::ios::out | std::ios::app);
			if (file.fail())
				return false;

			//make sure write fails with exception if something is wrong
			file.exceptions(file.exceptions() | std::ios::failbit | std::ifstream::badbit);

			file << Value << std::endl;

			return true;
		}
	};
}

#endif