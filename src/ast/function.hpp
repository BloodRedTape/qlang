#pragma once

#include <vector>
#include <optional>
#include "lexer.hpp"
#include "ast/ast.hpp"
#include "ast/statement.hpp"

struct FunctionParamDecl {
	KeywordType DataType = KeywordType::Count;
	TokenAtom IdentifierIndex = InvalidTokenAtom;

	u64 TryParse(const TokenStream &stream, u64 start);
};

namespace FunctionQualifier{
	enum Type: u8{
		None   = 0x00,
		Extern = 0x01
	};
}//namespace FunctionQualifiers::

struct AstFunctionDeclaration: AstNode{
	FunctionQualifier::Type Qualifiers = FunctionQualifier::None;
	TokenAtom Identifier = InvalidTokenAtom;

	std::vector<FunctionParamDecl> Parameters;
	KeywordType ReturnType = KeywordType::Count;

	std::optional<CompoundStatement> Body;

	AstFunctionDeclaration(){ Type = AstNodeType::FunctionDeclaration; }

	u64 TryParse(const TokenStream &stream, u64 start);
};
