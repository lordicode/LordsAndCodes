// CombatChallenge.cpp
#include "CombatChallenge.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// singleton accessor
CombatChallenge& CombatChallenge::instance()
{
    static CombatChallenge db;
    return db;
}

// lookup by challenge ID
const Challenge* CombatChallenge::get(int id) const
{
    auto it = m_table.find(id);
    return it == m_table.end() ? nullptr : &it->second;
}

static std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

// constructor: load CSV once
CombatChallenge::CombatChallenge()
{
    loadCSV("src/Assets/GameChallenge/challenges.csv");
}

// helper: split one CSV line into fields, respecting quotes
static std::vector<std::string> splitQuoted(const std::string& line)
{
    std::vector<std::string> out;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';   // doubled quote inside quoted field
                ++i;
            } else {
                inQuotes = !inQuotes;          // toggle quote state
            }
        }
        else if (c == ',' && !inQuotes) {
            out.push_back(std::move(field));
            field.clear();
        }
        else {
            field += c;
        }
    }
    out.push_back(std::move(field));           // last field
    return out;
}

static void trim(std::string& s)
{
    s.erase(0, s.find_first_not_of(" \t"));
    s.erase(s.find_last_not_of(" \t") + 1);
}

// strip outer quotes and un-escape
static std::string unquote(std::string s)
{
    trim(s);
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        s = s.substr(1, s.size() - 2);
    size_t pos = 0;
    while ((pos = s.find("\"\"", pos)) != std::string::npos)
        s.replace(pos, 2, "\"");
    return s;
}

// parse challenges.csv
void CombatChallenge::loadCSV(const std::string& path)
{
    std::ifstream in(path);
    if (!in) return;

    std::string line;
    bool firstLine = true;  // skip header row

    while (std::getline(in, line))
    {
        if (line.empty()) continue;
        auto cols = splitQuoted(line);
        if (cols.size() < 3) continue;

        std::string idStr      = unquote(cols[0]);
        std::string prompt     = unquote(cols[1]);
        std::string answersStr = unquote(cols[2]);

        // skip header
        if (firstLine && !std::isdigit(idStr.front())) {
            firstLine = false;
            continue;
        }
        firstLine = false;

        int id = 0;
        try { id = std::stoi(idStr); }
        catch (...) { continue; }

        Challenge ch;
        ch.id     = id;
        ch.prompt = prompt;

        std::istringstream as(answersStr);
        std::string ans;
        while (std::getline(as, ans, '|')) {
            trim(ans);
            if (!ans.empty())
                ch.answers.push_back(toLower(ans));
        }

        m_table.emplace(ch.id, std::move(ch));
    }
}
