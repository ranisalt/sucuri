TODO: Sintático
===============

Trabalho
--------

 - [ ] Gerar uma árvore sintática;
 - [ ] Apresentar um sistema de tratamento de erros;
    - [ ] Detectar o maior número possível de erros em uma única passagem;
 - [ ] Incluir as novas estruturas sintáticas necessárias;

### Erros encontrados e correções

#### "undefined reference: `yy::parser::parser(variant<...>, location)`"

O erro se dá porque o gerador não sabe, por algum motivo, quem é a classe do
parser.
Adicionando `%define parser_class_name {parser}` "corrige" o problema.

#### "no matching function for call to `AST::[...]Expr::[...]Expr()`"

Construções sintáticas do tipo:

```
%type <Type> stmt

stmt
    : valid1;
```

Irá chamar o construtor padrão de `Type` ao reconhecer `valid1`. Definir um
construtor customizado caso ele não exista.

Relatório e Apresentação
------------------------

 - [ ] Gramática completa;
 - [ ] Código fonte;
 - [ ] Exemplos de programas com e sem erros;
 - [ ] Logs gerados pelo compilador mostrando a AST gerada (ou os erros
 - [ ] detectados);
 - [ ] Um relato sucinto da heurística utilizada para o tratamento de erros.


Ideias
------

1. Aproveitar o token `ERROR` (do Bison) para chamar diferentes funções de
   tratamento de erro:

   ```bison
   stmt
       : valid1        { $$ = normal_validation($1); }
       | valid1 valid2 { $$ = normal_validation($1, $2); }
       | valid1 ERROR  { specific_error(); }
   ```

2. Tabela de símbolos:

   ```c++
   class Statement {
   public:
       Statement():
           parent{nullptr},
           root{true}
       {}

       Statement(Statement& parent):
           parent{parent},
           root{false}
       {}

       auto has_symbol(symbol_t& s) {
           if (symbols.count(s)) {
               return true;
           }
           return not root and parent.has_symbol(s);
       }

   private:
       std::set<symbol_t> symbols;
       Statement& parent;
       bool root;
   };


   static auto root = new Statement{root = true};
   ```
