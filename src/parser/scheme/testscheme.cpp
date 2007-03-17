
#include "scheme.h"
#include "lexer.h"
#include "parser.h"

void test_tokenizer() {
    Lexer* l = new Lexer("(+ 1.5 (2 . \"Hej\") .x)");
    assert(l->nextToken() == Lexer::OPEN_PAREN);
    assert(l->nextToken() == Lexer::SYMBOL);
    assert(l->getString() == "+");
    assert(l->nextToken() == Lexer::NUMBER);
    assert(l->getNumber() == 1.5);
    assert(l->nextToken() == Lexer::OPEN_PAREN);
    assert(l->nextToken() == Lexer::NUMBER);
    assert(l->getNumber() == 2);
    assert(l->nextToken() == Lexer::PERIOD);
    assert(l->nextToken() == Lexer::STRING);
    assert(l->getString() == "Hej");
    assert(l->nextToken() == Lexer::CLOSE_PAREN);
    assert(l->nextToken() == Lexer::SYMBOL);
    assert(l->nextToken() == Lexer::CLOSE_PAREN);
    assert(l->nextToken() == Lexer::END);
    delete l;

    l = new Lexer("#f #tf");
    assert(l->nextToken() == Lexer::BOOLEAN);
    assert(l->getBool() == false);
    assert(l->nextToken() == Lexer::BOOLEAN);
    assert(l->getBool() == true);
    assert(l->nextToken() == Lexer::SYMBOL);
    assert(l->nextToken() == Lexer::END);
    
    l = new Lexer("a");
    assert(l->nextToken() == Lexer::SYMBOL);
    assert(l->getString() == "a");
    assert(l->nextToken() == Lexer::END);
}

void test_parser() {
    Lexer* l = new Lexer("(+ 1.5 (list? \"Hej\"))");
    Parser* p = new Parser(l);
    SchemePair* t = p->parse();
    SchemePair* e = static_cast<SchemePair*> (t->car);
    assert(e->car->type() == SchemeObject::SYMBOL);
    assert(e->cdrAsPair()->car->type() == SchemeObject::NUMBER);
    SchemePair* inner = static_cast<SchemePair*> (e->cdrAsPair()->cdrAsPair()->car);
    assert(inner->car->type() == SchemeObject::SYMBOL);
    assert(inner->car->toString() == "list?");
    assert(inner->cdrAsPair()->car->type() == SchemeObject::STRING);
    assert(inner->cdrAsPair()->cdrAsPair()->type() == SchemeObject::EMPTY_LIST);
    assert(e->cdrAsPair()->cdrAsPair()->cdrAsPair()->type() == SchemeObject::EMPTY_LIST);
    
    l = new Lexer("'(x . y)");
    p = new Parser(l);
    t = p->parse();
    e = static_cast<SchemePair*> (t->car);
    assert(e->car->type() == SchemeObject::SYMBOL);
    assert(e->cdrAsPair()->car->type() == SchemeObject::PAIR);
    assert(static_cast<SchemePair*>(e->cdrAsPair()->car)->car->type() == SchemeObject::SYMBOL);
    assert(static_cast<SchemePair*>(e->cdrAsPair()->car)->car->toString() == "x");
    assert( static_cast<SchemePair*>(e->cdrAsPair()->car)->cdr->toString() == "y");
}

void test_bools() {
    Scheme* s = new Scheme();
    assert(s->eval("(bool? #t)") == S_TRUE);
    assert(s->eval("(bool? #f)") == S_TRUE);
    assert(s->eval("(bool? 1)") == S_FALSE);
    assert(s->eval("(bool? '(1 2 3))") == S_FALSE);
}

void test_lists() {
    SchemePair* p = s_cons(NULL,new SchemeSymbol("x"),new SchemeSymbol("y"));
    assert(p->toString() == "(x . y)");
    
    Scheme* s = new Scheme();
    assert(s->eval("(list? '())") == S_TRUE);
    assert(s->eval("(list? '(1 2 3))") == S_TRUE);
    assert(s->eval("(list? 1)") == S_FALSE);
    assert(s->eval("(list? '(1 2 . 3))") == S_FALSE);
    assert(s->eval("(pair? 1)") == S_FALSE);
    assert(s->eval("(pair? '())") == S_FALSE);
    assert(s->eval("(pair? '(1 2))") == S_TRUE);
    assert(s->eval("(pair? '(1 2 . 3))") == S_TRUE);
}

int main(int argc, char *argv[]) {
    try {
        test_tokenizer();
        test_parser();
        test_bools();
        test_lists();
    } catch (scheme_exception e) {
		cerr << "Exception: " << e.str << endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
