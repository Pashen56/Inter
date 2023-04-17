#include <iostream>
#include <map>
#include <functional>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <stack>
#include <unordered_map>

using namespace std;

struct Function {
    string name;
    int startLine;
    int endLine;
    vector<string> parameters;
    string line_return;
};


map<string, Function> functions;
stack<Function> callStack;

void executeLine(string line) {
    // Реализуйте выполнение команды на текущей строке
}

// Класс для хранения переменных
class VariableMap {
private:
    vector<map<string, string>> scopes;

public:
    // Добавление переменной в карту
    void addVariable(const string& name, const string& value) {
        if (scopes.empty()) {
            scopes.push_back(map<string, string>());
        }
        scopes.back()[name] = value;
        //auto it = scopes.back();
        //cout <<"r"<< it <<endl;
        //cout <<"r"<< it.second;
    }

    // Получение значения переменной по имени
    string& getVariable(string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto jt = it->find(name);
            if (jt != it->end()) {
                return jt->second;
            }
        }
        return name;
    }

    bool checkVariable(string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto jt = it->find(name);
            if (jt != it->end()) {
                return true;
            }
        }
        return false;
    }

    void pushScope() {
        scopes.push_back(map<string, string>());
    }
    void popScope() {
        scopes.pop_back();
    }
};

// Функция для проверки наличия переменной и получения ее значения
string& variableCheck(VariableMap& var_map, string& name) {
    return var_map.getVariable(name);
}

VariableMap variable_map;

// Проверка на принадлежность к операторам
bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

// Определение приоритета
int precedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    return 0;
}

// Структура дерева
struct Node {
    string value;
    Node* left;
    Node* right;
    Node(string x) : value(x), left(NULL), right(NULL) {}
};

// Построение дерева
Node* buildTree(string expression) {
    stack<Node*> operands;
    stack<char> operators;

    for (int i = 0; i < expression.length(); i++) {
        if (expression[i] == '(') {
            operators.push(expression[i]);
        }
        else if (isdigit(expression[i])) {
            int val = 0;
            while (i < expression.length() && isdigit(expression[i])) {
                val = (val * 10) + (expression[i] - '0');
                i++;
            }
            i--;
            operands.push(new Node(to_string(val)));
        }
        else if (isalpha(expression[i])) {
            string var;
            while (i < expression.length() && isalnum(expression[i])) {
                var += expression[i];
                i++;
            }
            i--;
            operands.push(new Node(variableCheck(variable_map, var)));
        }
        else if (isOperator(expression[i])) {
            while (!operators.empty() && precedence(operators.top()) >= precedence(expression[i])) {
                char a = operators.top();
                string str;
                str += a;
                Node* op = new Node(str);
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
                char a = operators.top();
                string str;
                str += a;
                Node* op = new Node(str);
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
        char a = operators.top();
        string str;
        str += a;
        Node* op = new Node(str);
        operators.pop();

        op->right = operands.top();
        operands.pop();
        op->left = operands.top();
        operands.pop();

        operands.push(op);
    }

    return operands.top();
}

// Подсчет значения построенного дерева
int evaluate_tree(Node* root) {
    if (!root) return 0;
    if (!root->left && !root->right) return stoi(root->value);
    int left = evaluate_tree(root->left);
    int right = evaluate_tree(root->right);
    if (root->value == "+") return left + right;
    if (root->value == "-") return left - right;
    if (root->value == "*") return left * right;
    return left / right;
}

// Определение типа функтора для инструкций
using Instruction = function<void(vector<string>)>;

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

// Вычисления операции
int applyOp(int a, int b, char op) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return a / b;
    }
    return 0;
}

// Вычисление арифметического выражения
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

            values.push(stoi(variableCheck(variable_map, var)));
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
        variable_map.addVariable(tokens[0], tokens[1]);
    };

    instruction_map["out"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "ERROR: Неверное количество аргументов для инструкции out" << endl;
            return;
        }
        if (variable_map.checkVariable(tokens[0])) {
            cout << tokens[0] << " = " << variableCheck(variable_map, tokens[0]) << endl;
        }
        else {
            cout << "Переменной " << tokens[0] << " не существует!" << endl;
        }
    };

    instruction_map["add"] = [](vector<string> tokens) {
        if (tokens.size() != 2 && tokens.size() != 3) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции add" << endl;
            return;
        }
        if (tokens.size() == 2) {
            cout << tokens[0] << " + " << tokens[1] << " = " << stoi(variableCheck(variable_map, tokens[0])) + stoi(variableCheck(variable_map, tokens[1])) << endl;
        }
        else {
            variable_map.addVariable(tokens[0], to_string(stoi(variableCheck(variable_map, tokens[1])) + stoi(variableCheck(variable_map, tokens[2]))));
            cout << tokens[0] << " = " << tokens[1] << " + " << tokens[2] << " = " << variableCheck(variable_map, tokens[1]) << " + " << variableCheck(variable_map, tokens[2]) << " = " << stoi(variableCheck(variable_map, tokens[1])) + stoi(variableCheck(variable_map, tokens[2])) << endl;
        }
    };

    instruction_map["sub"] = [](vector<string> tokens) {
        if (tokens.size() != 2 && tokens.size() != 3) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции sub" << endl;
            return;
        }
        if (tokens.size() == 2) {
            cout << tokens[0] << " - " << tokens[1] << " = " << stoi(variableCheck(variable_map, tokens[0])) - stoi(variableCheck(variable_map, tokens[1])) << endl;
        }
        else {
            variable_map.addVariable(tokens[0], to_string(stoi(variableCheck(variable_map, tokens[1])) - stoi(variableCheck(variable_map, tokens[2]))));
            cout << tokens[0] << " = " << tokens[1] << " - " << tokens[2] << " = " << variableCheck(variable_map, tokens[1]) << " - " << variableCheck(variable_map, tokens[2]) << " = " << stoi(variableCheck(variable_map, tokens[1])) - stoi(variableCheck(variable_map, tokens[2])) << endl;
        }
    };

    instruction_map["mul"] = [](vector<string> tokens) {
        if (tokens.size() != 2 && tokens.size() != 3) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции mul" << endl;
            return;
        }
        if (tokens.size() == 2) {
            cout << tokens[0] << " * " << tokens[1] << " = " << stoi(variableCheck(variable_map, tokens[0])) * stoi(variableCheck(variable_map, tokens[1])) << endl;
        }
        else {
            variable_map.addVariable(tokens[0], to_string(stoi(variableCheck(variable_map, tokens[1])) * stoi(variableCheck(variable_map, tokens[2]))));
            cout << tokens[0] << " = " << tokens[1] << " * " << tokens[2] << " = " << variableCheck(variable_map, tokens[1]) << " * " << variableCheck(variable_map, tokens[2]) << " = " << stoi(variableCheck(variable_map, tokens[1])) * stoi(variableCheck(variable_map, tokens[2])) << endl;
        }
    };

    instruction_map["div"] = [](vector<string> tokens) {
        if (tokens.size() != 2 && tokens.size() != 3) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции div" << endl;
            return;
        }
        if (tokens.size() == 2) {
            if (stoi(variableCheck(variable_map, tokens[1])) == 0) {
                cerr << "Ошибка: Деление на ноль" << endl;
                return;
            }
            cout << tokens[0] << " / " << tokens[1] << " = " << stoi(variableCheck(variable_map, tokens[0])) / stoi(variableCheck(variable_map, tokens[1])) << endl;
        }
        else {
            if (stoi(variableCheck(variable_map, tokens[2])) == 0) {
                cerr << "Ошибка: Деление на ноль" << endl;
                return;
            }
            variable_map.addVariable(tokens[0], to_string(stoi(variableCheck(variable_map, tokens[1])) / stoi(variableCheck(variable_map, tokens[2]))));
            cout << tokens[0] << " = " << tokens[1] << " / " << tokens[2] << " = " << variableCheck(variable_map, tokens[1]) << " / " << variableCheck(variable_map, tokens[2]) << " = " << stoi(variableCheck(variable_map, tokens[1])) / stoi(variableCheck(variable_map, tokens[2])) << endl;
        }
    };

    instruction_map["and"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции and" << endl;
            return;
        }
        int result = stoi(variableCheck(variable_map, tokens[0])) & stoi(variableCheck(variable_map, tokens[1]));
        cout << tokens[0] << " & " << tokens[1] << " = " << result << endl;
    };

    instruction_map["or"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции or" << endl;
            return;
        }
        int result = stoi(variableCheck(variable_map, tokens[0])) | stoi(variableCheck(variable_map, tokens[1]));
        cout << tokens[0] << " | " << tokens[1] << " = " << result << endl;
    };

    instruction_map["xor"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции xor" << endl;
            return;
        }
        int result = stoi(variableCheck(variable_map, tokens[0])) ^ stoi(variableCheck(variable_map, tokens[1]));
        cout << tokens[0] << " ^ " << tokens[1] << " = " << result << endl;
    };

    instruction_map["jmp"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции jmp" << endl;
            return;
        }
        cout << "Выполнена инструкция jmp к инструкции #" << variableCheck(variable_map, tokens[0]) << endl;
        // Перейти к указанной инструкции
        // Например:
        // instruction_pointer = target;
    };

    instruction_map["not"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции not" << endl;
            return;
        }
        int result = ~(stoi(variableCheck(variable_map, tokens[0])));
        cout << "NOT " << tokens[0] << " = " << result << endl;
    };

    instruction_map["mov"] = [](vector<string> tokens) {
        if (tokens.size() != 2) {
            cerr << "ERROR: Неверное количество аргументов для инструкции mov" << endl;
            return;
        }
        variable_map.addVariable(tokens[0], variableCheck(variable_map, tokens[1]));
    };

    instruction_map["inc"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции inc" << endl;
            return;
        }
        int result = stoi(variableCheck(variable_map, tokens[0]));
        cout << "Increment " << tokens[0] << " = " << ++result << endl;
    };

    instruction_map["dec"] = [](vector<string> tokens) {
        if (tokens.size() != 1) {
            cerr << "Ошибка: Неверное количество аргументов для инструкции dec" << endl;
            return;
        }
        int result = stoi(variableCheck(variable_map, tokens[0]));
        cout << "Decrement " << tokens[0] << " = " << --result << endl;
    };

    ifstream input1("input.txt");
    ofstream output1("output.txt");
    bool in_comment = false; // Флаг для отслеживания начала и конца многострочного комментария
    string line;
    while (getline(input1, line)) { // Читаем файл построчно
        string result_line;
        for (int i = 0; i < line.length(); i++) { // Проходим по каждому символу в строке
            if (!in_comment && line[i] == '/' && i < line.length() - 1 && line[i + 1] == '/') {
                // Начало однострочного комментария - игнорируем остаток строки
                break;
            }
            else if (!in_comment && line[i] == '/' && i < line.length() - 1 && line[i + 1] == '*') {
                // Начало многострочного комментария - игнорируем остаток строки
                in_comment = true;
                i++;
            }
            else if (in_comment && line[i] == '*' && i < line.length() - 1 && line[i + 1] == '/') {
                // Конец многострочного комментария
                in_comment = false;
                i++;
            }
            else if (!in_comment) {
                // Символ не является частью комментария - добавляем его к результату
                result_line += line[i];
            }
        }
        if ((result_line != "") && (result_line != " ")) {
            output1 << result_line << endl; // Записываем строку без комментариев в выходной файл
        }
    }
    input1.close();
    output1.close();

    //Подсчет количества строк
    ifstream input2("output.txt");
    int lineCount = 0;
    while (getline(input2, line)) {
        lineCount++;
    }
    input2.close();


    // Считывание и интерпретация инструкций из стандартного ввода
    ifstream input3("output.txt");
    string str, variable, functionname, line_return;
    vector<string> parameters;
    int i = 0, currentLine = 1, functionstart = 0;
    bool infunction = false;
    bool endfunction = false;
    while (getline(input3, str)) {
        // Условия на нахождения в файле фигурных скобок
        if (str.find('{') != string::npos && !infunction) {
            variable_map.pushScope();
            if (str.find('(') != string::npos) {
                parameters.push_back(str.substr(str.find('(') + 1, str.find(',') - str.find('(') - 1));
                parameters.push_back(str.substr(str.find(',') + 1, str.find(')') - str.find(',') - 1));
                functionname = str.substr(0, str.find('('));
                functionstart = currentLine;
                infunction = true;
            }
        }
        else if (infunction){
            line_return = str.substr(str.find(' ') + 1, str.find(';') - str.find(' ') - 1); // Пока от пробела!!!!!!
            infunction = false;
            endfunction = true;
        }
        else if (str.find('}') != string::npos) {
            variable_map.popScope();
            infunction = false;
            if (endfunction){
                Function function;
                function.name = functionname;
                function.startLine = functionstart;
                function.endLine = currentLine;
                function.parameters = parameters;
                function.line_return = line_return;
                functions.insert(make_pair(functionname, function));
                parameters.clear();
            }
            endfunction = false;
        }
        else if (str.find('=') == string::npos && !infunction) {
            for (; !isalnum(str[i]); i++) {}
            // Удаление лишних пробелов
            for (; str[i] != ' '; i++) {}
            str[i] = ',';
            str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
            str[str.find(';')] = ' ';
            i = 0;
            // Разбиение строки на отдельные слова
            replace(str.begin(), str.end(), '(', ',');
            replace(str.begin(), str.end(), ')', ',');
            str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
            vector<string> tokens = split1(str, ',');

            // Проверка наличия инструкции в ассоциативном массиве
            auto it = functions.find(tokens[0]);
            if (it != functions.end()) {
                string numberforfunction = str.substr(str.find('(') + 1, str.find(',') - 1);
                Function function;
                function = it->second;
                variable_map.addVariable(function.parameters[0], tokens[1]);
                variable_map.addVariable(function.parameters[1], tokens[2]);
                int result = evaluate_tree(buildTree(function.line_return));
                cout << it->first << ": " << result << endl;
            }
            else{
                if ((instruction_map.find(tokens[0]) == instruction_map.end())){
                    cerr << "Ошибка: Неизвестная инструкция " << endl;
                    continue;
                }
                // Выполнение инструкции
                instruction_map[tokens[0]](vector<string>(tokens.begin() + 1, tokens.end()));
            }
        }
        else if (str.find('=') && !infunction) {
            str[str.find(';')] = ' ';
            str.erase(remove(str.begin(), str.end(), ' '), str.end());
            string var = str.substr(0, str.find('='));
            string expression = str.substr(str.find('=') + 1);
            int result = evaluate_tree(buildTree(expression));
            cout << "buildTree: " << result << endl;
            variable_map.addVariable(var, to_string(evaluate(expression)));
        }
        currentLine++;
    }

    input3.close();
    return 0;
}
