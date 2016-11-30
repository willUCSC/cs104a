// Colton Willey
// cwwilley@ucsc.edu
//
// Jacob Janowski
// jnjanows@ucsc.edu

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxlib.h"
#include "lyutils.h"

bool lexer::interactive = true;
location lexer::lloc = {0, 1, 0};
size_t lexer::last_yyleng = 0;
vector<string> lexer::filenames;
FILE *tokout;

astree* parser::root = nullptr;

void set_tokout(string filename) {
   tokout = fopen(filename.c_str(), "w");
}

void write_token(astree *token) {
   fprintf (tokout, "\t%lu %5lu.%03lu  %3d  %-15s (%s)\n", 
      token->lloc.filenr, token->lloc.linenr, 
      token->lloc.offset, token->symbol, 
      parser::get_yytname(token->symbol),
      token->lexinfo->c_str());
}

const string* lexer::filename (int filenr) {
   return &lexer::filenames.at(filenr);
}

void lexer::newfilename (const string& filename) {
   lexer::lloc.filenr = lexer::filenames.size();
   lexer::filenames.push_back (filename);
}

void lexer::advance() {
   if (lexer::lloc.offset == 0) {
   }
   lexer::lloc.offset += last_yyleng;
   last_yyleng = yyleng;
}

void lexer::newline() {
   ++lexer::lloc.linenr;
   lexer::lloc.offset = 0;
}

void lexer::badchar (unsigned char bad) {
   char buffer[16];
   snprintf (buffer, sizeof buffer,
             isgraph (bad) ? "%c" : "\\%03o", bad);
   errllocprintf (lexer::lloc, "invalid source character (%s)\n",
                  buffer);
}

void lexer::badtoken (char* lexeme) {
   errllocprintf (lexer::lloc, "invalid token (%s)\n", lexeme);
}

void lexer::include() {
   size_t linenr;
   static char filename[0x1000];
   assert (sizeof filename > strlen (yytext));
   int scan_rc = sscanf (yytext, "# %zd \"%[^\"]\"", &linenr, filename);
   if (scan_rc != 2) {
      errprintf ("%s: invalid directive, ignored\n", yytext);
   }else {
      if (yy_flex_debug) {
         fprintf (stderr, "--included # %zd \"%s\"\n",
                  linenr, filename);
      }
      lexer::lloc.linenr = linenr - 1;
      lexer::newfilename (filename);
      fprintf(tokout, "%s\n", yytext);
   }
}

int yylval_token (int symbol) {
   yylval = new astree (symbol, lexer::lloc, yytext);
   write_token(yylval);
   return symbol;
}

void yyerror (const char* message) {
   assert (not lexer::filenames.empty());
   errllocprintf (lexer::lloc, "%s\n", message);
}
