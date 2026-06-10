#include <iostream>
#include <format>
#include "parser.hpp"

size_t clause_idx = 0;
std::string gen_clause() {
	return "_clause" + std::to_string(clause_idx++);
}
size_t end_idx = 0;
std::string gen_end() {
	return "_end" + std::to_string(end_idx++);
}

void generateNodeAsm(const ASTNode* node, std::unordered_map<std::string, int>& var_map) {
	if (node == nullptr) {
		std::cout << "\tmov\teax, 0\n";
	}
	else if (node->type == ASTNodeType::Literal) {
		auto lNode = static_cast<const LiteralNode*>(node);
		std::cout << "\tmov\teax, " << lNode->value << "\n";
	}
	else if (node->type == ASTNodeType::Unary) {
		auto uNode = static_cast<const UnaryNode*>(node);
		generateNodeAsm(uNode->right.get(), var_map);
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
	else if (node->type == ASTNodeType::Binary) {
		using enum TokenType;
		auto bNode = static_cast<const BinaryNode*>(node);
		if (bNode->op == OR) {
			auto clause = gen_clause();
			auto end = gen_end();
			generateNodeAsm(bNode->left.get(), var_map);
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tje\t" << clause << "\n";
			std::cout << "\tmov\teax, 1\n";
			std::cout << "\tjmp\t" << end << "\n";
			std::cout << clause << ":\n";
			generateNodeAsm(bNode->right.get(), var_map);
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tmov\teax, 0\n";
			std::cout << "\tsetne\tal\n";
			std::cout << end << ":\n";
			return;
		}
		if (bNode->op == AND) {
			auto clause = gen_clause();
			auto end = gen_end();
			generateNodeAsm(bNode->left.get(), var_map);
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tjne\t" << clause << "\n";
			std::cout << "\tjmp\t" << end << "\n";
			std::cout << clause << ":\n";
			generateNodeAsm(bNode->right.get(), var_map);
			std::cout << "\tcmp\teax, 0\n";
			std::cout << "\tmov\teax, 0\n";
			std::cout << "\tsetne\tal\n";
			std::cout << end << ":\n";
			return;
		}
		generateNodeAsm(bNode->left.get(), var_map);
		std::cout << "\tpush\trax" << "\n";
		generateNodeAsm(bNode->right.get(), var_map);
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
	else if (node->type == ASTNodeType::Assign) {
		auto aNode = static_cast<const AssignNode*>(node);
		generateNodeAsm(aNode->exp.get(), var_map);
		auto vNode = static_cast<const VarNode*>(aNode->var.get());
		auto var_offset = var_map.find(vNode->var_name);
		std::cout << std::format("\tmov\t[rbp{}], eax\n", var_offset->second);
	}
	else if (node->type == ASTNodeType::CompoundAssign) {
		auto caNode = static_cast<const CompoundAssignNode*>(node);
		generateNodeAsm(caNode->exp.get(), var_map);
		auto vNode = static_cast<const VarNode*>(caNode->var.get());
		auto var_offset = var_map.find(vNode->var_name);
		if (caNode->op == TokenType::PlusEquals) {
			std::cout << std::format("\tadd\t[rbp{}], eax\n", var_offset->second);
		}
		else if (caNode->op == TokenType::MinusEquals) {
			std::cout << std::format("\tsub\t[rbp{}], eax\n", var_offset->second);
		}
	}
	else if (node->type == ASTNodeType::Var) {
		auto vNode = static_cast<const VarNode*>(node);
		auto var_offset = var_map.find(vNode->var_name);
		std::cout << std::format("\tmov\teax, [rbp{}]\n", var_offset->second);
	}
}

void generateStmtAsm(const ASTNode* stmt, std::unordered_map<std::string, int>& var_map, int& stack_index) {
	if (stmt->type == ASTNodeType::ReturnStmt) {
		auto rStmt = static_cast<const ReturnStmtNode*>(stmt);
		generateNodeAsm(rStmt->exp.get(), var_map);
	}
	else if (stmt->type == ASTNodeType::DeclareStmt) {
		auto dStmt = static_cast<const DeclareStmtNode*>(stmt);
		if (var_map.contains(dStmt->var_name)) {
			std::cerr << "Declared " << dStmt->var_name << " twice\n";
		}
		generateNodeAsm(dStmt->exp.get(), var_map);
		std::cout << "\tpush\trax\n";
		stack_index -= 8;
		var_map.insert({dStmt->var_name, stack_index});
	}
	else if (stmt->type == ASTNodeType::ExprStmt) {
		auto eStmt = static_cast<const ExprStmtNode*>(stmt);
		generateNodeAsm(eStmt->exp.get(), var_map);
	}
}

void generateFuncAssembly(const FunctionNode* func) {
	std::unordered_map<std::string, int> var_map;
	int stack_index = 0;
	std::cout << "main:\n";
	std::cout << "\tpush\trbp\n";
	std::cout << "\tmov\trbp, rsp\n\n";
	for (const auto& b : func->body) {
		generateStmtAsm(b.get(), var_map, stack_index);
	}
	// leave
	std::cout << "\n\tmov\trsp, rbp\n";
	std::cout << "\tpop\trbp\n";
	std::cout << "\tret\n";
}

void generateAssembly(const FunctionNode* func) {
	if (func->name != "main") {
		std::cerr << "function name needs to be main";
		return;
	}
	std::cout << "global _start" << ":\n\n";

	generateFuncAssembly(func);

	std::cout << "_start" << ":\n";
	std::cout << "\tcall\tmain\n";

	std::cout << "\tmov\trdi, rax\n";
	std::cout << "\tmov\trax, 60\n";
	std::cout << "\tsyscall\n";
}
