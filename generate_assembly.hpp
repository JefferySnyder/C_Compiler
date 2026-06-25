#include <iostream>
#include <format>
#include <ranges>
#include "parser.hpp"
#include "context.hpp"

size_t clause_idx = 0;
std::string gen_clause() {
	return "_clause" + std::to_string(clause_idx++);
}
size_t end_idx = 0;
std::string gen_end() {
	return "_end" + std::to_string(end_idx++);
}
size_t else_idx = 0;
std::string gen_else() {
	return "_else" + std::to_string(else_idx++);
}
size_t end_if_idx = 0;
std::string gen_end_if() {
	return "_end_if" + std::to_string(end_if_idx++);
}
size_t end_cond_idx = 0;
std::string gen_end_cond() {
	return "_end_cond" + std::to_string(end_cond_idx++);
}
size_t while_idx = 0;
std::string gen_while() {
	return "_while" + std::to_string(while_idx++);
}
size_t end_while_idx = 0;
std::string gen_end_while() {
	return "_end_while" + std::to_string(end_while_idx++);
}
size_t for_idx = 0;
std::string gen_for() {
	return "_for" + std::to_string(for_idx++);
}
size_t end_for_idx = 0;
std::string gen_end_for() {
	return "_end_for" + std::to_string(end_for_idx++);
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
		std::cout << "\tpush\trax\n";
		generateNodeAsm(bNode->right.get(), var_map);
		std::cout << "\tpop\trcx\n";
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
		auto& key = vNode->var_name;
		if (!var_map.contains(key)) {
			std::cerr << "Variable '" << key << "' doesn't exist (in this scope)\n";
			throw std::runtime_error("Failed to generate");
		}
		std::cout << std::format("\tmov\t[rbp{:+}], eax\n", var_map.at(key));
	}
	else if (node->type == ASTNodeType::CompoundAssign) {
		auto caNode = static_cast<const CompoundAssignNode*>(node);
		generateNodeAsm(caNode->exp.get(), var_map);
		auto vNode = static_cast<const VarNode*>(caNode->var.get());
		auto var_offset = var_map.find(vNode->var_name);
		if (var_offset == var_map.end()) {
			std::cerr << "Variable '" << vNode->var_name << "' doesn't exist (in this scope)\n";
			throw std::runtime_error("Failed to generate");
		}
		else if (caNode->op == TokenType::PlusEquals) {
			std::cout << std::format("\tadd\t[rbp{}], eax\n", var_offset->second);
		}
		else if (caNode->op == TokenType::MinusEquals) {
			std::cout << std::format("\tsub\t[rbp{}], eax\n", var_offset->second);
		}
	}
	else if (node->type == ASTNodeType::Var) {
		auto vNode = static_cast<const VarNode*>(node);
		auto& key = vNode->var_name;
		if (!var_map.contains(key)) {
			std::cerr << "Variable '" << key << "' doesn't exist (in this scope)\n";
			throw std::runtime_error("Failed to generate");
		}
		std::cout << std::format("\tmov\teax, [rbp{:+}]\n", var_map.at(key));
	}
	else if (node->type == ASTNodeType::ConditionalExpr) {
		auto cNode = static_cast<const ConditionalExprNode*>(node);
		auto else_clause = gen_else();
		auto end_cond = gen_end_cond();
		generateNodeAsm(cNode->if_exp.get(), var_map);
		std::cout << "\tcmp\teax, 0\n";
		std::cout << std::format("\tje\t{}\n", else_clause);
		generateNodeAsm(cNode->then_exp.get(), var_map);
		std::cout << std::format("\tjmp\t{}\n", end_cond);
		std::cout << else_clause << ":\n";
		generateNodeAsm(cNode->else_exp.get(), var_map);
		std::cout << end_cond << ":\n";
	}
	else if (node->type == ASTNodeType::FunCall) {
		auto fcNode = static_cast<const FunCallNode*>(node);
		// 16 byte align first (saves 8 bit return address)
		auto offset = ((fcNode->args.size() + 1) * 8) % 16;
		std::cout << std::format("\tsub\trsp, {}\n", offset);
		// push args to stack in reverse order
		for (const auto& a : std::views::reverse(fcNode->args)) {
			generateNodeAsm(a.get(), var_map);
			std::cout << "\tpush\trax\n";
		}
		std::cout << std::format("\tcall\t{}\n", fcNode->name);
		// deallocate args
		std::cout << std::format("\tadd\trsp, {}\n", fcNode->args.size() * 8 + offset);
	}
}

void generateBlockAsm(const ASTNode*, Context&);
void generateBlockItemAsm(const ASTNode*, Context&);

void generateStmtAsm(const ASTNode* stmt, Context& context) {
	if (stmt->type == ASTNodeType::ReturnStmt) {
		auto rStmt = static_cast<const ReturnStmtNode*>(stmt);
		generateNodeAsm(rStmt->exp.get(), context.var_map);
		// leave
		std::cout << "\n";
		std::cout << "\tmov\trsp, rbp\n";
		std::cout << "\tpop\trbp\n";
		std::cout << "\tret\n";
	}
	else if (stmt->type == ASTNodeType::ExprStmt) {
		auto eStmt = static_cast<const ExprStmtNode*>(stmt);
		generateNodeAsm(eStmt->exp.get(), context.var_map);
	}
	else if (stmt->type == ASTNodeType::ConditionalStmt) {
		auto cStmt = static_cast<const ConditionalStmtNode*>(stmt);
		generateNodeAsm(cStmt->exp.get(), context.var_map);
		std::cout << "\tcmp\teax, 0\n";
		if (cStmt->else_branch == nullptr) {
			std::string end_if_name = gen_end_if();
			std::cout << std::format("\tje\t{}\n", end_if_name);
			generateStmtAsm(cStmt->if_branch.get(), context);
			std::cout << end_if_name << ":\n";
		}
		else {
			std::string else_name = gen_else();
			std::string end_if_name = gen_end_if();
			std::cout << std::format("\tje\t{}\n", else_name);
			generateStmtAsm(cStmt->if_branch.get(), context);
			std::cout << std::format("\tjmp\t{}\n", end_if_name);
			std::cout << else_name << ":\n";
			generateStmtAsm(cStmt->else_branch.get(), context);
			std::cout << end_if_name << ":\n";
		}
	}
	else if (stmt->type == ASTNodeType::CompoundStmt) {
		generateBlockAsm(stmt, context);
	}
	else if (stmt->type == ASTNodeType::WhileStmt) {
		auto wStmt = static_cast<const WhileStmtNode*>(stmt);
		auto while_name = gen_while();
		std::cout << while_name << ":\n";
		generateNodeAsm(wStmt->init.get(), context.var_map);
		std::cout << "\tcmp\teax, 0\n";
		auto end_while_name = gen_end_while();
		std::cout << std::format("\tje\t{}\n", end_while_name);
		context.break_label = end_while_name;
		context.continue_label = while_name;
		generateStmtAsm(wStmt->body.get(), context);
		std::cout << std::format("\tjmp\t{}\n", while_name);
		std::cout << end_while_name << ":\n";
	}
	else if (stmt->type == ASTNodeType::DoStmt) {
		auto dStmt = static_cast<const DoStmtNode*>(stmt);
		auto while_name = gen_while();
		std::cout << while_name << ":\n";
		auto end_while_name = gen_end_while();
		context.break_label = end_while_name;
		context.continue_label = while_name;
		generateStmtAsm(dStmt->body.get(), context);
		generateNodeAsm(dStmt->init.get(), context.var_map);
		std::cout << "\tcmp\teax, 0\n";
		std::cout << std::format("\tjne\t{}\n", while_name);
		std::cout << end_while_name << ":\n";
	}
	else if (stmt->type == ASTNodeType::ForStmt) {
		auto fStmt = static_cast<const ForStmtNode*>(stmt);
		auto& var_map_copy = context.var_map;
		auto for_init = fStmt->init.get();
		if (for_init != nullptr && for_init->type == ASTNodeType::Declaration) {
			generateBlockItemAsm(for_init, context);
		} else {
			generateNodeAsm(for_init, context.var_map);
		}
		auto for_name = gen_for();
		std::cout << for_name << ":\n";
		generateNodeAsm(fStmt->cond.get(), context.var_map);
		std::cout << "\tcmp\teax, 0\n";
		auto end_for_name = gen_end_for();
		std::cout << std::format("\tje\t{}\n", end_for_name);
		context.break_label = end_for_name;
		context.continue_label = for_name;
		generateStmtAsm(fStmt->body.get(), context);
		generateNodeAsm(fStmt->post.get(), context.var_map);
		std::cout << std::format("\tjmp\t{}\n", for_name);
		std::cout << end_for_name << ":\n";
		context.var_map = var_map_copy;
	}
	else if (stmt->type == ASTNodeType::BreakStmt) {
		std::cout << std::format("\tjmp\t{}\n", context.break_label);
	}
	else if (stmt->type == ASTNodeType::ContinueStmt) {
		std::cout << std::format("\tjmp\t{}\n", context.continue_label);
	}
}

void generateBlockItemAsm(const ASTNode* block_item, Context& context) {
	if (block_item->type == ASTNodeType::Declaration) {
		auto decl = static_cast<const DeclarationNode*>(block_item);
		if (context.var_map.contains(decl->var_name)) {
			std::cerr << "Declared '" << decl->var_name << "' twice\n";
			throw std::runtime_error("Failed to generate");
		}
		generateNodeAsm(decl->exp.get(), context.var_map);
		context.stack_index -= 8;
		std::cout << std::format("\tmov\t[rbp{}], eax\n", context.stack_index);
		context.var_map.insert({decl->var_name, context.stack_index});
	}
	else {
		generateStmtAsm(block_item, context);
	}
}

void generateBlockAsm(const ASTNode* node, Context& context) {
	if (node->type != ASTNodeType::CompoundStmt) {
		return;
	}
	auto& var_map_copy = context.var_map;
	auto cpStmt = static_cast<const CompoundStmtNode*>(node);
	for (const auto& b : cpStmt->block) {
		generateBlockItemAsm(b.get(), context);
	}
	context.var_map = var_map_copy;
}

void generateFuncAssembly(const FunctionNode* func) {
	std::cout << func->name << ":\n";
	std::cout << "\tpush\trbp\n";
	std::cout << "\tmov\trbp, rsp\n\n";
	Context context;
	int param_offset = 8;
	for (const auto& param : func->params) {
		param_offset += 8;
		context.var_map.insert({ param, param_offset });
	}
	generateBlockAsm(func->body.get(), context);
}

void generateAssembly(const ProgramNode* program) {
	std::cout << "global _start" << ":\n\n";

	for (const auto& f : program->funcs) {
		generateFuncAssembly(f.get());
	}

	std::cout << "_start" << ":\n";
	std::cout << "\tcall\tmain\n";

	std::cout << "\tmov\trdi, rax\n";
	std::cout << "\tmov\trax, 60\n";
	std::cout << "\tsyscall\n";
}
