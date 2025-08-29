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
#include <iomanip>

using namespace std;
const std::string EPSILON = "epsilon";

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

// TerminalSet 전용 파싱 함수 (중괄호 제거 후 쉼표로 분리)
vector<Symbol> parseTerminalSet(const string& s) {
    string str = s;
    str.erase(remove(str.begin(), str.end(), '{'), str.end());
    str.erase(remove(str.begin(), str.end(), '}'), str.end());

    vector<Symbol> result;
    stringstream ss(str);
    string item;
    while (getline(ss, item, ',')) {
        size_t start = 0;
        while (start < item.size() && isspace((unsigned char)item[start])) start++;
        size_t end = item.size();
        while (end > start && isspace((unsigned char)item[end - 1])) end--;
        result.push_back(item.substr(start, end - start));
    }
    return result;
}

// --- 새로 추가: { ... } 단위로 상태 집합 문자열 분리 함수 ---
vector<string> splitStateSets(const string& s) {
    vector<string> result;
    int level = 0;
    string current;
    for (char c : s) {
        if (c == '{') {
            if (level == 0) current.clear();
            level++;
            current += c;
        }
        else if (c == '}') {
            current += c;
            level--;
            if (level == 0) {
                result.push_back(current);
                current.clear();
            }
        }
        else {
            if (level > 0) current += c;
        }
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
            string rhs = line.substr(line.find('=') + 1);
            fa.alphabet = parseTerminalSet(rhs);
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
            string rhs = line.substr(line.find('=') + 1);

            // 중괄호 제거
            rhs.erase(remove(rhs.begin(), rhs.end(), '{'), rhs.end());
            rhs.erase(remove(rhs.begin(), rhs.end(), '}'), rhs.end());

            fa.finals.clear();

            rhs = trim(rhs);

            if (!rhs.empty()) {
                stringstream ss(rhs);
                string item;
                while (getline(ss, item, ',')) {
                    string state = trim(item);
                    if (!state.empty()) {
                        fa.finals.insert(state);
                    }
                }
            }
        }
    }
    return fa;
}

void writeFA(const FA& fa, const string& filename) {
    ofstream file(filename);
    file << "StateSet = { ";
    for (const auto& s : fa.states) file << s << ", ";
    file << "}\n";

    file << "TerminalSet = { ";
    for (const auto& a : fa.alphabet) file << a << ", ";
    file << "}\n";

    file << "DeltaFunctions = {\n";
    for (const auto& [key, val] : fa.delta) {
        file << "(" << key.first << ", " << key.second << ") = { ";
        for (const auto& s : val) file << s << ", ";
        file << "}\n";
    }
    file << "}\n";

    file << "StartState = " << fa.start << "\n";
    file << "FinalStateSet = { ";
    for (const auto& f : fa.finals) file << f << ", ";
    file << "}\n";
}

StateSet epsilonClosure(const FA& nfa, const StateSet& states) {
    StateSet closure = states;
    queue<State> q;
    for (auto s : states) q.push(s);

    while (!q.empty()) {
        State current = q.front(); q.pop();
        auto it = nfa.delta.find({ current, EPSILON });
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

FA convertToDFA(const FA& nfa) {
    FA dfa;

    // ε는 DFA 알파벳에서 제거
    dfa.alphabet = nfa.alphabet;
    dfa.alphabet.erase(remove(dfa.alphabet.begin(), dfa.alphabet.end(), "ε"), dfa.alphabet.end());

    // 상태 집합 이름 생성 함수
    auto makeName = [](const StateSet& states) -> string {
        if (states.empty()) return "";  // 공집합은 빈 문자열로 처리
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

    // 시작 상태의 ε-closure부터 시작
    StateSet startClosure = epsilonClosure(nfa, { nfa.start });
    string startName = makeName(startClosure);
    stateNames[startClosure] = startName;
    dfa.start = startName;
    q.push(startClosure);

    while (!q.empty()) {
        StateSet currSet = q.front(); q.pop();
        string currName = stateNames[currSet];
        dfa.states.insert(currName);

        for (const auto& symbol : dfa.alphabet) {
            StateSet moveSet;

            // currSet 각 상태에서 symbol 이동
            for (const auto& s : currSet) {
                auto it = nfa.delta.find({ s, symbol });
                if (it != nfa.delta.end()) {
                    moveSet.insert(it->second.begin(), it->second.end());
                }
            }

            // 이동 집합에 대해 ε-closure 계산
            StateSet closure = epsilonClosure(nfa, moveSet);

            // 공집합 상태면 전이 저장하지 않고 continue
            if (closure.empty()) continue;

            string closureName = makeName(closure);
            if (!stateNames.count(closure)) {
                stateNames[closure] = closureName;
                q.push(closure);
            }

            dfa.delta[{currName, symbol}] = { closureName };
        }
    }

    // 종결 상태 결정 (입실론 클로저를 다시 한번 적용하여 정확하게 판단)
    for (const auto& [nfaStates, dfaStateName] : stateNames) {
        StateSet closureSet = epsilonClosure(nfa, nfaStates);
        bool isFinal = false;
        for (const auto& s : closureSet) {
            if (nfa.finals.count(s)) {
                isFinal = true;
                break;
            }
        }
        if (isFinal) {
            dfa.finals.insert(dfaStateName);
        }
    }

    return dfa;
}

FA minimizeDFA(const FA& dfa) {
    FA minimized;
    minimized.alphabet = dfa.alphabet;

    // 상태를 그룹으로 나누기: 초기엔 final과 non-final로 분할
    set<string> finalStates = dfa.finals;
    set<string> nonFinalStates;
    for (const auto& s : dfa.states) {
        if (!finalStates.count(s)) {
            nonFinalStates.insert(s);
        }
    }

    vector<set<string>> partitions;
    if (!nonFinalStates.empty()) partitions.push_back(nonFinalStates);
    if (!finalStates.empty()) partitions.push_back(finalStates);

    map<string, int> stateToPartition;
    for (int i = 0; i < partitions.size(); ++i) {
        for (const auto& s : partitions[i]) {
            stateToPartition[s] = i;
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;
        vector<set<string>> newPartitions;

        for (const auto& group : partitions) {
            map<string, set<string>> splitter;
            for (const auto& state : group) {
                string signature;
                for (const auto& symbol : dfa.alphabet) {
                    auto it = dfa.delta.find({ state, symbol });
                    if (it != dfa.delta.end()) {
                        const string& target = *it->second.begin();
                        signature += to_string(stateToPartition[target]) + ",";
                    }
                    else {
                        signature += "#,"; // '#' = no transition
                    }
                }
                splitter[signature].insert(state);
            }

            if (splitter.size() == 1) {
                newPartitions.push_back(group);
            }
            else {
                changed = true;
                for (const auto& [_, splitGroup] : splitter) {
                    newPartitions.push_back(splitGroup);
                }
            }
        }

        partitions = newPartitions;
        stateToPartition.clear();
        for (int i = 0; i < partitions.size(); ++i) {
            for (const auto& s : partitions[i]) {
                stateToPartition[s] = i;
            }
        }
    }

    // 새 상태 이름 만들기
    map<int, string> partitionToName;
    for (int i = 0; i < partitions.size(); ++i) {
        vector<string> sortedStates(partitions[i].begin(), partitions[i].end());
        sort(sortedStates.begin(), sortedStates.end());
        stringstream ss;
        ss << "{";
        for (size_t j = 0; j < sortedStates.size(); ++j) {
            ss << sortedStates[j];
            if (j != sortedStates.size() - 1) ss << ",";
        }
        ss << "}";
        partitionToName[i] = ss.str();
        minimized.states.insert(partitionToName[i]);
    }

    // 시작 상태
    minimized.start = partitionToName[stateToPartition[dfa.start]];

    // 종결 상태
    for (const auto& f : dfa.finals) {
        minimized.finals.insert(partitionToName[stateToPartition[f]]);
    }

    // 전이 함수
    for (int i = 0; i < partitions.size(); ++i) {
        string rep = *partitions[i].begin();
        string fromState = partitionToName[i];
        for (const auto& symbol : dfa.alphabet) {
            auto it = dfa.delta.find({ rep, symbol });
            if (it != dfa.delta.end()) {
                const string& toState = *it->second.begin();
                int toPartition = stateToPartition[toState];
                minimized.delta[{fromState, symbol}] = { partitionToName[toPartition] };
            }
        }
    }

    return minimized;
}



int main() {
    // 1. NFA 읽기
    FA nfa = readFA("nfa.txt");

    // 2. delta 출력
    for (const auto& p : nfa.delta) {
        const auto& key = p.first;
        const auto& val = p.second;
        cout << "Delta key: (" << key.first << ", " << key.second << ") -> { ";
        for (const auto& s : val) cout << s << " ";
        cout << "}" << endl;
    }

    // 3. NFA 최종 상태 출력 (디버그)
    cout << "NFA Final States (nfa.finals): { ";
    for (const auto& f : nfa.finals) {
        cout << "\"" << f << "\" ";
    }
    cout << "}" << endl;

    // 4. epsilon-closure 디버깅: {q000, q001} 클로저 계산 및 출력
    StateSet testStates = { "q000", "q001" };
    StateSet closure = epsilonClosure(nfa, testStates);

    cout << "Epsilon-closure({q000,q001}): { ";
    for (const auto& s : closure) {
        cout << s << " ";
    }
    cout << "}" << endl;

    // 5. NFA -> DFA 변환 후 출력
    FA dfa = convertToDFA(nfa);
    writeFA(dfa, "dfa_output.txt");

    FA minimized = minimizeDFA(dfa);
    writeFA(minimized, "reduced_dfa_output.txt");


    cout << "변환 완료: 결과가 출력 파일에 저장되었습니다." << endl;
    return 0;
}


