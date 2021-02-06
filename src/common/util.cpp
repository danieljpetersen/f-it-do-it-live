#include <utility>

#include <sstream>
#include <iostream>
#include <cmath>
#include "util.h"
#include "engine.h"
#include "picojson.h"

////////////////////////////////////////////////////////////

ImVec2 operator+(ImVec2 a, ImVec2 const& b) {
    a.x = a.x + b.x;
    a.y = a.y + b.y;
    return a;
}

////////////////////////////////////////////////////////////

ImVec2 operator-(ImVec2 a, ImVec2 const& b) {
    a.x = a.x - b.x;
    a.y = a.y - b.y;
    return a;
}

////////////////////////////////////////////////////////////

sf::Vector2f operator+(sf::Vector2f a, sf::Vector2i const& b) {
    a.x = a.x + b.x;
    a.y = a.y + b.y;
    return a;
}

////////////////////////////////////////////////////////////

sf::Vector2f operator-(sf::Vector2f a, sf::Vector2f const& b) {
    a.x = a.x - b.x;
    a.y = a.y - b.y;
    return a;
}

////////////////////////////////////////////////////////////

void fi::log(sf::Color Color)
{
    std::string Result = to_s(Color);
    log(Result);
}

////////////////////////////////////////////////////////////

void fi::log(sf::Vector2i Value)
{
    std::string FinalResult = to_s(Value.x) + ", " + to_s(Value.y);
    log(FinalResult);
}

////////////////////////////////////////////////////////////

void fi::log(sf::Vector2f Value)
{
    std::string FinalResult = to_s(Value.x) + ", " + to_s(Value.y);
    log(FinalResult);
}

////////////////////////////////////////////////////////////

void fi::log(std::string Value)
{
    std::stringstream ss;
    ss << Value;
    ss << "\n";
    std::cout << ss.str();
}

////////////////////////////////////////////////////////////

void fi::log(int Value)
{
    std::stringstream ss;
    ss << Value;
    log(ss.str());
}

////////////////////////////////////////////////////////////

void fi::log(float Value)
{
    std::stringstream ss;
    ss << Value;
    log(ss.str());
}

////////////////////////////////////////////////////////////

void fi::log(std::stringstream Value)
{
    log(Value.str());
}

////////////////////////////////////////////////////////////

void fi::log(unsigned int Value)
{
    std::stringstream ss;
    ss << Value;
    log(ss.str());
}

////////////////////////////////////////////////////////////

std::string fi::to_s(sf::Color Color)
{
    return std::string("r:  " + to_s(Color.r) + ",  g:  " + to_s(Color.g) + ",  b:  " + to_s(Color.b) + ", " + to_s(Color.a));
}

////////////////////////////////////////////////////////////

std::string fi::to_s(float Num)
{
    std::stringstream ss;
    ss << Num;
    if (std::floor(Num) == Num)//if has no decimal
        ss << ".0";
    return ss.str();
}

////////////////////////////////////////////////////////////

std::string fi::to_s(float Num, int NumberOfDecimals)
{
    std::stringstream ss;

    int b = (int)(Num*std::pow(10, NumberOfDecimals));
    Num = (float)(b / (std::pow(10, NumberOfDecimals)));
    ss << Num;

    if (NumberOfDecimals != 0)
        if (std::floor(Num) == Num)//if has no decimal
        {
            ss << ".0";
        }
    return ss.str();
}

////////////////////////////////////////////////////////////

std::string fi::to_s(int Num)
{
    std::stringstream ss;
    ss << Num;
    return ss.str();
}

////////////////////////////////////////////////////////////

std::string fi::to_s(unsigned int Num)
{
    std::stringstream ss;
    ss << Num;
    return ss.str();
}

////////////////////////////////////////////////////////////

std::string fi::to_s(long Num)
{
    std::stringstream ss;
    ss << Num;
    return ss.str();
}

////////////////////////////////////////////////////////////

std::string fi::to_s(long long Num)
{
    std::stringstream ss;
    ss << Num;
    return ss.str();
}

////////////////////////////////////////////////////////////

std::string fi::to_s(sf::Vector2f Value)
{
    std::string FinalResult = fi::to_s(Value.x) + ", " + fi::to_s(Value.y);
    return FinalResult;
}

////////////////////////////////////////////////////////////

std::string fi::to_s(sf::Vector2i Value)
{
    std::string FinalResult = fi::to_s(Value.x) + ", " + fi::to_s(Value.y);
    return FinalResult;
}

////////////////////////////////////////////////////////////

std::string fi::to_s_withCommas(long long n)
{
    bool negative = (n < 0);
    if (negative) n *= -1; // think positive!

    // convert the integer into a string.
    std::ostringstream result;
    result << n;
    std::string number = result.str();

    // format it to include comma seperators
    int length = (signed int)number.length(), i;
    std::string formatted = "";

    for (i = length - 3; i >= 0; i -= 3)
        if (i > 0)
            formatted = ',' + number.substr(i, 3) + formatted;
        else
            formatted = number.substr(i, 3) + formatted;
    if (i < 0)
        formatted = number.substr(0, 3 + i) + formatted;
    if (negative)
        formatted = "-" + formatted;

    return formatted;
}

////////////////////////////////////////////////////////////

std::string fi::stripComment(std::string input)
{
    unsigned long CommentIndex = input.find("//");
    return input.substr(0, CommentIndex);
}

////////////////////////////////////////////////////////////

std::string fi::removeLastCharacterFromString(std::string input)
{
    return input.substr(0, input.size()-1);
}

////////////////////////////////////////////////////////////

std::string fi::trimStringOfWhitespace(std::string input)
{
    const char* t = " \t\n\r\f\v";
    input.erase(0, input.find_first_not_of(t));
    input.erase(input.find_last_not_of(t) + 1);
    return input;
}

////////////////////////////////////////////////////////////

std::string fi::stringDowncase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

////////////////////////////////////////////////////////////

std::string fi::datetime_to_s()
{
    std::time_t rawtime;
    std::tm* timeinfo;
    char buffer [80];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer,80,"%Y-%m-%d-%H-%M-%S", timeinfo);
    return buffer;
}

////////////////////////////////////////////////////////////

std::string fi::jsonLoadStr(std::string FullPathToJSONFile, std::string &err)
{
    // there are better ways to do this but this is what i did at the time.

    if (!fi::getOS().fileExists(FullPathToJSONFile))
    {
        err = "empty json file";
        return "";
    }

    std::ifstream File(FullPathToJSONFile);
    std::string jsonStr, Previous;
    for( std::string line; getline(File, line ); )
    {
        line = fi::stripComment(line);
        line = fi::trimStringOfWhitespace(line);
        if (!Previous.empty())
        {
            if (Previous.at(Previous.size()-1) == ',')
            {
                if (!line.empty())
                {
                    if ((line.at(0) == '}') || (line.at(0) == ']'))
                    {
                        jsonStr = fi::removeLastCharacterFromString(jsonStr);
                    }
                }
            }
        }
        jsonStr += "\n" + line;

        if (!line.empty())
        {
            Previous = line;
        }
    }

    err = fi::jsonGetErrorFromParsing(jsonStr);

    return jsonStr;
}

////////////////////////////////////////////////////////////

nlohmann::json fi::jsonLoad(std::string FullPathToJSONFile, bool QuitIfError)
{
    std::string err;
    std::string jsonStr = jsonLoadStr(std::move(FullPathToJSONFile), err);

    if (!err.empty())
    {
        if (QuitIfError)
        {
            fi::Engine::instance().quitFromError("(json) " + err);
        }

        return nlohmann::json();
    }

    nlohmann::json parsedJSON = nlohmann::json::parse(jsonStr);
    return parsedJSON;
}

////////////////////////////////////////////////////////////

void fi::jsonSaveToFile(std::string Path, std::string jsonStr)
{
    std::ofstream myfile;
    myfile.open (Path);
    myfile << jsonStr;
    myfile.close();
}

////////////////////////////////////////////////////////////

std::string fi::jsonGetErrorFromParsing(std::string str)
{
    // This is fucked, but to my knowledge nlohmann::json does not give me an easy way to get the line number of failed json.
    // I am using picojson for this purpose.  It is fucked, it is fucked...

    picojson::value v;
    std::string err = picojson::parse(v, str);
    return err;
}

////////////////////////////////////////////////////////////

sf::Color fi::jsonGetColor(nlohmann::json Config, std::string Field)
{
    nlohmann::json ColorSection = Config[Field].get<nlohmann::json>();
    sf::Color Color;

    if (ColorSection.count("r"))
    {
        Color.r = ColorSection["r"].get<int>();
        Color.g = ColorSection["g"].get<int>();
        Color.b = ColorSection["b"].get<int>();

        if (ColorSection.count("a") > 0)
        {
            Color.a = ColorSection["a"].get<int>();
        }
        else
        {
            Color.a = 255;
        }
    }
    else
    {
        auto str = ColorSection.get<std::string>();

        if (str[0] == '#')
        {
            fi::colorConvertToRGB(str, Color);
        }
        else
        {
            std::vector<std::string> rgb = fi::splitString(str, ',');
            Color.r = static_cast<sf::Uint8>(std::stoi(rgb[0].c_str()));
            Color.g = static_cast<sf::Uint8>(std::stoi(rgb[1].c_str()));
            Color.b = static_cast<sf::Uint8>(std::stoi(rgb[2].c_str()));

            if (rgb.size() == 4)
            {
                Color.a = static_cast<sf::Uint8>(std::stoi(rgb[3].c_str()));
            }
        }
    }

    return Color;
}

////////////////////////////////////////////////////////////

void fi::stringDeleteCharAt(std::string &Source, unsigned int Index)
{
    if (Index < 0)
        return;
    if (Index > Source.size())
        return;

    std::stringstream ss;
    for (unsigned int i = 0; i < Source.size(); i++)
    {
        if (i != Index)
            ss << Source[i];
    }

    Source = ss.str();
}

////////////////////////////////////////////////////////////

std::vector<std::string>& fi::splitString(const std::string s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

////////////////////////////////////////////////////////////

std::vector<std::string> fi::splitString(const std::string s, char delim)
{
    std::vector<std::string> elems;
    return splitString(s, delim, elems);
}
////////////////////////////////////////////////////////////

std::vector<std::string> fi::jsonGetVectorOfStrings(nlohmann::json Config, std::string Field)
{
    std::vector<std::string> result;
    auto array = Config[Field].get<nlohmann::json>();
    if (!array.empty())
    {
        for (auto &it : array)
        {
            result.push_back(it.get<std::string>());
        }
    }

    return result;
}

sf::Vector2i fi::convert(sf::Vector2f value)
{
    return sf::Vector2i((int)value.x, (int)value.y);
}

////////////////////////////////////////////////////////////

sf::Vector2f fi::convert(sf::Vector2i value)
{
    return sf::Vector2f((float)value.x, (float)value.y);
}

////////////////////////////////////////////////////////////

bool fi::vectorContains(std::vector<int> *Vector, int Value)
{
    return std::find(Vector->begin(), Vector->end(), Value) != Vector->end();
}

////////////////////////////////////////////////////////////

bool fi::vectorContains(std::deque<int> *Vector, int Value)
{
    return std::find(Vector->begin(), Vector->end(), Value) != Vector->end();
}

////////////////////////////////////////////////////////////

void fi::vectorShuffle(std::vector<int> *Vector, int NumberOfTimes)
{
    if (!Vector->empty())
    {
        for (int i = 0; i < NumberOfTimes; i++)
        {
            int a = fi::getRandom().i(0, Vector->size() - 1);
            int b = fi::getRandom().i(0, Vector->size() - 1);
            int Temp = Vector->at(a);
            (*Vector)[a] = Vector->at(b);
            (*Vector)[b] = Temp;
        }
    }
}

////////////////////////////////////////////////////////////

std::vector<std::string> fi::removeElementsFromVectorOfStringsNotContainingSubstr(std::vector<std::string> strings, std::string substr)
{
    std::vector<std::string> ReturnVector;

    for (int i = 0; i < strings.size(); i++)
    {
        if(strstr(strings[i].c_str(),substr.c_str()))
        {
            ReturnVector.push_back(strings[i]);
        }
    }

    return ReturnVector;
}