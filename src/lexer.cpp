#include "lexer.hpp"
#include <cmath>
#include <sstream>


const char* TokenTypeString(TokenType type){
	switch(type){
	case TokenType::None: return "None";
	case TokenType::Keyword: return "Keyword";
	case TokenType::Identifier: return "Identifier";
	case TokenType::IntegerLiteral: return "IntegerLiteral";
	case TokenType::Colon: return "Colon";
	case TokenType::Semicolon: return "Semicolon";
	case TokenType::Comma: return "Comma";
	case TokenType::OpenParentheses: return "OpenParentheses";
	case TokenType::CloseParentheses: return "CloseParentheses";
	case TokenType::OpenBraces: return "OpenBraces";
	case TokenType::CloseBraces: return "CloseBraces";
	case TokenType::OpenBrackets: return "OpenBrackets";
	case TokenType::CloseBrackets: return "CloseBrackets";
	case TokenType::Plus: return "Plus";
	case TokenType::Minus: return "Minus";
	case TokenType::Mult: return "Mult";
	case TokenType::Div: return "Div";
	case TokenType::Equal: return "Equal";
	case TokenType::Not: return "Not";
	case TokenType::Less: return "Less";
	case TokenType::More: return "More";
	default: return (assert(false), "Shit happens");
	}
}

const char* KeywordTypeString(KeywordType type){
	switch (type) {
	case KeywordType::Extern: return "extern";
	case KeywordType::Fn: return "fn";
	case KeywordType::Int: return "int";
	case KeywordType::If: return "if";
	case KeywordType::Else: return "else";
	case KeywordType::Return: return "return";
	case KeywordType::For: return "for";
	default: return (assert(false), "Shit happens");
	}
}

Token Token::Regular(TokenType type){
	return {type};
}

Token Token::Identifier(TokenAtom identifier_index){
	Token token = {TokenType::Identifier};
	token.IdentifierIndex = identifier_index;
	return token;
}

Token Token::Keyword(KeywordType keyword_index){
	Token token = {TokenType::Keyword};
	token.KeywordIndex = keyword_index;
	return token;
}

Token Token::IntegerLiteral(u64 value){
	Token token = {TokenType::IntegerLiteral};
	token.IntegerLiteralValue = value;
	return token;
}

TokenAtom SymbolTable::Add(std::string identifier){
	m_Identifiers.push_back(std::move(identifier));
	return m_Identifiers.size() - 1;
}

const std::string& SymbolTable::operator[](size_t index) const{
	return m_Identifiers[index];
}

CharacterStream::CharacterStream(std::string data):
	m_Data(data),
	m_Current(0)
{}

void CharacterStream::Consume(u64 count){
	m_Current += count;
}

char CharacterStream::Peek(u64 offset)const{
	if(m_Current + offset>= m_Data.size())
		return '\0';
	return m_Data[m_Current + offset];
}

bool CharacterStream::ConsumeIfMatch(const char* str){
	u64 len = strlen(str);

	if(len > m_Data.size() - m_Current)
		return false;

	for(int i = 0; i < len; i++) {
		if(str[i] != m_Data[m_Current + i])
			return false;
	}

	Consume(len);

	return true;
}

bool CharacterStream::ConsumeIfMatch(char ch){
	if (Peek() != ch)
		return false;

	Consume(1);
	return true;
}

bool CharacterStream::End() const{
	return m_Current >= m_Data.size();
}

bool IsDigit(char ch) {
	return ch >= '0' && ch <= '9';
}

bool IsLetter(char ch) {
	return ch >= 'a' && ch <= 'z'
		|| ch >= 'A' && ch <= 'Z'
		|| ch == '_';
}

bool IsEscape(char ch){
	return ch == '\n'
		|| ch == '\t'
		|| ch == '\r';
}

static const std::pair<TokenType, char> s_SingleCharTokenTable[] = {
	{TokenType::Colon,				':'},
	{TokenType::Semicolon,			';'},
	{TokenType::Comma,				','},
	{TokenType::OpenParentheses,	'('},
	{TokenType::CloseParentheses,	')'},
	{TokenType::OpenBraces,			'{'},
	{TokenType::CloseBraces,		'}'},
	{TokenType::OpenBrackets,		'['},
	{TokenType::CloseBrackets,		']'},
	{TokenType::Plus,				'+'},
	{TokenType::Minus,				'-'},
	{TokenType::Mult,				'*'},
	{TokenType::Div,				'/'},
	{TokenType::Equal,				'='},
	{TokenType::Not,				'!'},
	{TokenType::Less,				'<'},
	{TokenType::More,				'>'}
};

std::vector<Token> Lexer::DoLexicalAnalysis(const std::string& sources, SymbolTable& table){
	std::vector<Token> tokens;
	CharacterStream chars(sources);

	while (!chars.End()) {
		if (chars.Peek() == ' ' || IsEscape(chars.Peek())) {
			chars.Consume();
			continue;
		}
			
		TokenType token_type;
		if(TryConsumeSingleCharToken(chars, token_type)){
			tokens.push_back(Token::Regular(token_type));
			continue;
		}
				
		u64 number = 0;
		if (TryConsumeInteger(chars, number)) {
			tokens.push_back(Token::IntegerLiteral(number));
			continue;
		}

		std::string word = ConsumeWord(chars);

		if(word.size()){
			KeywordType keyword = KeywordType::Count;
			for (int i = 0; i < (int)KeywordType::Count; i++) {
				KeywordType current = (KeywordType)i;
				if (KeywordTypeString(current) == word) {
					keyword = current;
					break;
				}
			}

			if(keyword != KeywordType::Count)
				tokens.push_back(Token::Keyword(keyword));
			else
				tokens.push_back(Token::Identifier(table.Add(std::move(word))));

			continue;
		}
		
		assert(false);
	}

	return tokens;
}

bool Lexer::TryConsumeSingleCharToken(CharacterStream& chars, TokenType& token){
	for(int i = 0; i<std::size(s_SingleCharTokenTable); i++){
		if(chars.ConsumeIfMatch(s_SingleCharTokenTable[i].second)){
			token = s_SingleCharTokenTable[i].first;
			return true;
		}
	}
	return false;
}

bool Lexer::TryConsumeInteger(CharacterStream& chars, u64& number){
	int digits = 0;
	number = 0;
	
	while (IsDigit(chars.Peek())){
		number = number * std::pow(10, digits++) + ToDigit(chars.Peek());
		chars.Consume();
	}

	return digits;
}

std::string Lexer::ConsumeWord(CharacterStream& chars){
	std::stringstream result;

	for(int i = 0; !chars.End(); i++){
		char ch = chars.Peek(i);

		assert(i == 0 ? !IsDigit(ch) : true);

		bool is_word_char = IsLetter(ch) || IsDigit(ch);

		if(!is_word_char){
			chars.Consume(i);
			break;
		}

		result << ch;
	}
	return result.str();
}
