#include <unordered_set>
#include <functional>
#include "lexer.hpp"

enum class ASTNodeType { Function, ReturnStmt, Declaration, ExprStmt, ConditionalStmt, CompoundStmt, ForStmt, WhileStmt, DoStmt, BreakStmt, ContinueStmt, Literal, Unary, Binary, Assign, Var, CompoundAssign, ConditionalExpr };

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
	TokenType op;
	std::unique_ptr<ASTNode> right;
	UnaryNode(TokenType o, std::unique_ptr<ASTNode> r) 
		: op{ o }, right{ std::move(r) } 
	{
		type = ASTNodeType::Unary;
	};
};

struct BinaryNode : public ASTNode {
	TokenType op;
	std::unique_ptr<ASTNode> left;
	std::unique_ptr<ASTNode> right;
	BinaryNode(TokenType o, std::unique_ptr<ASTNode> l , std::unique_ptr<ASTNode> r) 
		: op{ o }, left{ std::move(l) } , right{ std::move(r) } 
	{
		type = ASTNodeType::Binary;
	};
};

struct AssignNode : public ASTNode {
	std::unique_ptr<ASTNode> var;
	std::unique_ptr<ASTNode> exp;
	AssignNode(std::unique_ptr<ASTNode> v, std::unique_ptr<ASTNode> e) 
		: var{ std::move(v) }, exp{ std::move(e) } 
	{
		type = ASTNodeType::Assign;
	};
};

struct CompoundAssignNode : public ASTNode {
	std::unique_ptr<ASTNode> var;
	TokenType op;
	std::unique_ptr<ASTNode> exp;
	CompoundAssignNode(std::unique_ptr<ASTNode> v, TokenType o, std::unique_ptr<ASTNode> e) 
		: var{ std::move(v) }, op{ o }, exp{std::move(e)}
	{
		type = ASTNodeType::CompoundAssign;
	};
};

struct VarNode : public ASTNode {
	std::string var_name;
	VarNode(std::string vn) : var_name{vn}
	{
		type = ASTNodeType::Var;
	};
};

struct ConditionalExprNode : public ASTNode {
	std::unique_ptr<ASTNode> if_exp;
	std::unique_ptr<ASTNode> then_exp;
	std::unique_ptr<ASTNode> else_exp;
	ConditionalExprNode(std::unique_ptr<ASTNode> i, std::unique_ptr<ASTNode> t, std::unique_ptr<ASTNode> e) 
		: if_exp{ std::move(i) }, then_exp{ std::move(t) }, else_exp{ std::move(e) }
	{
		type = ASTNodeType::ConditionalExpr;
	};
};

struct ReturnStmtNode : public ASTNode {
	std::unique_ptr<ASTNode> exp;
	ReturnStmtNode(std::unique_ptr<ASTNode> e) : exp{ std::move(e) }
	{
		type = ASTNodeType::ReturnStmt;
	};
};

struct DeclarationNode : public ASTNode {
	std::string var_name;
	std::unique_ptr<ASTNode> exp;
	DeclarationNode(std::string vn, std::unique_ptr<ASTNode> e = nullptr) 
		: var_name{ vn }, exp { std::move(e) }
	{
		type = ASTNodeType::Declaration;
	};
};

struct ExprStmtNode : public ASTNode {
	std::unique_ptr<ASTNode> exp;
	ExprStmtNode(std::unique_ptr<ASTNode> e) : exp{ std::move(e) }
	{
		type = ASTNodeType::ExprStmt;
	};
};

struct ConditionalStmtNode : public ASTNode {
	std::unique_ptr<ASTNode> exp;
	std::unique_ptr<ASTNode> if_branch;
	std::unique_ptr<ASTNode> else_branch;
	ConditionalStmtNode(std::unique_ptr<ASTNode> e, std::unique_ptr<ASTNode> ib, std::unique_ptr<ASTNode> eb = nullptr) 
		: exp{ std::move(e) }, if_branch{ std::move(ib) }, else_branch{ std::move(eb) }
	{
		type = ASTNodeType::ConditionalStmt;
	};
};

struct CompoundStmtNode : public ASTNode {
	std::vector<std::unique_ptr<ASTNode>> block;
	CompoundStmtNode(std::vector<std::unique_ptr<ASTNode>> b) 
		:block{ std::move(b) }
	{
		type = ASTNodeType::CompoundStmt;
	};
};

struct ForStmtNode : public ASTNode {
	std::unique_ptr<ASTNode> init;
	std::unique_ptr<ASTNode> cond;
	std::unique_ptr<ASTNode> post;
	std::unique_ptr<ASTNode> body;
	ForStmtNode(std::unique_ptr<ASTNode> i, std::unique_ptr<ASTNode> c, std::unique_ptr<ASTNode> p, std::unique_ptr<ASTNode> b)
		:init{ std::move(i) }, cond{ std::move(c) }, post{ std::move(p) }, body{ std::move(b) }
	{
		type = ASTNodeType::ForStmt;
	};
};

struct WhileStmtNode : public ASTNode {
	std::unique_ptr<ASTNode> init;
	std::unique_ptr<ASTNode> body;
	WhileStmtNode(std::unique_ptr<ASTNode> i, std::unique_ptr<ASTNode> b)
		:init{ std::move(i) }, body{ std::move(b) }
	{
		type = ASTNodeType::WhileStmt;
	};
};

struct DoStmtNode : public ASTNode {
	std::unique_ptr<ASTNode> body;
	std::unique_ptr<ASTNode> init;
	DoStmtNode(std::unique_ptr<ASTNode> b, std::unique_ptr<ASTNode> i)
		:body{ std::move(b) }, init{ std::move(i) }
	{
		type = ASTNodeType::DoStmt;
	};
};

struct BreakStmtNode : public ASTNode {
	BreakStmtNode() { type = ASTNodeType::BreakStmt; };
};

struct ContinueStmtNode : public ASTNode {
	ContinueStmtNode() { type = ASTNodeType::ContinueStmt; };
};

struct FunctionNode : public ASTNode {
	std::string name;
	std::unique_ptr<ASTNode> body;
	FunctionNode(std::string n, std::unique_ptr<ASTNode> b) 
		: name{ n }, body { std::move(b) }
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
		ensure(TokenType::Int);
		auto name = ensure(TokenType::Identifier); // "main"
		ensure(TokenType::OpenParen);
		// Parameters here
		ensure(TokenType::CloseParen);
		return std::make_unique<FunctionNode>(name, parse_block());
	}
	std::unique_ptr<ASTNode> parse_block() {
		ensure(TokenType::OpenBrace);
		std::vector<std::unique_ptr<ASTNode>> block;
		while (tokens.at(index).type != TokenType::CloseBrace) {
			block.emplace_back(parse_block_item());
		}
		auto res = std::make_unique<CompoundStmtNode>(std::move(block));
		//skip "}"
		ensure(TokenType::CloseBrace);
		return res;
	}
	std::unique_ptr<ASTNode> parse_block_item() {
		std::unique_ptr<ASTNode> res;
		if (tokens[index].type == TokenType::Int) {
			res = parse_declaration();
		}
		else {
			res = parse_stmt();
		}
		return res;
	}
	std::unique_ptr<ASTNode> parse_declaration() {
		ensure(TokenType::Int);
		auto var_name = ensure(TokenType::Identifier);
		std::unique_ptr<ASTNode> res;
		if (tokens[index].type == TokenType::Assignment) {
			ensure(TokenType::Assignment);
			auto exp = parse_expr();
			res = std::make_unique<DeclarationNode>(var_name, std::move(exp));
		}
		else {
			res = std::make_unique<DeclarationNode>(var_name);
		}
		ensure(TokenType::Semicolon);
		return res;
	}
	std::unique_ptr<ASTNode> parse_stmt() {
		std::unique_ptr<ASTNode> res;
		if (tokens[index].type == TokenType::Return) {
			ensure(TokenType::Return);
			auto exp = parse_expr();
			res = std::make_unique<ReturnStmtNode>(std::move(exp));
			ensure(TokenType::Semicolon);
		}
		else if (tokens[index].type == TokenType::If) {
			ensure(TokenType::If);
			ensure(TokenType::OpenParen);
			auto exp = parse_expr();
			ensure(TokenType::CloseParen);
			auto if_branch = parse_stmt();
			if (tokens[index].type == TokenType::Else) {
				ensure(TokenType::Else);
				auto else_branch = parse_stmt();
				res = std::make_unique<ConditionalStmtNode>(std::move(exp), std::move(if_branch), std::move(else_branch));
			}
			else {
				res = std::make_unique<ConditionalStmtNode>(std::move(exp), std::move(if_branch));
			}
		}
		else if (tokens[index].type == TokenType::OpenBrace) {
			res = parse_block();
		}
		else if (tokens[index].type == TokenType::For) {
			ensure(TokenType::For);
			ensure(TokenType::OpenParen);
			std::unique_ptr<ASTNode> init;
			if (tokens[index].type == TokenType::Int) {
				init = parse_declaration();
			}
			else {
				init = parse_opt_expr();
				ensure(TokenType::Semicolon);
			}
			auto cond = parse_opt_expr();
			if (cond == nullptr) {
				cond = std::make_unique<LiteralNode>(1);
			}
			ensure(TokenType::Semicolon);
			auto post = parse_opt_expr();
			ensure(TokenType::CloseParen);
			auto stmt = parse_stmt();
			res = std::make_unique<ForStmtNode>(std::move(init), std::move(cond), std::move(post), std::move(stmt));
		}
		else if (tokens[index].type == TokenType::While) {
			ensure(TokenType::While);
			ensure(TokenType::OpenParen);
			auto exp = parse_expr();
			ensure(TokenType::CloseParen);
			auto stmt = parse_stmt();
			res = std::make_unique<WhileStmtNode>(std::move(exp), std::move(stmt));
		}
		else if (tokens[index].type == TokenType::Do) {
			ensure(TokenType::Do);
			auto stmt = parse_stmt();
			ensure(TokenType::While);
			ensure(TokenType::OpenParen);
			auto exp = parse_expr();
			ensure(TokenType::CloseParen);
			res = std::make_unique<DoStmtNode>(std::move(stmt), std::move(exp));
			ensure(TokenType::Semicolon);
		}
		else if (tokens[index].type == TokenType::Break) {
			ensure(TokenType::Break);
			ensure(TokenType::Semicolon);
		}
		else if (tokens[index].type == TokenType::Continue) {
			ensure(TokenType::Continue);
			ensure(TokenType::Semicolon);
		}
		else {
			res = std::make_unique<ExprStmtNode>(parse_opt_expr());
			ensure(TokenType::Semicolon);
		}
		return res;
	}
	std::unique_ptr<ASTNode> parse_opt_expr() {
		if (tokens[index].type == TokenType::Semicolon || tokens[index].type == TokenType::CloseParen)
			return nullptr;
		return parse_expr();
	}
	std::unique_ptr<ASTNode> parse_expr() {
		auto id = parse_assign_expr();
		std::unordered_set<TokenType> caops{TokenType::PlusEquals, TokenType::MinusEquals};
		if (caops.contains(tokens[index].type) && id->type == ASTNodeType::Var) {
			auto tt = tokens[index++].type;
			auto right = parse_expr();
			id = std::make_unique<CompoundAssignNode>(std::move(id), tt, std::move(right));
		}
		return id;
	}
	std::unique_ptr<ASTNode> parse_assign_expr() {
		auto id = parse_conditional_expr();
		if (tokens[index].type == TokenType::Assignment && id->type == ASTNodeType::Var) {
			ensure(TokenType::Assignment);
			auto right = parse_expr();
			id = std::make_unique<AssignNode>(std::move(id), std::move(right));
		}
		return id;
	}
	std::unique_ptr<ASTNode> parse_conditional_expr() {
		auto if_exp = parse_or_expr();
		if (tokens.at(index).type == TokenType::QuestionMark) {
			ensure(TokenType::QuestionMark);
			auto then_exp = parse_expr();
			ensure(TokenType::Colon);
			auto else_exp = parse_conditional_expr();
			if_exp = std::make_unique<ConditionalExprNode>(std::move(if_exp), std::move(then_exp), std::move(else_exp));
		}
		return if_exp;
	}
	std::unique_ptr<ASTNode> parse_or_expr() {
		auto left = parse_and_expr();
		while (tokens.at(index).type == TokenType::OR) {
			auto op = tokens.at(index++).type;
			auto right = parse_and_expr();
			left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
		}
		return left;
	}
	std::unique_ptr<ASTNode> parse_and_expr() {
		auto left = parse_equality_expr();
		while (tokens.at(index).type == TokenType::AND) {
			auto op = tokens.at(index++).type;
			auto right = parse_equality_expr();
			left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
		}
		return left;
	}
	std::unique_ptr<ASTNode> parse_equality_expr() {
		auto left = parse_relational_expr();
		while (tokens.at(index).type == TokenType::Equal || tokens.at(index).type == TokenType::NotEqual) {
			auto op = tokens.at(index++).type;
			auto right = parse_relational_expr();
			left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
		}
		return left;
	}
	std::unique_ptr<ASTNode> parse_relational_expr() {
		auto left = parse_add_expr();

		while (tokens.at(index).type == TokenType::LessThan
			|| tokens.at(index).type == TokenType::GreaterThan
			|| tokens.at(index).type == TokenType::LessThanOrEqual
			|| tokens.at(index).type == TokenType::GreaterThanOrEqual) {
			auto op = tokens.at(index++).type;
			auto right = parse_add_expr();
			left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
		}
		return left;
	}
	std::unique_ptr<ASTNode> parse_add_expr() {
		auto left = parse_term();
		while (tokens.at(index).type == TokenType::Plus || tokens.at(index).type == TokenType::Minus) {
			auto op = tokens.at(index++).type;
			auto right = parse_term();
			left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
		}
		return left;
	}
	std::unique_ptr<ASTNode> parse_term() {
		auto left = parse_factor();
		while (tokens.at(index).type == TokenType::Asterisk || tokens.at(index).type == TokenType::ForwardSlash) {
			auto op = tokens.at(index++).type;
			auto right = parse_factor();
			left = std::make_unique<BinaryNode>(op, std::move(left), std::move(right));
		}
		return left;
	}
	std::unique_ptr<ASTNode> parse_factor() {
		auto peek_tok_type = tokens[index].type;
		if (peek_tok_type == TokenType::OpenParen) {
			ensure(TokenType::OpenParen);
			auto res = parse_expr();
			ensure(TokenType::CloseParen);
			return res;
		}
		std::unordered_set<TokenType> un_ops{TokenType::Bang, TokenType::Tilde, TokenType::Minus};
		if (un_ops.contains(peek_tok_type)) {
			auto un_op = peek_tok_type;
			ensure(peek_tok_type);
			auto expr = parse_factor();
			return std::make_unique<UnaryNode>(un_op, std::move(expr));
		}
		if (peek_tok_type == TokenType::IntegerLiteral) {
			auto value = ensure(TokenType::IntegerLiteral);
			return std::make_unique<LiteralNode>(std::stoi(value));
		}
		if (peek_tok_type == TokenType::Identifier) {
			auto value = ensure(TokenType::Identifier);
			return std::make_unique<VarNode>(value);
		}
		fail();
		return nullptr;
	}


	std::string ensure(TokenType comp) {
		if (tokens.at(index).type != comp) {
			fail();
		}
		return tokens.at(index++).value;
	}
	void fail() {
		std::cerr << "failed at token: " << tokens.at(index).value<< std::endl;
		throw std::runtime_error("Failed to parse");
		//std::cin.get();
		//std::exit(EXIT_FAILURE);
	}
};
