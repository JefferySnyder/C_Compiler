#include <iostream>
#include "parser.hpp"

void generateNodeAsm(const ASTNode* node) {
	if (node->type == ASTNodeType::Literal) {
		auto lNode = static_cast<const LiteralNode*>(node);
		std::cout << "\tmov\teax, " << lNode->value << "\n";
	}
	if (node->type == ASTNodeType::Unary) {
		auto uNode = static_cast<const UnaryNode*>(node);
		generateNodeAsm(uNode->right.get());
		if (uNode->op == "-") {
			std::cout << "\tneg\teax" << "\n";
		}
		if (uNode->op == "~") {
			std::cout << "\tnot\teax" << "\n";
		}
		if (uNode->op == "!") {
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tsete\tal\n";
			std::cout << "\tmovzx\teax, 0\n";
		}
	}
	if (node->type == ASTNodeType::Binary) {
		auto bNode = static_cast<const BinaryNode*>(node);
		generateNodeAsm(bNode->left.get());
		std::cout << "\tpush\trax" << "\n";
		generateNodeAsm(bNode->right.get());
		std::cout << "\tpop\trcx" << "\n";
		if (bNode->op == "+") std::cout << "\tadd";
		if (bNode->op == "-") {
			std::cout << "\txchg\teax, ecx\n";
			std::cout << "\tsub";
		}
		if (bNode->op == "*") std::cout << "\timul";
		if (bNode->op == "/") {
			std::cout << "\txchg\teax, ecx\n";
			std::cout << "\tcdq\n";
			std::cout << "\tidiv\tecx\n";
			return;
		}

		std::cout << "\teax, ecx" << "\n";
	}
}

void generateStmtAsm(const StatementNode* stmt) {
	generateNodeAsm(stmt->exp.get());
}

void generateAssembly(const FunctionNode* func) {
	if (func->name != "main") {
		std::cerr << "function name needs to be main";
		return;
	}
	std::cout << "global _start" << ":\n\n";
	std::cout << "_start" << ":\n";
	generateStmtAsm(func->body.get());
	std::cout << "\tmov\tebx, eax\n";
	std::cout << "\tmov\teax, 1\n";
	std::cout << "\tint\t0x80\n";
}
