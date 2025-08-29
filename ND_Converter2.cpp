#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <cctype>
#include <iomanip> // 추가

using namespace std;

using State = string;
using Symbol = string;
using StateSet = set<State>;

struct FA {
    StateSet states;
    vector<Symbol> alphabet;
    map<pair<State, Symbol>, StateSet> delta;
    State start;
    StateSet finals;
};

string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start])) start++;
    size_t end = s.size();
    while (end > start && isspace((unsigned char)s[end - 1])) end--;
    return s.substr(start, end - start);
}

StateSet split(const string& s, char delimiter = ',') {
    StateSet result;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delimiter)) {
        result.insert(trim(item));
    }
    return result;
}

FA readFA(const string& filename) {
    ifstream file(filename);
    FA fa;
    string line;
    bool inDelta = false;

    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (line.find("StateSet") == 0) {
            fa.states = split(line.substr(line.find('=') + 1));
        }
        else if (line.find("TerminalSet") == 0) {
            StateSet terminals = split(line.substr(line.find('=') + 1));
            fa.alphabet = vector<Symbol>(terminals.begin(), terminals.end());
        }
        else if (line.find("DeltaFunctions") == 0) {
            inDelta = true;
        }
        else if (line == "}") {
            inDelta = false;
        }
        else if (inDelta) {
            size_t eqPos = line.find('=');
            if (eqPos == string::npos) continue;

            string key = trim(line.substr(0, eqPos));
            string value = trim(line.substr(eqPos + 1));

            if (key.front() == '(' && key.back() == ')') {
                key = key.substr(1, key.size() - 2);
                size_t commaPos = key.find(',');
                if (commaPos == string::npos) continue;

                string from = trim(key.substr(0, commaPos));
                string symbol = trim(key.substr(commaPos + 1));

                if (value.front() == '{' && value.back() == '}') {
                    value = value.substr(1, value.size() - 2);
                    StateSet toSet = split(value);
                    fa.delta[{from, symbol}] = toSet;
                }
            }
        }
        else if (line.find("StartState") == 0) {
            fa.start = trim(line.substr(line.find('=') + 1));
        }
        else if (line.find("FinalStateSet") == 0) {
            fa.finals = split(line.substr(line.find('=') + 1));
        }
    }
    return fa;
}



StateSet epsilonClosure(const FA& nfa, const StateSet& states) {
    StateSet closure = states;
    queue<State> q;
    for (auto s : states) q.push(s);

    while (!q.empty()) {
        State current = q.front(); q.pop();
        auto it = nfa.delta.find({ current, "ε" });
        if (it != nfa.delta.end()) {
            for (const auto& next : it->second) {
                if (!closure.count(next)) {
                    closure.insert(next);
                    q.push(next);
                }
            }
        }
    }
    return closure;
}


FA minimizeDFA(const FA& dfa) {
    return dfa; // DFA 최소화 생략
}

FA convertToDFA(const FA& nfa) {
    FA dfa;

    dfa.alphabet = nfa.alphabet;
    dfa.alphabet.erase(remove(dfa.alphabet.begin(), dfa.alphabet.end(), "ε"), dfa.alphabet.end());

    auto makeName = [](const StateSet& states) -> string {
        vector<string> sorted(states.begin(), states.end());
        sort(sorted.begin(), sorted.end());
        stringstream ss;
        ss << "{";
        for (size_t i = 0; i < sorted.size(); ++i) {
            ss << sorted[i];
            if (i != sorted.size() - 1) ss << ",";
        }
        ss << "}";
        return ss.str();
    };

    map<StateSet, string> stateNames;
    queue<StateSet> q;

    StateSet startClosure = epsilonClosure(nfa, { nfa.start });
    string startName = makeName(startClosure);
    stateNames[startClosure] = startName;
    dfa.start = startName;
    dfa.states.insert(startName);  // **시작 상태도 반드시 추가**
    q.push(startClosure);

    while (!q.empty()) {
        StateSet currSet = q.front(); q.pop();
        string currName = stateNames[currSet];

        // 현재 상태 이름는 이미 추가되어 있으니 중복 추가 방지 가능

        for (const auto& symbol : dfa.alphabet) {
            StateSet moveSet;
            for (const auto& s : currSet) {
                auto it = nfa.delta.find({ s, symbol });
                if (it != nfa.delta.end()) {
                    moveSet.insert(it->second.begin(), it->second.end());
                }
            }

            StateSet closure = epsilonClosure(nfa, moveSet);
            if (closure.empty()) continue;

            string closureName = makeName(closure);
            if (!stateNames.count(closure)) {
                stateNames[closure] = closureName;
                dfa.states.insert(closureName);  // **여기서도 새 상태 추가**
                q.push(closure);
            }

            dfa.delta[{currName, symbol}] = { closureName };
        }
    }

    // 종결 상태 결정
    for (const auto& [nfaStates, name] : stateNames) {
        for (const auto& f : nfaStates) {
            if (nfa.finals.count(f)) {
                dfa.finals.insert(name);
                break;
            }
        }
    }

    return dfa;
}

// writeFA에서 쉼표 제거 개선
void writeFA(const FA& fa, const string& filename) {
    ofstream file(filename);

    file << "StateSet = { ";
    for (auto it = fa.states.begin(); it != fa.states.end(); ++it) {
        if (it != fa.states.begin()) file << ", ";
        file << *it;
    }
    file << " }\n";

    file << "TerminalSet = { ";
    for (size_t i = 0; i < fa.alphabet.size(); ++i) {
        if (i != 0) file << ", ";
        file << fa.alphabet[i];
    }
    file << " }\n";

    file << "DeltaFunctions = {\n";
    for (const auto& [key, val] : fa.delta) {
        file << "(" << key.first << ", " << key.second << ") = { ";
        for (auto it = val.begin(); it != val.end(); ++it) {
            if (it != val.begin()) file << ", ";
            file << *it;
        }
        file << " }\n";
    }
    file << "}\n";

    file << "StartState = " << fa.start << "\n";

    file << "FinalStateSet = { ";
    for (auto it = fa.finals.begin(); it != fa.finals.end(); ++it) {
        if (it != fa.finals.begin()) file << ", ";
        file << *it;
    }
    file << " }\n";
}


int main() {
    FA nfa = readFA("nfa.txt");
    writeFA(nfa, "epsilon_nfa.txt");

    FA dfa = convertToDFA(nfa);
    writeFA(dfa, "dfa_output.txt");

    FA minimized = minimizeDFA(dfa);
    writeFA(minimized, "reduced_dfa_output.txt");

    cout << "변환 완료: 결과가 출력 파일에 저장되었습니다." << endl;
    return 0;
}
