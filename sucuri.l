D [0-9]
L [a-zA-Z_!@$?]
NO_SQUOTE_STRING_LITERAL [^']
NO_DQUOTE_STRING_LITERAL [^"]

%{

#include <iostream>
#include "y.tab.h"

void count();
void indent_level();

%}

%%

"#"         { comment(); }

    /* literals */
{D}+        { count(); return INTEGER_LITERAL; }
{D}*"."{D}+ { count(); return FLOAT_LITERAL; }
{D}+"."{D}* { count(); return FLOAT_LITERAL; }
\'{NO_SQUOTE_STRING_LITERAL}\' { count(); return STRING_LITERAL; }
\"{NO_DQUOTE_STRING_LITERAL}\" { count(); return STRING_LITERAL; }

    /* operators */
"!="        { count(); return NE; }
"("         { count(); return '('; }
")"         { count(); return ')'; }
"*"         { count(); return '*'; }
"**"        { count(); return POW; }
"+"         { count(); return '+'; }
"-"         { count(); return '-'; }
"/"         { count(); return '/'; }
"<"         { count(); return '<'; }
"<="        { count(); return LE; }
"="         { count(); return '='; }
">"         { count(); return '>'; }
">="        { count(); return GE; }
"and"       { count(); return AND; }
"not"       { count(); return NOT; }
"or"        { count(); return OR; }
"xor"       { count(); return XOR; }

    /* keywords */
"as"        { count(); return AS; }
"class"     { count(); return CLASS; }
"else"      { count(); return ELSE; }
"export"    { count(); return EXPORT; }
"for"       { count(); return FOR; }
"from"      { count(); return FROM; }
"if"        { count(); return IF; }
"import"    { count(); return IMPORT; }
"in"        { count(); return IN; }
"let"       { count(); return LET; }
"return"    { count(); return RETURN; }
"throw"     { count(); return THROW; }
"while"     { count(); return WHILE; }

    /* extra */
"..."                 { count(); return ELLIPSIS; }
{L}((\.{L})|{L}|{D})* { count(); return IDENTIFIER; }
