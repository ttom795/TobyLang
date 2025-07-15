#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

using namespace std;

// Tokens
enum TokenType {
    IDENT,
    NUMBER,
    OPERATOR,
    PRINT,
    END
};

vector<string> TokenTypeString{
    "Identifier","Number","Operator","Print","End"
};

struct Token {
    TokenType type;
    string value;
};

unordered_map<string, TokenType> keywords = {
    {"+", OPERATOR},
    {"-", OPERATOR},
    {"t", IDENT},
    {"o", IDENT},
    {"b", IDENT},
    {"y", IDENT},
    {"toby", PRINT}
};

unordered_map<string, float> variables;

// Lexer
vector<Token> Lexer(string line) {
    vector<Token> tokens;
    istringstream stream(line);
    string word;
    while (stream >> word) {
        if (keywords.count(word)) {
            tokens.push_back({keywords[word], word});
        } 
        else if (isdigit(word[0])) {
            tokens.push_back({NUMBER, word});
        } 
        else {
            cout << "Syntax Error: Unknown token ->" << word << "<-" << endl;
        }
    }
    tokens.push_back({END, ""});
    return tokens;
}

// AST
enum NodeType {
    NODEPROG, 
    NODENUM, 
    NODEOP, 
    NODEIDENT, 
    NODEPRINT, 
    NODEASSIGN 
};

class NodeStatement {
public:
    NodeType type;
    virtual ~NodeStatement() {}
};

class NodeProgram : public NodeStatement {
public:
    vector<NodeStatement*> nodes;
    NodeProgram() { type = NODEPROG; }
    ~NodeProgram() { for (auto node : nodes) delete node; }
};

class NodeExpression : public NodeStatement {};

class NodeOperator : public NodeExpression {
public:
    NodeExpression* left;
    NodeExpression* right;
    string op;
    NodeOperator(string OpInput, NodeExpression* LeftInput, NodeExpression* RightInput) {
        type = NODEOP;
        op = OpInput;
        left = LeftInput;
        right = RightInput;
    }
    ~NodeOperator() { delete left; delete right; }
};

class NodeIdentifier : public NodeExpression {
public:
    string symbol;
    NodeIdentifier(string SymbolInput) {
        type = NODEIDENT;
        symbol = SymbolInput;
    }
};

class NodeNumber : public NodeExpression {
public:
    float number;
    NodeNumber(float NumberInput) {
        type = NODENUM;
        number = NumberInput;
    }
};

class NodePrint : public NodeStatement {
public:
    NodeExpression* expr;
    NodePrint(NodeExpression* ExprInput) {
        type = NODEPRINT;
        expr = ExprInput;
    }
    ~NodePrint() { delete expr; }
};

class NodeAssign : public NodeStatement {
public:
    string variable;
    NodeExpression* expr;
    NodeAssign(string VariableInput, NodeExpression* ExprInput) {
        type = NODEASSIGN;
        variable = VariableInput;
        expr = ExprInput;
    }
    ~NodeAssign() { delete expr; }
};

// Parser
class Parser {
public:
    vector<Token> tokens;
    bool NotEndOfFile() { return tokens[0].type != END; }
    Token curr() { return tokens[0]; }
    Token eat() { Token prev = curr(); tokens.erase(tokens.begin()); return prev; }

    NodeProgram* ProduceAST(const vector<Token>& TokenInput) {
        tokens = TokenInput;
        NodeProgram* program = new NodeProgram();
        while (NotEndOfFile()) {
            program->nodes.push_back(ParseStatement());
        }
        return program;
    }

    NodeStatement* ParseStatement() {
        if (curr().type == PRINT) {
            eat();
            return new NodePrint(ParseExpression());
        }
    
        if (curr().type == IDENT) {
            string varName = eat().value;

            if (curr().type == NUMBER || curr().type == IDENT){
                return new NodeAssign(varName, ParseExpression());
            }

            return ParseExpression();
        }
        
        return ParseExpression();
    }

    NodeExpression* ParseExpression() { return ParseAdditiveExpression(); }

    NodeExpression* ParseAdditiveExpression() {
        NodeExpression* left = ParsePrimaryExpression();
        while (curr().value == "+" || curr().value == "-") {
            string op = eat().value;
            NodeExpression* right = ParsePrimaryExpression();
            left = new NodeOperator(op, left, right);
        }
        return left;
    }
    
    NodeExpression* ParsePrimaryExpression() {
        Token tk = curr();
        if (tk.type == IDENT) return new NodeIdentifier(eat().value);
        if (tk.type == NUMBER) return new NodeNumber(stof(eat().value));
        
        return new NodeNumber(0);
    }
    
};

// Interpreter
enum RunType { RUNNUM, VOID };

class RuntimeType {
public:
    RunType type;
    virtual ~RuntimeType() {}
};

class RuntimeNumber : public RuntimeType {
public:
    float value;
    explicit RuntimeNumber(float NumberInput) { type = RUNNUM; value = NumberInput; }
};

class RuntimeVoid : public RuntimeType {
public:
    RuntimeVoid() { type = VOID; }
};

class Interpreter {
public:
    RuntimeType* EvaluateProgram(NodeProgram* NodeInput) {
        RuntimeType* lastEvaluated = new RuntimeVoid();
        for (auto node : NodeInput->nodes) {
            delete lastEvaluated;
            lastEvaluated = Evaluate(node);
        }
        return lastEvaluated;
    }

    RuntimeType* EvaluateNumbers(RuntimeNumber* LeftIn, RuntimeNumber* RightIn, string OpIn) {
        float result = (OpIn == "+") ? LeftIn->value + RightIn->value : LeftIn->value - RightIn->value;
        return new RuntimeNumber(result);
    }

    RuntimeType* Evaluate(NodeStatement* StatementInput) {
        switch (StatementInput->type) {
            case NODENUM:
                return new RuntimeNumber(static_cast<NodeNumber*>(StatementInput)->number);
            case NODEOP: {
                auto* opNode = static_cast<NodeOperator*>(StatementInput);
                auto* left = Evaluate(opNode->left);
                auto* right = Evaluate(opNode->right);
                if (left->type == RUNNUM && right->type == RUNNUM) {
                    return EvaluateNumbers(static_cast<RuntimeNumber*>(left), static_cast<RuntimeNumber*>(right), opNode->op);
                }
                return new RuntimeVoid();
            }
            case NODEIDENT: {
                string var = static_cast<NodeIdentifier*>(StatementInput)->symbol;
                if (variables.count(var)) return new RuntimeNumber(variables[var]);
                cout << "Undefined variable ->" << var << "<-" << endl;
                return new RuntimeVoid();
            }
            case NODEASSIGN: {
                auto* assignNode = static_cast<NodeAssign*>(StatementInput);
                auto* result = Evaluate(assignNode->expr);
                if (result->type == RUNNUM) variables[assignNode->variable] = static_cast<RuntimeNumber*>(result)->value;
                return new RuntimeVoid();
            }
            case NODEPRINT: {
                auto* printNode = static_cast<NodePrint*>(StatementInput);
                auto* result = Evaluate(printNode->expr);
                if (result->type == RUNNUM) cout << static_cast<RuntimeNumber*>(result)->value << endl;
                return new RuntimeVoid();
            }
            default:
                return new RuntimeVoid();
        }
    }
};

// Main
int main(int argc, char* argv[]) {
    string input;
    Interpreter interpreter;
    NodeProgram* program;
    Parser parser;
    
    cout << "TobyLang Interpreter. Type 'exit' to quit.\n";
    
    if (argc == 2) {
        ifstream inputFile(argv[1]);

        if (!inputFile) {
            cout << "Error: Could not open file " << argv[1] << endl;
            return 1;
        }
        cout << "TobyLang Interpreter. Processing file: " << argv[1] << endl;
        while (getline(inputFile, input)) {
            vector<Token> tokens = Lexer(input);
            program = parser.ProduceAST(tokens);
            interpreter.EvaluateProgram(program);
            delete program;
        }
        
        inputFile.close();
    } 

    while (true) {
        cout << "> ";
        getline(cin, input);
        if (input == "exit") break;
        vector<Token> tokens = Lexer(input);
        program = parser.ProduceAST(tokens);
        interpreter.EvaluateProgram(program);
        delete program;
    }
    return 0;
}
