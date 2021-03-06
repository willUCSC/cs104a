#ifndef __SYMSTACK__
#define __SYMSTACK__H

#include "astree.h"
#include <stdlib.h>
#include <unordered_map>
using namespace std;

struct symbol;
using symbol_table = unordered_map<string*,symbol*>;
using symbol_entry = symbol_table::value_type;

struct symbol {
	attr_bitset attributes;
	symbol_table* fields;
	size_t filenr;
	size_t linenr;
	size_t offset;
	size_t block_nr;
	vector<symbol*>* parameters;
	symbol(astree*);
};

void init_symtables(astree* node);
void build_symtables(astree* node);
void processNode(astree* node);

#endif