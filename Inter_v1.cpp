#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <stack>

using namespace std;

bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

//определение приоритета
int precedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0;
}

struct TreeNode {
    char val;
    TreeNode* left, * right;
    TreeNode(char x) : val(x), left(NULL), right(NULL) {}
};

TreeNode* buildTree(string expression) {
    stack<TreeNode*> operands;
    stack<char> operators;

    for (int i = 0; i < expression.length(); i++) {
        if (expression[i] == '(') {
            operators.push(expression[i]);
        }
        else if (isdigit(expression[i])) {
            operands.push(new TreeNode(expression[i]));
        }
        else if (isOperator(expression[i])) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(expression[i])) {
                TreeNode* op = new TreeNode(operators.top());
                operators.pop();

                op->right = operands.top();
                operands.pop();
                op->left = operands.top();
                operands.pop();

                operands.push(op);
            }
            operators.push(expression[i]);
        }
        else if (expression[i] == ')') {
            while (!operators.empty() && operators.top() != '(') {
                TreeNode* op = new TreeNode(operators.top());
                operators.pop();

                op->right = operands.top();
                operands.pop();
                op->left = operands.top();
                operands.pop();

                operands.push(op);
            }
            operators.pop();
        }
    }

    while (!operators.empty()) {
        TreeNode* op = new TreeNode(operators.top());
        operators.pop();

        op->right = operands.top();
        operands.pop();
        op->left = operands.top();
        operands.pop();

        operands.push(op);
    }

    return operands.top();
}

int evaluate(TreeNode* root) {
    if (!root) return 0;
    if (!root->left && !root->right) return root->val - '0';
    int left = evaluate(root->left);
    int right = evaluate(root->right);
    if (root->val == '+') return left + right;
    if (root->val == '-') return left - right;
    if (root->val == '*') return left * right;
    return left / right;
}

// Определение типа функтора для инструкций
using Instruction = function<void(vector<string>)>;

map<string, string> variable_map;

// Функция для разбора строки на отдельные слова (токены), (лексический анализатор)
vector<string> split1(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

//функция для замены имени переменной на ее значение, если такая переменная существует
int variableCheck(string token) {
    if (variable_map.count(token)) {
        token = variable_map[token];
    }
    return stoi(token);
}

//вычисления операции
int applyOp(int a, int b, char op) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return a / b;
    }
    return 0;
}

//вычисление арифметического выражения
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
    // Создание ассоциативного массива, где ключ это имя инструкции, а значение это функтор
    map<string, Instruction> instruction_map;

    instruction_map["halt"] = [](vector<string> tokens) {
        if (tokens.size() != 0) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции halt" << endl;
            return;
        }
        cout << "Выполнена инструкция halt" << endl;
        exit(0);
    };

    // Добавление инструкций в ассоциативный массив
    instruction_map["in"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "ERROR: Неверное количество аргументов для инструкции in" << endl;
            return;
        }
        variable_map[tokens[0]] = tokens[1];
    };

    instruction_map["out"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "ERROR: Неверное количество аргументов для инструкции out" << endl;
            return;
        }
        cout << tokens[0] << " = " << variableCheck(tokens[0]) << endl;
    };

    instruction_map["add"] = [](vector<string> tokens) {
        if (tokens.size() != 2 && tokens.size() != 3) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции add" << endl;
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
            cerr << "Ошибка: Неверное количество аргументов для инструкции sub" << endl;
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
            cerr << "Ошибка: Неверное количество аргументов для инструкции mul" << endl;
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
            cerr << "Ошибка: Неверное количество аргументов для инструкции div" << endl;
            return;
        }
        if (tokens.size() == 2) {
            if (variableCheck(tokens[1]) == 0) {
                cerr << "Ошибка: Деление на ноль" << endl;
                return;
            }
            cout << tokens[0] << " / " << tokens[1] << " = " << variableCheck(tokens[0]) / variableCheck(tokens[1]) << endl;
        }
        else {
            if (variableCheck(tokens[2]) == 0) {
                cerr << "Ошибка: Деление на ноль" << endl;
                return;
            }
            variable_map[tokens[0]] = to_string(variableCheck(tokens[1]) / variableCheck(tokens[2]));
            cout << tokens[0] << " = " << tokens[1] << " / " << tokens[2] << " = " << variableCheck(tokens[1]) << " / " << variableCheck(tokens[2]) << " = " << variableCheck(tokens[1]) / variableCheck(tokens[2]) << endl;
        }
    };

    instruction_map["and"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции and" << endl;
            return;
        }
        int result = variableCheck(tokens[0]) & variableCheck(tokens[1]);
        cout << tokens[0] << " & " << tokens[1] << " = " << result << endl;
    };

    instruction_map["or"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции or" << endl;
            return;
        }
        int result = variableCheck(tokens[0]) | variableCheck(tokens[1]);
        cout << tokens[0] << " | " << tokens[1] << " = " << result << endl;
    };

    instruction_map["xor"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции xor" << endl;
            return;
        }
        int result = variableCheck(tokens[0]) ^ variableCheck(tokens[1]);
        cout << tokens[0] << " ^ " << tokens[1] << " = " << result << endl;
    };

    instruction_map["jmp"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции jmp" << endl;
            return;
        }
        cout << "Выполнена инструкция jmp к инструкции #" << variableCheck(tokens[0]) << endl;
        // Перейти к указанной инструкции
        // Например:
        // instruction_pointer = target;
    };

    instruction_map["not"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции not" << endl;
            return;
        }
        int result = ~variableCheck(tokens[0]);
        cout << "NOT " << tokens[0] << " = " << result << endl;
    };

    instruction_map["mov"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "ERROR: Неверное количество аргументов для инструкции mov" << endl;
            return;
        }
        variable_map[tokens[0]] = to_string(variableCheck(tokens[1]));
    };

    instruction_map["inc"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции inc" << endl;
            return;
        }
        int result = variableCheck(tokens[0]);
        cout << "Increment " << tokens[0] << " = " << ++result << endl;
    };

    instruction_map["dec"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции dec" << endl;
            return;
        }
        int result = variableCheck(tokens[0]);
        cout << "Decrement " << tokens[0] << " = " << --result << endl;
    };

    ifstream input1("input.txt");
    ofstream output1("output.txt");
    bool in_comment = false; // флаг для отслеживания начала и конца многострочного комментария
    string line;
    while (getline(input1, line)) { // читаем файл построчно
        string result_line;
        for (int i = 0; i < line.length(); i++) { // проходим по каждому символу в строке
            if (!in_comment && line[i] == '/' && i < line.length() - 1 && line[i + 1] == '/') {
                // начало однострочного комментария - игнорируем остаток строки
                break;
            }
            else if (!in_comment && line[i] == '/' && i < line.length() - 1 && line[i + 1] == '*') {
                // начало многострочного комментария - игнорируем остаток строки
                in_comment = true;
                i++;
            }
            else if (in_comment && line[i] == '*' && i < line.length() - 1 && line[i + 1] == '/') {
                // конец многострочного комментария
                in_comment = false;
                i++;
            }
            else if (!in_comment) {
                // символ не является частью комментария - добавляем его к результату
                result_line += line[i];
            }
        }
        if ((result_line != "") && (result_line != " ")) {
            output1 << result_line << endl; // записываем строку без комментариев в выходной файл
        }
    }
    input1.close();
    output1.close();

    // Считывание и интерпретация инструкций из стандартного ввода
    ifstream input2("output.txt");
    string str, variable;
    int i = 0;
    while (getline(input2, str)) {
        if (str.find('=') == string::npos) {
            for (; str[i] != ' '; i++) {}
            str[i] = ',';
            str[str.find(';')] = ' ';
            i = 0;
            // Удаление лишних пробелов
            str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
            // Разбиение строки на отдельные слова
            vector<string> tokens = split1(str, ',');

            // Проверка наличия инструкции в ассоциативном массиве
            if (instruction_map.find(tokens[0]) == instruction_map.end()) {
                cerr << "Ошибка: Неизвестная инструкция " << endl;
                continue;
            }

            // Выполнение инструкции
            instruction_map[tokens[0]](vector<string>(tokens.begin() + 1, tokens.end()));
        }
        else {
            str[str.find(';')] = ' ';
            str.erase(remove(str.begin(), str.end(), ' '), str.end());
            string var = str.substr(0, str.find('='));
            string expression = str.substr(str.find('=') + 1);
            int result = evaluate(buildTree(expression));
            cout << "buildTree: " << result << endl;
            variable_map[var] = to_string(evaluate(expression));
        }

    }
    input2.close();
    return 0;
}
