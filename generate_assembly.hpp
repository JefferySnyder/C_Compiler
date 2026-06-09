#include <iostream>
#include "parser.hpp"

size_t clause_idx = 0;
std::string gen_clause() {
	return "_clause" + std::to_string(clause_idx++);
}
size_t end_idx = 0;
std::string gen_end() {
	return "_end" + std::to_string(end_idx++);
}

void generateNodeAsm(const ASTNode* node) {
	if (node->type == ASTNodeType::Literal) {
		auto lNode = static_cast<const LiteralNode*>(node);
		std::cout << "\tmov\teax, " << lNode->value << "\n";
	}
	if (node->type == ASTNodeType::Unary) {
		auto uNode = static_cast<const UnaryNode*>(node);
		generateNodeAsm(uNode->right.get());
		if (uNode->op == TokenType::Minus) {
			std::cout << "\tneg\teax" << "\n";
		}
		if (uNode->op == TokenType::Tilde) {
			std::cout << "\tnot\teax" << "\n";
		}
		if (uNode->op == TokenType::Bang) {
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tmov\teax, 0\n";
			std::cout << "\tsete\tal\n";
		}
	}
	if (node->type == ASTNodeType::Binary) {
		using enum TokenType;
		auto bNode = static_cast<const BinaryNode*>(node);
		if (bNode->op == OR) {
			auto clause = gen_clause();
			auto end = gen_end();
			generateNodeAsm(bNode->left.get());
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tje\t" << clause << "\n";
			std::cout << "\tmov\teax, 1\n";
			std::cout << "\tjmp\t" << end << "\n";
			std::cout << clause << ":\n";
			generateNodeAsm(bNode->right.get());
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tmov\teax, 0\n";
			std::cout << "\tsetne\tal\n";
			std::cout << end << ":\n";
			return;
		}
		if (bNode->op == AND) {
			auto clause = gen_clause();
			auto end = gen_end();
			generateNodeAsm(bNode->left.get());
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tjne\t" << clause << "\n";
			std::cout << "\tjmp\t" << end << "\n";
			std::cout << clause << ":\n";
			generateNodeAsm(bNode->right.get());
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tmov\teax, 0\n";
			std::cout << "\tsetne\tal\n";
			std::cout << end << ":\n";
			return;
		}
		generateNodeAsm(bNode->left.get());
		std::cout << "\tpush\trax" << "\n";
		generateNodeAsm(bNode->right.get());
		std::cout << "\tpop\trcx" << "\n";
		if (bNode->op == Plus) {
			std::cout << "\tadd\teax, ecx\n"; return;
		}
		if (bNode->op == Minus) {
			std::cout << "\txchg\teax, ecx\n";
			std::cout << "\tsub\teax, ecx\n";
			return;
		}
		if (bNode->op == Asterisk) {
			std::cout << "\timul\teax, ecx\n"; return;
		}
		if (bNode->op == ForwardSlash) {
			std::cout << "\txchg\teax, ecx\n";
			std::cout << "\tcdq\n";
			std::cout << "\tidiv\tecx\n";
			return;
		}
		std::unordered_set<TokenType> stt{Equal, NotEqual, GreaterThan, LessThan, GreaterThanOrEqual, LessThanOrEqual};
		if (stt.contains(bNode->op)) {
			std::unordered_set<TokenType> sub_stt{GreaterThan, LessThan, GreaterThanOrEqual, LessThanOrEqual};
			if (sub_stt.contains(bNode->op)) std::cout << "\txchg\teax, ecx\n";
			std::cout << "\tcmp\teax, ecx\n";
			std::cout << "\tmov\teax, 0\n\t";
			if (bNode->op == Equal) std::cout << "sete";
			if (bNode->op == NotEqual) std::cout << "setne";
			if (bNode->op == GreaterThan) std::cout << "setg";
			if (bNode->op == LessThan) std::cout << "setl";
			if (bNode->op == GreaterThanOrEqual) std::cout << "setge";
			if (bNode->op == LessThanOrEqual) std::cout << "setle";
			std::cout << "\tal\n";
			return;
		}
	}
}

void generateStmtAsm(const ASTNode* stmt) {
	if (stmt->type == ASTNodeType::ReturnStmt) {
		auto rStmt = static_cast<const ReturnStmtNode*>(stmt);
		generateNodeAsm(rStmt->exp.get());
	}
}

void generateAssembly(const FunctionNode* func) {
	if (func->name != "main") {
		std::cerr << "function name needs to be main";
		return;
	}
	std::cout << "global _start" << ":\n\n";
	std::cout << "_start" << ":\n";
	for (const auto& b : func->body) {
		generateStmtAsm(b.get());
	}
	std::cout << "\tmov\tebx, eax\n";
	std::cout << "\tmov\teax, 1\n";
	std::cout << "\tint\t0x80\n";
}
