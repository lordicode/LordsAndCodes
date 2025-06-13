#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct Challenge {
    int id{};
    std::string prompt;
    std::vector<std::string> answers;   // accepted answers (lower-case)
};

class CombatChallenge {
public:
    static CombatChallenge& instance();                     // singleton

    // returns nullptr if id not found
    const Challenge* get(int id) const;

private:
    CombatChallenge();                                      // loads CSV once
    void loadCSV(const std::string& path);

    std::unordered_map<int, Challenge> m_table;
};
