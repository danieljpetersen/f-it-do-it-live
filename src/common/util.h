#ifndef FI_UTIL_H
#define FI_UTIL_H

#include <SFML/Graphics.hpp>
#include <deque>
#include "nlohmann_json.h"
#include "cereal/cereal.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/archives/json.hpp"
#include "imgui/imgui.h"

namespace sf
{
    template<class Archive>
    void serialize(Archive & archive,
                   sf::Vector2f & m)
    {
        archive( m.x, m.y );
    }

    template<class Archive>
    void serialize(Archive & archive,
                   sf::Color & m)
    {
        archive( m.r, m.g, m.b, m.a );
    }


}

ImVec2 operator+(ImVec2 a, ImVec2 const& b);
ImVec2 operator-(ImVec2 a, ImVec2 const& b);
sf::Vector2f operator+(sf::Vector2f a, sf::Vector2i const& b);
sf::Vector2f operator-(sf::Vector2f a, sf::Vector2f const& b);
namespace fi
{
    void log(sf::Color Color);
	void log(sf::Vector2i Value);
	void log(sf::Vector2f Value);
	void log(std::string Value);
	void log(unsigned int Value);
	void log(int Value);
	void log(float Value);
	void log(std::stringstream Value);

	// ----

    std::string to_s(sf::Color Color);
    std::string to_s(float Num);
    std::string to_s(float Num, int NumberOfDecimals);
    std::string to_s(int Num);
    std::string to_s(unsigned int Num);
    std::string to_s(long Num);
    std::string to_s(long long Num);
    std::string to_s(sf::Vector2f Value);
    std::string to_s(sf::Vector2i Value);
    std::string to_s_withCommas(long long n);

    // ----

    void stringDeleteCharAt(std::string &Source, unsigned int Index);
    std::vector<std::string>& splitString(const std::string s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> splitString(const std::string s, char delim);
    std::string stripComment(std::string input);
    std::string removeLastCharacterFromString(std::string input);
    std::string trimStringOfWhitespace(std::string input);
    std::string stringDowncase(std::string str);
    std::string datetime_to_s();

    // ----

    std::string jsonLoadStr(std::string FullPathToJSONFile, std::string &err);
    nlohmann::json jsonLoad(std::string FullPathToJSONFile, bool QuitIfError);
    void jsonSaveToFile(std::string Path, std::string jsonStr);
    std::string jsonGetErrorFromParsing(std::string str);
    sf::Color jsonGetColor(nlohmann::json Config, std::string Field);
    std::vector<std::string> jsonGetVectorOfStrings(nlohmann::json Config, std::string Field);

    // ----

    sf::Vector2i convert(sf::Vector2f value);
    sf::Vector2f convert(sf::Vector2i value);

    bool vectorContains(std::vector<int> *Vector, int Value);
    bool vectorContains(std::deque<int> *Vector, int Value);
    void vectorShuffle(std::vector<int> *Vector, int NumberOfTimes);
    std::vector<std::string> removeElementsFromVectorOfStringsNotContainingSubstr(std::vector<std::string> strings, std::string substr);



    // ----

    struct sort_high_to_low
    {
        template<class T>
        bool operator()(T const &a, T const &b) const { return a > b; }
    };

    // ----
}


#endif