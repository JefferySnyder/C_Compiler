#include <unordered_set>
#include "lexer.hpp"

enum class ASTNodeType { Function, Statement, Literal, Unary, Binary };

struct ASTNode { 
	ASTNodeType type;
	virtual ~ASTNode() = default; 
};

struct LiteralNode : public ASTNode {
	int value;
	LiteralNode(int val) : value{ val } {
		type = ASTNodeType::Literal;
	};
};

struct UnaryNode : public ASTNode {
	std::string op;
	std::unique_ptr<ASTNode> right;
	UnaryNode(std::string o, std::unique_ptr<ASTNode> r) 
		: op{ o }, right{ std::move(r) } 
	{
		type = ASTNodeType::Unary;
	};
};

struct BinaryNode : public ASTNode {
	std::string op;
	std::unique_ptr<ASTNode> left;
	std::unique_ptr<ASTNode> right;
	BinaryNode(std::string o, std::unique_ptr<ASTNode> l , std::unique_ptr<ASTNode> r) 
		: op{ o }, left{ std::move(l) } , right{ std::move(r) } 
	{
		type = ASTNodeType::Binary;
	};
};

struct StatementNode : public ASTNode {
	std::unique_ptr<ASTNode> exp;
	StatementNode(std::unique_ptr<ASTNode> e) : exp{ std::move(e) }
	{
		type = ASTNodeType::Statement;
	};
};

struct FunctionNode : public ASTNode {
	std::string name;
	std::unique_ptr<StatementNode> body;
	FunctionNode(std::string n, std::unique_ptr<StatementNode> b) : name{ n }, body{ std::move(b) }
	{
		type = ASTNodeType::Function;
	}
};


class Parser {
	std::vector<Token> tokens;
	size_t index = 0;
public:
	explicit Parser(std::vector<Token> t) : tokens{ t } {};
	std::unique_ptr<FunctionNode> parse() {
		return parse_func();
	}
private:
	std::unique_ptr<FunctionNode> parse_func() {
		//skip "int"
		ensure(TokenType::Int);
		// "main"
		auto name = ensure(TokenType::Identifier);
		//skip "() {"
		ensure(TokenType::OpenParen);
		// Parameters here
		ensure(TokenType::CloseParen);
		ensure(TokenType::OpenBrace);
		auto body = parse_stmt();
		auto res = std::make_unique<FunctionNode>(name, std::move(body));
		//skip "}"
		ensure(TokenType::CloseBrace);
		return res;
	}
	std::unique_ptr<StatementNode> parse_stmt() {
		// "return"
		ensure(TokenType::Return);
		auto exp = parse_expr();
		auto res = std::make_unique<StatementNode>(std::move(exp));
		//skip ";"
		ensure(TokenType::Semicolon);
		return res;
	}
	std::unique_ptr<ASTNode> parse_expr() {
		auto left = parse_term();
		while (tokens[index].type == TokenType::Plus || tokens[index].type == TokenType::Minus) {
			auto op = tokens[index++].value;
			auto right = parse_term();
			left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
		}
		return left;
	}
	std::unique_ptr<ASTNode> parse_term() {
		auto left = parse_factor();
		while (tokens[index].type == TokenType::Asterisk || tokens[index].type == TokenType::ForwardSlash) {
			auto op = tokens[index++].value;
			auto right = parse_factor();
			left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
		}
		return left;
	}
	std::unique_ptr<ASTNode> parse_factor() {
		std::unordered_set<TokenType> un_ops{TokenType::Bang, TokenType::Tilde, TokenType::Minus};
		auto next_tok = tokens[index++];
		if (next_tok.type == TokenType::OpenParen) {
			auto res = parse_expr();
			ensure(TokenType::CloseParen);
			return res;
		}
		if (un_ops.contains(next_tok.type)) {
			auto un_op = next_tok.value;
			auto expr = parse_factor();
			return std::make_unique<UnaryNode>(un_op, std::move(expr));
		}
		if (next_tok.type == TokenType::IntegerLiteral) {
			return std::make_unique<LiteralNode>(std::stoi(next_tok.value));
		}
		fail();
	}


	std::string ensure(TokenType comp) {
		if (tokens[index].type != comp) {
			fail();
		}
		return tokens[index++].value;
	}
	void fail() {
		std::cerr << "failed at token: " << tokens[index].value << std::endl;
		//std::runtime_error("Failed to parse");
		std::cin.get();
		std::exit(EXIT_FAILURE);
	}
};
