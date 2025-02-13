#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

// Token types
enum TokenType {
    VAR,      // Variable assignment (c, o, d, e)
    IF,       // "cod" (if statement)
    COMPARE,  // "od" (greater than)
    PRINT,    // "codecode" (print)
    BOOL_TRUE,  // "co" (true)
    BOOL_FALSE, // "de" (false)
    NUMBER,   // Integer numbers
    END       // End of statement
};

struct Token {
    TokenType type;
    string value;
};

unordered_map<string, TokenType> keywords = {
    {"t", VAR}, {"o", VAR}, {"b", VAR}, {"y", VAR},
    {"tob", IF}, {"ob", COMPARE}, {"toby", PRINT},
    {"to", BOOL_TRUE}, {"by", BOOL_FALSE}
};

unordered_map<string, int> variables;

vector<Token> lexer(string line) {
    vector<Token> tokens;
    istringstream stream(line);
    string word;
    while (stream >> word) {
        if (keywords.count(word)) {
            tokens.push_back({keywords[word], word});
        } else if (isdigit(word[0])) {
            tokens.push_back({NUMBER, word});
        } else {
            cout << "Syntax Error: Unknown token " << word << endl;
        }
    }
    tokens.push_back({END, ""});
    return tokens;
}

void parser(const vector<Token>& tokens) {
    if (tokens.empty()) return;

    // Variable assignment working
    if (tokens[0].type == VAR && tokens.size() >= 2 && tokens[1].type == NUMBER) {
        variables[tokens[0].value] = stoi(tokens[1].value); 
    }

    // Greater than comparison statement semi working
    else if (tokens[0].type == IF && tokens.size() >= 5 && tokens[1].type == VAR &&
             tokens[2].type == COMPARE && tokens[3].type == VAR && tokens[4].type == PRINT) {
        if (variables[tokens[1].value] > variables[tokens[3].value]) {
            if (tokens[5].type == VAR) {
                cout << variables[tokens[5].value] << endl;
            } else if (tokens[5].type == BOOL_TRUE) {
                cout << "true" << endl;
            } else if (tokens[5].type == BOOL_FALSE) {
                cout << "false" << endl;
            }
        }
    }

    // Printing working
    else if (tokens[0].type == PRINT && tokens.size() >= 2) {
        if (tokens[1].type == VAR) {
            cout << variables[tokens[1].value] << endl;
        } else if (tokens[1].type == BOOL_TRUE) {
            cout << "true" << endl;
        } else if (tokens[1].type == BOOL_FALSE) {
            cout << "false" << endl;
        }
    } 
    
    else {
        cout << "Syntax Error: Invalid statement" << endl;
    }
}

int main() {
    string input;
    cout << "TobyLang Interpreter. Type 'exit' to quit.\n";
    while (true) {
        cout << "> ";
        getline(cin, input);
        if (input == "exit") break;
        if (input.find('.') != string::npos) {
            // Assume it's a filename, open and process the file
            ifstream file(input);
            string line;
            while (getline(file, line)) {
                vector<Token> tokens = lexer(line);
                parser(tokens);
            }
        } else {
            // Treat the input as a direct command
            vector<Token> tokens = lexer(input);
            parser(tokens);
        }
    }
    return 0;
}
