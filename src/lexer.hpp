#pragma once

#include "utils.hpp"
#include <vector>
#include <string>

enum class TokenType {
	None,
	Keyword,
	Identifier,
	IntegerLiteral,
	Colon,
	Semicolon,
	Comma,
	OpenParentheses,
	CloseParentheses,
	OpenBraces,
	CloseBraces,
	OpenBrackets,
	CloseBrackets,
	Plus,
	Minus,
	Mult,
	Div,
	Equal,
	Not,
	Less,
	More,
	Exclamation
};

const char *TokenTypeString(TokenType type);

enum class KeywordType {
	Extern	= 0,
	Fn		= 1,
	Int		= 2,
	If		= 3,
	Else	= 4,
	Return	= 5,
	For		= 6,
	Void    = 7,
	//While = 7
	Count
};

const char *KeywordTypeString(KeywordType type);

using TokenAtom = u64;

constexpr TokenAtom InvalidTokenAtom = -1;

struct Token {
	TokenType Type = TokenType::None;
	union {
		KeywordType KeywordIndex;
		TokenAtom IdentifierIndex;
		u64 IntegerLiteralValue;
	};
	u64 Line = 0;

	bool IsKeyword(KeywordType keyword)const;

	bool IsType(TokenType type)const;

	bool IsDataType()const;
	
	static Token Regular(TokenType type, u64 line);

	static Token Identifier(TokenAtom identifier_index, u64 line);

	static Token Keyword(KeywordType keyword_index, u64 line);

	static Token IntegerLiteral(u64 value, u64 line);

	friend std::ostream& operator<<(std::ostream& stream, const Token& token);
};

class SymbolTable {
private:
	std::vector<std::string> m_Identifiers;
public:
	TokenAtom Add(std::string identifier);

	const std::string& operator[](size_t index)const;

	TokenAtom Find(const std::string &identifier);
};

class CharacterStream {
private:
	std::string m_Data;
	size_t m_Current;
public:
	CharacterStream(std::string data);

	void Consume(u64 count = 1);

	char Peek(u64 offset = 0)const;

	bool ConsumeIfMatch(const char *str);

	bool ConsumeIfMatch(char ch);

	bool End()const;
};

bool IsDigit(char ch);

bool IsLetter(char ch);

bool IsEscape(char ch);

inline u64 ToDigit(char ch){assert(IsDigit(ch)); return ch - '0'; }

class Lexer {
public:
	static std::vector<Token> DoLexicalAnalysis(const std::string &sources, SymbolTable &table);

	static bool TryConsumeSingleCharToken(CharacterStream &stream, TokenType &token);

	static bool TryConsumeInteger(CharacterStream &stream, u64 &number);

	static std::string ConsumeWord(CharacterStream &stream);
};

