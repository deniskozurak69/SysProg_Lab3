#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#define RESET   "\033[0m"
#define BLACK   "\033[30m" 
#define GRAY    "\033[90m" 
#define RED     "\033[31m"      
#define GREEN   "\033[32m"      
#define YELLOW  "\033[33m"      
#define BLUE    "\033[34m"      
#define MAGENTA "\033[35m"      
#define CYAN    "\033[36m"      
#define WHITE   "\033[37m"
#define PURPLE     "\033[95m"
using namespace std;
enum LexemeType 
{
    INTEGER,
    FLOAT_NUMBER,
    HEX_NUMBER,
    STRING_CONST,
    CHAR_CONST,
    PREPROCESSOR_DIRECTIVE,
    ONE_STRING_COMMENT,
    MULTI_STRING_COMMENT,
    RESERVED_WORD,
    OPERATOR,
    PUNCTUATION_MARK,
    IDENTIFIER,
    ERROR
};
unordered_map<string, LexemeType> reservedWords = 
{
    {"int", RESERVED_WORD}, {"float", RESERVED_WORD}, {"if", RESERVED_WORD}, {"const", RESERVED_WORD},
    {"else", RESERVED_WORD}, {"for", RESERVED_WORD}, {"while", RESERVED_WORD}, {"do", RESERVED_WORD},
    {"return", RESERVED_WORD}, {"char", RESERVED_WORD}, {"void", RESERVED_WORD}, {"vector", RESERVED_WORD},
    {"double", RESERVED_WORD}, {"long", RESERVED_WORD}, {"short", RESERVED_WORD}, {"unsigned", RESERVED_WORD},
    {"bool", RESERVED_WORD}, {"true", RESERVED_WORD}, {"false", RESERVED_WORD}, {"struct", RESERVED_WORD},
    {"class", RESERVED_WORD},{"public", RESERVED_WORD}, {"private", RESERVED_WORD}, {"enum", RESERVED_WORD},
    {"cin", RESERVED_WORD}, {"cout", RESERVED_WORD}, {"using", RESERVED_WORD}, {"namespace", RESERVED_WORD},
    {"endl", RESERVED_WORD}, {"case", RESERVED_WORD}, {"switch", RESERVED_WORD}, {"auto", RESERVED_WORD},
    {"std", RESERVED_WORD}, {"string", RESERVED_WORD}
};
struct Lexeme 
{
    string text;
    LexemeType type;
};
LexemeType identifyLexeme(const string& lexeme) 
{
    if (regex_match(lexeme, regex("^\\d+\\.\\d+$"))) return FLOAT_NUMBER;
    if (regex_match(lexeme, regex("^0x[0-9A-Fa-f]+$"))) return HEX_NUMBER;
    if (regex_match(lexeme, regex("^\\d+$"))) return INTEGER;
    if (regex_match(lexeme, regex("^[a-zA-Z_][a-zA-Z0-9_]*$"))) 
    {
        if (reservedWords.find(lexeme) != reservedWords.end()) return RESERVED_WORD;            
        return IDENTIFIER;
    }
    if (regex_match(lexeme, regex("^\".*\"$"))) return STRING_CONST;
    if (regex_match(lexeme, regex("^'.'$"))) return CHAR_CONST;
    if (regex_match(lexeme, regex("^#.*$"))) return PREPROCESSOR_DIRECTIVE;
    if (regex_match(lexeme, regex("^//.*$"))) return ONE_STRING_COMMENT;
    if (regex_match(lexeme, regex("^/\\*.*\\*/$"))) return MULTI_STRING_COMMENT;
    if (regex_match(lexeme, regex("^\\+|\\-|\\*|\\/|=|==|!=|<=|>=|&&|\\|\\|$"))) return OPERATOR;
    if (regex_match(lexeme, regex("^[,;{}()\\[\\]]$"))) return PUNCTUATION_MARK;
    return ERROR;
}

vector<Lexeme> SplitIntoLexemes(const string& code) 
{
    vector<Lexeme> Lexemes;
    regex LexemeRegex(R"([\w]+|\d+\.?\d*|0x[\dA-Fa-f]+|".*?"|'.'|#.*|//.*|\+|\-|\*|\/|=|==|!=|<=|>=|&&|\|\||[{}()\[\];,.])");
    auto words_begin = sregex_iterator(code.begin(), code.end(), LexemeRegex);
    auto words_end = sregex_iterator();
    for (sregex_iterator i = words_begin; i != words_end; ++i) 
    {
        string lexeme = (*i).str();
        LexemeType type = identifyLexeme(lexeme);
        Lexemes.push_back({ lexeme, type });
    }
    return Lexemes;
}
void printLexemes(const vector<Lexeme>& Lexemes) 
{
    string LexemeTypeStr,MultiStringComment;
    for (int i=0;i<Lexemes.size();++i)
    {
        
        if (Lexemes[i].type == INTEGER && i + 1 < Lexemes.size() && Lexemes[i + 1].text == ".")
        {
            std::cout << WHITE << "< " << Lexemes[i].text << Lexemes[i + 1].text << Lexemes[i + 2].text << " , ";
            if (i + 2 < Lexemes.size() && Lexemes[i + 2].type == INTEGER) std::cout << "FLOAT_NUMBER" << " >" << "\n";
            else std::cout << RED << "ERROR" << RESET << " >" << "\n";
            i += 3;
        }
        else
        {
            if ((i + 1 < Lexemes.size()) && ((Lexemes[i].text == "=")|| (Lexemes[i].text == "&") ||(Lexemes[i].text == "|")|| (Lexemes[i].text == "+") || (Lexemes[i].text == "-") || (Lexemes[i].text == ":")) && (Lexemes[i + 1].text == Lexemes[i].text))
            {
                std::cout << YELLOW << "< " << Lexemes[i].text << Lexemes[i + 1].text << RESET << " , " << "OPERATOR" << " >" << "\n";
                i += 2;
            }
            else
            {
                if (i + 1 < Lexemes.size() && Lexemes[i].text == "/" && Lexemes[i + 1].text == "*")
                {
                    MultiStringComment = Lexemes[i].text + Lexemes[i + 1].text+" ";
                    i += 2;
                    while (!(Lexemes[i].text == "*" && Lexemes[i + 1].text == "/"))
                    {
                        MultiStringComment += (Lexemes[i].text+" ");
                        ++i;
                        if (i == Lexemes.size() - 1)
                        {
                            MultiStringComment += Lexemes[i].text;
                            std::cout << GRAY << "< " << MultiStringComment << " , " << RED << "ERROR" << RESET << " >" << "\n";
                            return;
                        }
                    }
                    MultiStringComment += Lexemes[i].text + Lexemes[i + 1].text;
                    i += 2;
                    std::cout << GRAY << "< " << MultiStringComment << RESET << " , " << "MULTI_STRING_COMMENT" << " >" << "\n";
                }
                else
                {
                    if ((i + 1 < Lexemes.size()) && (Lexemes[i].text == "long"))
                    {
                        if (Lexemes[i + 1].text == "long" || Lexemes[i + 1].text == "double")
                        {
                            std::cout << CYAN << "< " << Lexemes[i].text << " " << Lexemes[i + 1].text << RESET << " , " << "OPERATOR" << " >" << "\n";
                            i += 2;
                        }
                    }
                    else
                    {
                        if ((i + 1 < Lexemes.size()) && (Lexemes[i+1].text == "="))
                        {
                            if (Lexemes[i].text == "+" || Lexemes[i].text == "-" || Lexemes[i].text == "*" || Lexemes[i].text == "/")
                            {
                                std::cout << CYAN << "< " << Lexemes[i].text << Lexemes[i + 1].text << RESET << " , " << "OPERATOR" << " >" << "\n";
                                i += 2;
                            }
                        }
                    }
                }
            }
        }
        switch (Lexemes[i].type)
        {
            case INTEGER: LexemeTypeStr = "INTEGER"; break; 
            case FLOAT_NUMBER: LexemeTypeStr = "FLOAT_NUMBER"; break; 
            case HEX_NUMBER: LexemeTypeStr = "HEX_NUMBER"; break; 
            case STRING_CONST: LexemeTypeStr = "STRING_CONST"; break; 
            case CHAR_CONST: LexemeTypeStr = "CHAR_CONST"; break; 
            case PREPROCESSOR_DIRECTIVE: LexemeTypeStr = "PREPROCESSOR_DIRECTIVE"; break; 
            case ONE_STRING_COMMENT: LexemeTypeStr = "ONE_STRING_COMMENT"; break; 
            case MULTI_STRING_COMMENT: LexemeTypeStr = "MULTI_STRING_COMMENT"; break;
            case RESERVED_WORD: LexemeTypeStr = "RESERVED_WORD"; break; 
            case OPERATOR: LexemeTypeStr = "OPERATOR"; break; 
            case PUNCTUATION_MARK: LexemeTypeStr = "PUNCTUATION_MARK"; break; 
            case IDENTIFIER: LexemeTypeStr = "IDENTIFIER"; break; 
            default: LexemeTypeStr = "ERROR"; break; 
        }
        if (LexemeTypeStr == "ERROR") std::cout << "< " << RED  << Lexemes[i].text;
        if (LexemeTypeStr == "RESERVED_WORD") std::cout << "< " << CYAN  << Lexemes[i].text;
        if (LexemeTypeStr == "IDENTIFIER") std::cout << "< " << BLUE  << Lexemes[i].text;
        if (LexemeTypeStr == "INTEGER") std::cout << "< " << WHITE  << Lexemes[i].text;
        if (LexemeTypeStr == "FLOAT_NUMBER") std::cout << "< " << WHITE <<  Lexemes[i].text;
        if (LexemeTypeStr == "HEX_NUMBER") std::cout << "< " << WHITE  << Lexemes[i].text;
        if (LexemeTypeStr == "STRING_CONST") std::cout << "< " << MAGENTA  << Lexemes[i].text;
        if (LexemeTypeStr == "CHAR_CONST") std::cout << "< " << MAGENTA  << Lexemes[i].text;
        if (LexemeTypeStr == "ONE_STRING_COMMENT") std::cout << "< " << GRAY  << Lexemes[i].text;
        if (LexemeTypeStr == "MULTI_STRING_COMMENT") std::cout << "< " << GRAY << Lexemes[i].text;
        if (LexemeTypeStr == "PREPROCESSOR_DIRECTIVE") std::cout << "< " << GREEN  << Lexemes[i].text;
        if (LexemeTypeStr == "PUNCTUATION_MARK") std::cout << "< " << YELLOW  << Lexemes[i].text;
        if (LexemeTypeStr == "OPERATOR") std::cout << "< " << YELLOW  << Lexemes[i].text;
        std::cout << RESET << " , " << LexemeTypeStr << " >" << "\n";
    }
}

int main() 
{
    cout << "Please enter the code you want to analyze. Enter ^Z to finish the input" << "\n";
    string code="";
    string code_line;
    while (getline(cin, code_line)) code += (code_line + "\n");
    vector<Lexeme> Lexemes = SplitIntoLexemes(code);
    printLexemes(Lexemes);
    return 0;
}
