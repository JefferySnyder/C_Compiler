#include "lexer.hpp"

enum class ExprNodeType { Literal, Unary };

struct ExprNode { 
	ExprNodeType type;
	virtual ~ExprNode() = default; 
};

struct LiteralNode : public ExprNode {
	int value;
	LiteralNode(int val) : value{ val } {
		type = ExprNodeType::Literal;
	};
};

struct UnaryNode : public ExprNode {
	std::string op;
	std::unique_ptr<ExprNode> right;
	UnaryNode(std::string o, std::unique_ptr<ExprNode> r) 
		: op{ o }, right{ std::move(r) } 
	{
		type = ExprNodeType::Unary;
	};
};

struct StatementNode {
	std::string type;
	std::unique_ptr<ExprNode> exp;
	StatementNode(std::string t, std::unique_ptr<ExprNode> e) : type{ t }, exp{ std::move(e) } {};
};

struct FunctionNode {
	std::string name;
	std::unique_ptr<StatementNode> body;
	FunctionNode(std::string n, std::unique_ptr<StatementNode> b) : name{ n }, body{ std::move(b) } {};
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
		auto type = ensure(TokenType::Return);
		auto exp = parse_expr();
		auto res = std::make_unique<StatementNode>(type, std::move(exp));
		//skip ";"
		ensure(TokenType::Semicolon);
		return res;
	}
	std::unique_ptr<ExprNode> parse_expr() {
		if (tokens[index].type == TokenType::IntegerLiteral) {
			auto value = std::stoi(ensure(TokenType::IntegerLiteral));
			auto res = std::make_unique<LiteralNode>(value);
			return res;
		}
		//check if token type in (!,~,-)
		std::vector<std::string> validUnary{"!","~","-"};
		auto op = tokens[index].value;
		if (std::find(validUnary.begin(), validUnary.end(), op) == validUnary.end())
			fail(); ++index;
		auto inner_exp = parse_expr();
		return std::make_unique<UnaryNode>(op, std::move(inner_exp));
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
