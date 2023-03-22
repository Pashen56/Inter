#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <stack>

using namespace std;

// ����������� ���� �������� ��� ����������
using Instruction = function<void(vector<string>)>;

map<string, string> variable_map;

// ������� ��� ������� ������ �� ��������� ����� (������), (����������� ����������)
vector<string> split1(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

//������� ��� ������ ����� ���������� �� �� ��������, ���� ����� ���������� ����������
int variableCheck(string token) {
    if (variable_map.count(token)) {
        token = variable_map[token];
    }
    return stoi(token);
}

int precedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0;
}

int applyOp(int a, int b, char op) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return a / b;
    }
    // Return 0 for any unexpected operator character
    return 0;
}

int evaluate(std::string tokens) {
    int i;
    std::stack<int> values;
    std::stack<char> ops;

    for (i = 0; i < tokens.length(); i++) {
        if (tokens[i] == ' ')
            continue;

        else if (tokens[i] == '(') {
            ops.push(tokens[i]);
        }

        else if (isdigit(tokens[i])) {
            int val = 0;

            while (i < tokens.length() && isdigit(tokens[i])) {
                val = (val * 10) + (tokens[i] - '0');
                i++;
            }

            values.push(val);
            i--;
        }

        else if (isalpha(tokens[i])) {
            string var;

            while (i < tokens.length() && isalnum(tokens[i])) {
                var += tokens[i];
                i++;
            }

            values.push(variableCheck(var));
            i--;
        }

        else if (tokens[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                int val2 = values.top();
                values.pop();

                int val1 = values.top();
                values.pop();

                char op = ops.top();
                ops.pop();

                values.push(applyOp(val1, val2, op));
            }

            if (!ops.empty())
                ops.pop();
        }

        else {
            while (!ops.empty() && precedence(ops.top()) >= precedence(tokens[i])) {
                int val2 = values.top();
                values.pop();

                int val1 = values.top();
                values.pop();

                char op = ops.top();
                ops.pop();

                values.push(applyOp(val1, val2, op));
            }

            ops.push(tokens[i]);
        }
    }

    while (!ops.empty()) {
        int val2 = values.top();
        values.pop();

        int val1 = values.top();
        values.pop();

        char op = ops.top();
        ops.pop();

        values.push(applyOp(val1, val2, op));
    }

    return values.top();
}

int main() {
    // �������� �������������� �������, ��� ���� ��� ��� ����������, � �������� ��� �������
    map<string, Instruction> instruction_map;

    instruction_map["halt"] = [](vector<string> tokens) {
        if (tokens.size() != 0) {
            cerr << "������: �������� ���������� ���������� ��� ���������� halt" << endl;
            return;
        }
        cout << "��������� ���������� halt" << endl;
        exit(0);
    };

    // ���������� ���������� � ������������� ������
    instruction_map["in"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "ERROR: �������� ���������� ���������� ��� ���������� in" << endl;
            return;
        }
        variable_map[tokens[0]] = tokens[1];
    };

    instruction_map["out"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "ERROR: �������� ���������� ���������� ��� ���������� out" << endl;
            return;
        }
        cout << tokens[0] << " = " << variableCheck(tokens[0]) << endl;
    };

    instruction_map["add"] = [](vector<string> tokens) {
        if (tokens.size() != 2 && tokens.size() != 3) {
            cerr << "������: �������� ���������� ���������� ��� ���������� add" << endl;
            return;
        }
        if (tokens.size() == 2) {
            cout << tokens[0] << " + " << tokens[1] << " = " << variableCheck(tokens[0]) + variableCheck(tokens[1]) << endl;
        }
        else {
            variable_map[tokens[0]] = to_string(variableCheck(tokens[1]) + variableCheck(tokens[2]));
            cout << tokens[0] << " = " << tokens[1] << " + " << tokens[2] << " = " << variableCheck(tokens[1]) << " + " << variableCheck(tokens[2]) << " = " << variableCheck(tokens[1]) + variableCheck(tokens[2]) << endl;
        }
    };

    instruction_map["sub"] = [](vector<string> tokens) {
        if (tokens.size() != 2 && tokens.size() != 3) {
            cerr << "������: �������� ���������� ���������� ��� ���������� sub" << endl;
            return;
        }
        if (tokens.size() == 2) {
            cout << tokens[0] << " - " << tokens[1] << " = " << variableCheck(tokens[0]) - variableCheck(tokens[1]) << endl;
        }
        else {
            variable_map[tokens[0]] = to_string(variableCheck(tokens[1]) - variableCheck(tokens[2]));
            cout << tokens[0] << " = " << tokens[1] << " - " << tokens[2] << " = " << variableCheck(tokens[1]) << " - " << variableCheck(tokens[2]) << " = " << variableCheck(tokens[1]) - variableCheck(tokens[2]) << endl;
        }
    };

    instruction_map["mul"] = [](vector<string> tokens) {
        if (tokens.size() != 2 && tokens.size() != 3) {
            cerr << "������: �������� ���������� ���������� ��� ���������� mul" << endl;
            return;
        }
        if (tokens.size() == 2) {
            cout << tokens[0] << " * " << tokens[1] << " = " << variableCheck(tokens[0]) * variableCheck(tokens[1]) << endl;
        }
        else {
            variable_map[tokens[0]] = to_string(variableCheck(tokens[1]) * variableCheck(tokens[2]));
            cout << tokens[0] << " = " << tokens[1] << " * " << tokens[2] << " = " << variableCheck(tokens[1]) << " * " << variableCheck(tokens[2]) << " = " << variableCheck(tokens[1]) * variableCheck(tokens[2]) << endl;
        }
    };

    instruction_map["div"] = [](vector<string> tokens) {
        if (tokens.size() != 2 && tokens.size() != 3) {
            cerr << "������: �������� ���������� ���������� ��� ���������� div" << endl;
            return;
        }
        if (tokens.size() == 2) {
            if (variableCheck(tokens[1]) == 0) {
                cerr << "������: ������� �� ����" << endl;
                return;
            }
            cout << tokens[0] << " / " << tokens[1] << " = " << variableCheck(tokens[0]) / variableCheck(tokens[1]) << endl;
        }
        else {
            if (variableCheck(tokens[2]) == 0) {
                cerr << "������: ������� �� ����" << endl;
                return;
            }
            variable_map[tokens[0]] = to_string(variableCheck(tokens[1]) / variableCheck(tokens[2]));
            cout << tokens[0] << " = " << tokens[1] << " / " << tokens[2] << " = " << variableCheck(tokens[1]) << " / " << variableCheck(tokens[2]) << " = " << variableCheck(tokens[1]) / variableCheck(tokens[2]) << endl;
        }
    };

    instruction_map["and"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "������: �������� ���������� ���������� ��� ���������� and" << endl;
            return;
        }
        int result = variableCheck(tokens[0]) & variableCheck(tokens[1]);
        cout << tokens[0] << " & " << tokens[1] << " = " << result << endl;
    };

    instruction_map["or"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "������: �������� ���������� ���������� ��� ���������� or" << endl;
            return;
        }
        int result = variableCheck(tokens[0]) | variableCheck(tokens[1]);
        cout << tokens[0] << " | " << tokens[1] << " = " << result << endl;
    };

    instruction_map["xor"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "������: �������� ���������� ���������� ��� ���������� xor" << endl;
            return;
        }
        int result = variableCheck(tokens[0]) ^ variableCheck(tokens[1]);
        cout << tokens[0] << " ^ " << tokens[1] << " = " << result << endl;
    };

    instruction_map["jmp"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "������: �������� ���������� ���������� ��� ���������� jmp" << endl;
            return;
        }
        cout << "��������� ���������� jmp � ���������� #" << variableCheck(tokens[0]) << endl;
        // ������� � ��������� ����������
        // ��������:
        // instruction_pointer = target;
    };

    instruction_map["not"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "������: �������� ���������� ���������� ��� ���������� not" << endl;
            return;
        }
        int result = ~variableCheck(tokens[0]);
        cout << "NOT " << tokens[0] << " = " << result << endl;
    };

    instruction_map["mov"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "ERROR: �������� ���������� ���������� ��� ���������� mov" << endl;
            return;
        }
        variable_map[tokens[0]] = to_string(variableCheck(tokens[1]));
    };

    instruction_map["inc"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "������: �������� ���������� ���������� ��� ���������� inc" << endl;
            return;
        }
        int result = variableCheck(tokens[0]);
        cout << "Increment " << tokens[0] << " = " << ++result << endl;
    };

    instruction_map["dec"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "������: �������� ���������� ���������� ��� ���������� dec" << endl;
            return;
        }
        int result = variableCheck(tokens[0]);
        cout << "Decrement " << tokens[0] << " = " << --result << endl;
    };

    ifstream input1("input.txt");
    ofstream output1("output.txt");
    bool in_comment = false; // ���� ��� ������������ ������ � ����� �������������� �����������
    string line;
    while (getline(input1, line)) { // ������ ���� ���������
        string result_line;
        for (int i = 0; i < line.length(); i++) { // �������� �� ������� ������� � ������
            if (!in_comment && line[i] == '/' && i < line.length() - 1 && line[i + 1] == '/') {
                // ������ ������������� ����������� - ���������� ������� ������
                break;
            }
            else if (!in_comment && line[i] == '/' && i < line.length() - 1 && line[i + 1] == '*') {
                // ������ �������������� ����������� - ���������� ������� ������
                in_comment = true;
                i++;
            }
            else if (in_comment && line[i] == '*' && i < line.length() - 1 && line[i + 1] == '/') {
                // ����� �������������� �����������
                in_comment = false;
                i++;
            }
            else if (!in_comment) {
                // ������ �� �������� ������ ����������� - ��������� ��� � ����������
                result_line += line[i];
            }
        }
        if ((result_line != "") && (result_line != " ")) {
            output1 << result_line << endl; // ���������� ������ ��� ������������ � �������� ����
        }
    }
    input1.close();
    output1.close();

    // ���������� � ������������� ���������� �� ������������ �����
    ifstream input2("output.txt");
    string str;
    int i = 0;
    while (getline(input2, str)) {
        if (str.find('=') == string::npos) {
            for (; str[i] != ' '; i++) {}
            str[i] = ',';
            str[str.find(';')] = ' ';
            i = 0;
            // �������� ������ ��������
            str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
            // ��������� ������ �� ��������� �����
            vector<string> tokens = split1(str, ',');

            // �������� ������� ���������� � ������������� �������
            if (instruction_map.find(tokens[0]) == instruction_map.end()) {
                cerr << "������: ����������� ���������� " << endl;
                continue;
            }

            // ���������� ����������
            instruction_map[tokens[0]](vector<string>(tokens.begin() + 1, tokens.end()));
        }
        else {
            for (; str[i] != ' '; i++) {}
            str[i] = ',';
            for (; str[i] != ' '; i++) {}
            str[i] = ',';
            str[str.find(';')] = ' ';
            i = 0;
            // �������� ������ ��������
            str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
            vector<string> tokens = split1(str, ',');
            variable_map[tokens[0]] = evaluate(tokens[2]);
            std::cout << tokens[0] << " = " << evaluate(tokens[2]);
        }

    }
    input2.close();
    return 0;
}