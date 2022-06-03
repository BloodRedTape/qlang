#include "ast/function.hpp"

u64 FunctionParamDecl::TryParse(const TokenStream& stream, u64 start){
	Token type		 = stream.Peek(start + 0);
	Token identifier = stream.Peek(start + 1);

	if(!type.IsKeyword(KeywordType::Int) || !identifier.IsType(TokenType::Identifier))
		return 0;

	DataType = type.KeywordIndex;
	IdentifierIndex = identifier.IdentifierIndex;

	return 2;
}

u64 AstFunctionDeclaration::TryParse(const TokenStream& stream, u64 start){
	u64 offset = 0;

	if (stream.Peek(start + offset).IsKeyword(KeywordType::Extern)) {
		Qualifiers = FunctionQualifier::Extern;
		offset += 1;
	}

	Token fn_keyword = stream.Peek(start + offset++);
	Token identifier = stream.Peek(start + offset++);
	
	if(!fn_keyword.IsKeyword(KeywordType::Fn)
	|| !identifier.IsType(TokenType::Identifier))
		return 0;

	Identifier = identifier.IdentifierIndex;

	if(!stream.Peek(start + offset++).IsType(TokenType::OpenParentheses))// (
		return 0;

	for (;;) {
		FunctionParamDecl param;
		u64 count = param.TryParse(stream, start + offset);

		offset += count;

		if (!count && stream.Peek(start + offset).IsType(TokenType::Comma)){
			offset += 1;
			continue;	
		}

		if(!count){
			break;
		}

		Parameters.push_back(param);
	}

	if(!stream.Peek(start + offset++).IsType(TokenType::CloseParentheses)
	|| !stream.Peek(start + offset++).IsType(TokenType::Colon))// ):
		return 0;

	Token ret = stream.Peek(start + offset++);

	if(!ret.IsDataType())
		return 0;

	ReturnType = ret.KeywordIndex;

	if(Qualifiers == FunctionQualifier::Extern)
		return offset;

	CompoundStatement body;

	u64 body_count = body.TryParse(stream, start + offset);

	if(!body_count)
		return 0;

	Body = std::move(body);

	return offset + body_count;
}

