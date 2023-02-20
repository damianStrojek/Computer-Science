// Made in collaboration with Chat GPT :D 
// Something for Nokia idk
#include <iostream>
#include <string>
#include <set>

#define CHARTRANSITION 10

bool isInteresting(std::string s) {
    std::set<char> digits;
    for (char c : s) {
        if (isdigit(c)) {
            digits.insert(c);
        }
    }
    return digits.size() <= 2;
}

void printInterestingTimes(std::string &a, std::string &b) {
    int h1, m1, s1, h2, m2, s2;
    sscanf_s(a.c_str(), "%d:%d:%d", &h1, &m1, &s1);
    sscanf_s(b.c_str(), "%d:%d:%d", &h2, &m2, &s2);

    int count = 0;
    for (int h = h1; h <= h2; h++) {
        for (int m = m1; m <= m2; m++) {
            for (int s = s1; s <= s2; s++) {
                std::string actualTime = "";

                if (h < CHARTRANSITION) actualTime += "0";
                actualTime += std::to_string(h) + ":";

                if (m < CHARTRANSITION) actualTime += "0";
                actualTime += std::to_string(m) + ":";

                if (s < CHARTRANSITION) actualTime += "0";
                actualTime += std::to_string(s);

                if (isInteresting(actualTime)) {
                    std::cout << actualTime << "\n";
                    count++;
                }
            }
        }
    }

    if (!count)
        std::cout << "No interesting times found.\n";
    else
        std::cout << "Interesting times found: " << count << "\n";
};

int main() {
    std::string a = "15:15:00", b = "15:15:12";
    printInterestingTimes(a, b);
    return 0;
};