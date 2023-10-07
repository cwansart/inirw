#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <map>

#define GLOBAL_SECTION "__global"

using namespace std;

typedef map<string, string> Section;
typedef map<string, Section> Sections;

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

class IniReader
{
  private:
    string currentSectionName;
    Sections sections;
    bool is_parsed_successful;

    bool parseLine(string line)
    {
      // key-value pairs outside a section will be saved in __global
      string trimmed = trim(line);

      // ignore empty lines or comments
      if (trimmed.empty() || trimmed[0] == ';' || trimmed[0] == '#')
      {
        return true;
      }

      if (trimmed.length() > 2 && trimmed.front() == '[' && trimmed.back() == ']')
      {
        currentSectionName = trimmed.substr(1, trimmed.length()-2);
        sections.insert(pair<string, Section>(currentSectionName, Section()));
        return true;
      }

      size_t pos = trimmed.find("=");
      if (pos != string::npos)
      {
        string key = trimmed.substr(0, pos);
        string value = trimmed.substr(pos+1);
        key = rtrim(key);
        value = ltrim(value);
        sections.find(currentSectionName)->second.insert(pair<string, string>(key, value));
        return true;
      }

      cerr << "this should never happen, unless I forgot a case" << endl;
      return false;
    }

    bool parse(string fileName)
    {
      ifstream in(fileName);

      if (in.is_open())
      {
        string line;
        while (getline(in, line))
        {
          if (!parseLine(line))
          {
            return false;
          }
        }
        in.close();
        return true;
      }
      else
      {
        cerr << "failed to open file '" << fileName << "'" << endl;
        return false;
      }
    }

  public:
    IniReader(string fileName) : currentSectionName(GLOBAL_SECTION)
    {
      sections.insert(pair<string, Section>(GLOBAL_SECTION, Section()));
      is_parsed_successful = parse(fileName);
    }

    bool isParsedSuccessful()
    {
      return is_parsed_successful;
    }

    Sections& getSections()
    {
      return sections;
    }
};

bool writeIni(string fileName, Sections sections)
{
  ofstream out(fileName);
  if (out.is_open())
  {
    for (map<string, Section>::iterator it = sections.begin(); it != sections.end(); ++it)
    {
      if (it->first != GLOBAL_SECTION)
      {
        out << "[" << it->first << "]" << endl;
      }

      for (map<string, string>::iterator itt = it->second.begin(); itt != it->second.end(); ++itt)
      {
        out << itt->first << " = " << itt->second << endl;
      }
    }

    out.close();
    return true;
  }
  else
  {
    cerr << "failed to open file for writing '" << fileName << "'" << endl;
    return false;
  }
}

int main (int argc, char *argv[])
{
  IniReader ini("test.ini");
  if (!ini.isParsedSuccessful())
  {
    cerr << "failed to parse ini file" << endl;
    return 1;
  }

  if (!writeIni("test2.ini", ini.getSections()))
  {
    cerr << "failed to write ini file" << endl;
    return 1;
  }
}

