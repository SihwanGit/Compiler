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
const std::string EPSILON = "epsilon";  //입실론은 epsilon으로 정의한다.
//입실론 신호 ε가 계속 인코딩 에러가 떠서 부득이하게 변경했다.

using State = string;           //상태 Q
using Symbol = string;          //terminal Set
using StateSet = set<State>;    //상태들의 집합, State의 집합을 또다른 하나의 상태로 인식할 수 있게끔 추가함.


//FA
struct FA {
    StateSet states;                            //Q
    vector<Symbol> alphabet;                    //시그마
    map<pair<State, Symbol>, StateSet> delta;   //델타 함수
    State start;                                //시작 상태
    StateSet finals;                            //종결 상태
};


//문자열의 앞뒤 공백 문자를 제거하여 반환하는 함수
string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start])) start++;
    size_t end = s.size();
    while (end > start && isspace((unsigned char)s[end - 1])) end--;
    return s.substr(start, end - start);
}


//주어진 문자열을 구분자(컴마',') 기준으로 잘라 StateSet에 담는다. 상태 집합 파싱에 사용됨.
StateSet split(const string& s, char delimiter = ',') {
    StateSet result;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delimiter)) {
        result.insert(trim(item));
    }
    return result;
}


//TerminalSet 전용 파싱 함수 (중괄호 제거 후 쉼표로 분리)
//예를들어 TerminalSet = { a, b, c }에서 {}를 제거하고, 각 기호를 vector<Symbol>로 파싱함.
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


//중첩된{ q001,q002 } 같은 상태 집합들을 통째로 분리해주는 파서.
//위의 split 함수는 ','를 기준으로 판단하지만 중괄호가 중첩된 State Set을 또 다른 state로 인식할 수 없기 때문에 만든 함수.
//split와 splitStateSets로 상태 집합 도 또다른 상태로 인식한다.
//예를들어 {q000,q001}은 q000과 q0001의 집합이지만 {q000,q001} 자체로도 또다른 State다.
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


//.txt 형식의 입력 파일을 읽어 FA 구조체에 구성요소들을 담아 반환하는 함수.
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


//주어진 FA 객체를 텍스트 파일로 출력하는 함수
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


//주어진 상태 집합에 대해 ε - closure(입실론 경로로 도달 가능한 상태들)를 계산하여 반환하는 함수.
//BFS로 ε 경로를 따라가며 상태를 확장함.
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


//NFA를 DFA로 변환하는 함수
//변환하는 과정에서 자연스럽게 inaccessible한 상태들도 정리됨.
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

            // 공집합 상태면 전이 저장하지 않고 continue. 공집합은 델타함수에서 배재.
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


//Hopcroft 알고리즘을 사용하여 DFA를 최소화하는 함수
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

    // 새 상태 이름 만들기. 새 상태는 { }를 사용해서 한 번 더 묶는 식으로 이름을 만든다.
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

    // 2. 입실론-nfa 출력.
    writeFA(nfa, "epepsilon_nfa.txt"); //사실 nfa.txt 자체가 입실론 클로저도 반영하다보니, nfa를 그대로 출력하면 됨.

    // 3. NFA -> DFA 변환 후 출력
    FA dfa = convertToDFA(nfa);
    writeFA(dfa, "dfa_output.txt");

    // 4. DFA -> reduced DFA 변환 후 출력
    FA minimized = minimizeDFA(dfa);
    writeFA(minimized, "reduced_dfa_output.txt");

    cout << "변환 완료: 결과가 출력 파일에 저장되었습니다." << endl;
    return 0;
}

