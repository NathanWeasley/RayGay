
#include "scheme.h"
#include "lexer.h"
#include "interpreter.h"
#include "parser.h"
#include <sstream>
#include <exception>
#include "testing.h"

/*
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
assert_eval(s, L"", L"");
*/

int errors_found = 0;

void assert_eval(Scheme* s, wstring expression, wstring expected) {
    if (expression == L"") {
        return;
    }
    
    uint32_t stacksize_before = stack.size();

    try {
//        cout << "Testing " << expression << endl;
        wstring result = s->eval(expression)->toString();
        if (result != expected) {
            errors_found++;
            wcerr << L"FAILED: " << expression;
            wcerr << L" expected " << expected;
            wcerr << L" got " << result << endl; 
        }  
    } catch (scheme_exception e) {
        errors_found++;
        wcerr << L"FAILED: " << expression << L": " << e.toString() << endl; 
    } catch (exception e) {
        errors_found++;
        wcerr << L"FAILED: " << expression << L": general exception" << endl; 
    }
    if (stacksize_before != stack.size()) {
        wcerr << L"FAILED: " << expression << L" made stack explode" << endl;
    }
}

void assert_fail(Scheme* s, wstring expression) {
    uint32_t stacksize_before = stack.size();
    try {
        s->eval(expression);
        errors_found++;
        wcerr << L"FAILED: " << expression;
        wcerr << L" didn't fail" << endl;
    } catch (scheme_exception e) {
        if (stacksize_before != stack.size()) {
            wcerr << L"FAILED: stack exploded when " << expression << L" failed." << endl;
        }
    }
}

void test_tokenizer() {
    wistream* is = new wistringstream(L"(+ 1.5 (2 . \"\\\\\\aHej\\\"\") .x)");
    Lexer* l = new Lexer();
    assert(l->nextToken(is) == Lexer::OPEN_PAREN);
    assert(l->nextToken(is) == Lexer::SYMBOL);
    assert(l->getString() == L"+");
    assert(l->nextToken(is) == Lexer::NUMBER);
    assert(l->getNumber()->toString() == SchemeObject::createRealNumber(1.5)->toString());
    assert(l->nextToken(is) == Lexer::OPEN_PAREN);
    assert(l->nextToken(is) == Lexer::NUMBER);
    assert(l->getNumber()->toString() == SchemeObject::createIntegerNumber(2)->toString());
    assert(l->nextToken(is) == Lexer::PERIOD);
    assert(l->nextToken(is) == Lexer::STRING);
    assert(l->getString() == L"\\aHej\"");
    assert(l->nextToken(is) == Lexer::CLOSE_PAREN);
    assert(l->nextToken(is) == Lexer::SYMBOL);
    assert(l->nextToken(is) == Lexer::CLOSE_PAREN);
    assert(l->nextToken(is) == Lexer::END);
    delete is;
    
    is = new wistringstream(L"#f #tf");
    assert(l->nextToken(is) == Lexer::BOOLEAN);
    assert(l->getBool() == false);
    assert(l->nextToken(is) == Lexer::BOOLEAN);
    assert(l->getBool() == true);
    assert(l->nextToken(is) == Lexer::SYMBOL);
    assert(l->nextToken(is) == Lexer::END);
    delete is;
    
    is = new wistringstream(L"a `b #| comment #| nested comment |# ... |# ");
    assert(l->nextToken(is) == Lexer::SYMBOL);
    assert(l->getString() == L"a");
    assert(l->nextToken(is) == Lexer::BACKQUOTE);
    assert(l->nextToken(is) == Lexer::SYMBOL);
    assert(l->getString() == L"b");
    assert(l->nextToken(is) == Lexer::END);
    delete is;

    delete l;
    
    Scheme* s = new Scheme();
    assert_eval(s, L"'((a)b)", L"((a) b)");
    assert_eval(s, L"'(c(a)b)", L"(c (a) b)");
    assert_eval(s, L"'(2(a)3)", L"(2 (a) 3)");
    assert_eval(s, L"'(a#(b)#(c))", L"(a #(b) #(c))");
    assert_eval(s, L"'(a'b`c,d,@e)", L"(a (quote b) (quasiquote c) (unquote d) (unquote-splicing e))");

    assert_eval(s, L"(let ((1+2 3) (2+3 5)) (+ 1+2 2+3))", L"8");
}


void test_objects() {
    SchemeObject* n = SchemeObject::createRealNumber(1.0);
    assert(n->type() == SchemeObject::REAL_NUMBER);
    assert(n->immutable() == false);
    n->set_immutable(true);
    assert(n->immutable() == true);
    assert(n->type() == SchemeObject::REAL_NUMBER);
    n->set_immutable(false);
    assert(n->immutable() == false);
    assert(n->type() == SchemeObject::REAL_NUMBER);
    assert(sizeof(SchemeObject) == 12);
}

void test_parser() {
    wistream* is = new wistringstream(L"(+ 1.5 (list? \"Hej\"))");
    Parser* p = new Parser();
    SchemeObject* t = p->parse(is);
    SchemeObject* e = s_car(t);
    assert(s_car(e)->type() == SchemeObject::SYMBOL);
    assert(s_cadr(e)->type() == SchemeObject::REAL_NUMBER);
    SchemeObject* inner = s_caddr(e);
    assert(s_car(inner)->type() == SchemeObject::SYMBOL);
    assert(s_car(inner)->toString() == L"list?");
    assert(s_cadr(inner)->type() == SchemeObject::STRING);
    assert(s_cddr(inner)->type() == SchemeObject::EMPTY_LIST);
    assert(s_cdddr(e)->type() == SchemeObject::EMPTY_LIST);
    
    is = new wistringstream(L"'(x . y)");
    t = p->parse(is);
    e = s_car(t);
    assert(s_car(e)->type() == SchemeObject::SYMBOL);
    assert(s_car(e)->toString() == L"quote");
    assert(s_cadr(e)->type() == SchemeObject::PAIR);
    assert(s_caadr(e)->type() == SchemeObject::SYMBOL);
    assert(s_caadr(e)->toString() == L"x");
    assert(s_cdadr(e)->toString() == L"y");

    is = new wistringstream(L"`(a b)");
    t = p->parse(is);
    e = s_car(t);
    assert(s_car(e)->type() == SchemeObject::SYMBOL);
    assert(s_car(e)->toString() == L"quasiquote");
    assert(s_cadr(e)->type() == SchemeObject::PAIR);
    assert(s_caadr(e)->type() == SchemeObject::SYMBOL);
    assert(s_caadr(e)->toString() == L"a");
}

void test_interpreter() {
    Scheme* s = new Scheme();
    assert(s->eval(L"") == S_UNSPECIFIED);
    assert(s->eval(L"#| xxx |#") == S_UNSPECIFIED);
    assert(s->eval(L"#| xxx |# ") == S_UNSPECIFIED);
    assert(s->eval(L";xxxxx ") == S_UNSPECIFIED);
    assert(s->eval(L"#;xxxxx ") == S_UNSPECIFIED);
    assert(s->eval(L"#; xxxxx ") == S_UNSPECIFIED);
    assert_eval(s, L"'(a b #; c d)", L"(a b d)");
    assert_eval(s, L"'(a b (#; a b) d)", L"(a b (b) d)");
    assert_eval(s, L"'(a b #;( a b) d)", L"(a b d)");

    // test eval_combo()
    assert_eval(s, L"((if #t reverse length) '(1 2 3))", L"(3 2 1)");
    assert_eval(s, L"((if #f reverse length) '(1 2 3))", L"3");
    assert_eval(s, L"((if #f reverse length) '(1 2 3))", L"3");
    assert_eval(s, L"((if #f + *) 3 4)", L"12");
    assert_eval(s, L"(if #f 'a)", L"#<unspecified>");
    assert_fail(s, L"(if)");
    assert_fail(s, L"(if #t)");
    
    // test define
    assert_eval(s, L"(define a 10) a", L"10");
    assert_fail(s, L"(define)");
    assert_fail(s, L"(define a)");
    assert_fail(s, L"(define 10 10)");
    assert_fail(s, L"(define a 10 20)");
    assert_fail(s, L"(define (a))");
    assert_fail(s, L"(define () 20)");
    assert_fail(s, L"(define ())");

    // test built-in with only rst args
    assert_eval(s, L"(+ 10 9 2 19 8 2 1 29 8 8 2 1 23 3 1) ", L"126");
    assert_eval(s, L"(+ (+ 1 2 3) (+ 1 2 3)) ", L"12");

    // Test or and and
    assert_eval(s, L"(and (= 2 2) (> 2 1))", L"#t");
    assert_eval(s, L"(and (= 2 2) (< 2 1))", L"#f");
    assert_eval(s, L"(and 1 2 'c '(f g))", L"(f g)");
    assert_eval(s, L"(and)", L"#t");
    assert_eval(s, L"(or)", L"#f");
    assert_eval(s, L"(or (= 2 2) (> 2 1))", L"#t");
    assert_eval(s, L"(or (= 2 2) (< 2 1))", L"#t");
    assert_eval(s, L"(or #f #f #f)", L"#f");
    assert_eval(s, L"(or (member 2 '(1 2 3)) #f)", L"(2 3)");
    assert_eval(s, L"(or (member 'b '(a b c)) #f)", L"(b c)");
    assert_eval(s, L"(not #t)", L"#f");
    assert_eval(s, L"(not 3)", L"#f");
    assert_eval(s, L"(not (list 3))", L"#f");
    assert_eval(s, L"(not #f)", L"#t");
    assert_eval(s, L"(not '())", L"#f");
    assert_eval(s, L"(not (list))", L"#f");
    assert_eval(s, L"(not 'nil)", L"#f");
    assert_fail(s, L"(not)");

    assert_eval(s, L"(apply + (list 3 4))", L"7");
    assert_eval(s, L"(apply + '(1 2 3))", L"6");
    assert_eval(s, L"(apply + 1 2 '(3 4))", L"10");
    assert_eval(s, L"(apply list '())", L"()");
    assert_eval(s, L"(apply * 1 2 (list 3 4))", L"24");
    assert_eval(s, L"(apply apply `(,+ ,(list 1 2)))", L"3");
    s->eval(L"(define compose (lambda (f g) (lambda args (f (apply g args)))))");
    assert_eval(s, L"((compose sqrt *) 12 75)", L"30");  // R^5RS, Section 6.4.
    
    assert_eval(s, L"(case (* 2 3) ((2 3 5 7) 'prime) ((1 4 6 8 9) 'composite))", L"composite");
    assert_eval(s, L"(case (car '(c d)) ((a) 'a) ((b) 'b))", L"#<unspecified>");
    assert_eval(s, L"(case (car '(c d)) ((a e i o u) 'vowel) ((w y) 'semivowel) (else 'consonant))", L"consonant");
    assert_eval(s, L"(case 2)", L"#<unspecified>");
    assert_fail(s, L"(case)");
    assert_fail(s, L"(case 2 (2))");
    assert_fail(s, L"(case 2 2)");
    
    assert_eval(s, L"(cond ((> 3 2) 'greater) ((< 3 2) 'less))", L"greater");
    assert_eval(s, L"(cond ((> 3 3) 'greater) ((< 3 3) 'less) (else 'equal))", L"equal");
    assert_eval(s, L"(cond ((assv 'b '((a 1) (b 2))) => cadr) (else #f))", L"2");
    assert_eval(s, L"(cond ((equal? 'a 'a)) (else 'b))", L"#t");
    assert_eval(s, L"(cond)", L"#<unspecified>");
    assert_eval(s, L"(cond (17))", L"17");
    assert_eval(s, L"(cond ((> 3 2)))", L"#t");
    assert_fail(s, L"(cond a)");
    assert_eval(s, L"(cond ('a 'b))", L"b");
    
    // Brian M. Moore in thread: shadowing syntatic keywords, bug in MIT Scheme?
    // http://groups.google.com/groups?selm=6e6n88%248qf%241%40news.cc.ukans.edu        
    assert_eval(s, L"((lambda lambda lambda) 'x)", L"(x)");
    assert_eval(s, L"(let ((quote -)) (eqv? '1 1))", L"#f");
    assert_eval(s, L"((lambda (begin) (begin 1 2 3)) (lambda lambda lambda))", L"(1 2 3)");    

    delete s;
}

void test_bools() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(boolean? #t)", L"#t");
    assert_eval(s, L"(boolean? #f)", L"#t");
    assert_eval(s, L"(boolean? 1)", L"#f");
    assert_eval(s, L"(boolean? '(1 2 3))", L"#f");
}

void test_char() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(char? #\\a)", L"#t");
    assert_eval(s, L"(char? 1)", L"#f");
    assert_eval(s, L"(char? 'a)", L"#f");
    assert_eval(s, L"#\\b", L"#\\b");
    assert_eval(s, L"(char->integer #\\Space)", L"32");
    assert_eval(s, L"(char->integer #\\newline)", L"10");
    assert_eval(s, L"#\\space", L"#\\space");
    assert_eval(s, L"#\\newline", L"#\\newline");
    assert_eval(s, L"(integer->char 66)", L"#\\B");
    assert_eval(s, L"(integer->char 95)", L"#\\_");
    assert_eval(s, L"(integer->char 32)", L"#\\space");
    assert_eval(s, L"(char->integer (integer->char 5000))", L"5000");
    assert_fail(s, L"(integer->char -1)");
    assert_fail(s, L"(integer->char 'a)");
    assert_fail(s, L"(integer->char)");
    assert_fail(s, L"(integer->char 55297)"); // 0xD801
    assert_fail(s, L"(integer->char 1179648)"); // 0x120000
    assert_eval(s, L"(char->integer #\\B)", L"66");
    assert_eval(s, L"(char->integer #\\_)", L"95");
    assert_eval(s, L"(<= (char->integer #\\a) (char->integer #\\b))", L"#t");
    assert_eval(s, L"(char-upcase #\\a)", L"#\\A");
    assert_eval(s, L"(char-upcase #\\A)", L"#\\A");
    assert_eval(s, L"(char-downcase #\\b)", L"#\\b");
    assert_eval(s, L"(char-downcase #\\B)", L"#\\b");
    assert_eval(s, L"(char-alphabetic? #\\a)", L"#t");
    assert_eval(s, L"(char-alphabetic? #\\space)", L"#f");
    assert_eval(s, L"(char-alphabetic? #\\1)", L"#f");
    assert_eval(s, L"(char-numeric? #\\1)", L"#t");
    assert_eval(s, L"(char-numeric? #\\x)", L"#f");
    assert_eval(s, L"(char-whitespace? #\\ )", L"#t");
    assert_eval(s, L"(char-whitespace? #\\a)", L"#f");
    assert_eval(s, L"(char-whitespace? #\\3)", L"#f");
    assert_eval(s, L"(char-upper-case? #\\F)", L"#t");
    assert_eval(s, L"(char-upper-case? #\\f)", L"#f");
    assert_eval(s, L"(char-upper-case? #\\1)", L"#f");
    assert_eval(s, L"(char-lower-case? #\\F)", L"#f");
    assert_eval(s, L"(char-lower-case? #\\f)", L"#t");
    assert_eval(s, L"(char-lower-case? #\\1)", L"#f");
    assert_eval(s, L"(char=?)", L"#t");
    assert_eval(s, L"(char=? #\\A)", L"#t");
    assert_eval(s, L"(char=? #\\a #\\a #\\a)", L"#t");
    assert_eval(s, L"(char=? #\\a #\\b)", L"#f");
    assert_eval(s, L"(char=? #\\a #\\a)", L"#t");
    assert_eval(s, L"(char=? #\\a #\\a #\\b)", L"#f");
    assert_fail(s, L"(char=? #\\a 'aa #\\b)");
    assert_eval(s, L"(char<? #\\a #\\z)", L"#t");
    assert_eval(s, L"(char<? #\\0 #\\9)", L"#t");
    assert_eval(s, L"(char>? #\\z #\\a)", L"#t");
    assert_eval(s, L"(char>? #\\9 #\\0)", L"#t");
    assert_eval(s, L"(char<? #\\a #\\b #\\d)", L"#t");
    assert_eval(s, L"(char<? #\\a #\\b #\\b)", L"#f");
    assert_eval(s, L"(char>? #\\c #\\b #\\c)", L"#f");
    assert_eval(s, L"(char>? #\\4 #\\3 #\\1)", L"#t");
    assert_eval(s, L"(char>? #\\b #\\a #\\a)", L"#f");
    assert_eval(s, L"(char>? #\\c #\\b #\\a)", L"#t");
    assert_eval(s, L"(char<=? #\\a #\\b #\\d)", L"#t");
    assert_eval(s, L"(char<=? #\\a #\\b #\\b)", L"#t");
    assert_eval(s, L"(char<=? #\\d #\\b #\\c)", L"#f");
    assert_eval(s, L"(char>=? #\\3 #\\3 #\\2)", L"#t");
    assert_eval(s, L"(char>=? #\\c #\\b #\\a)", L"#t");
    assert_eval(s, L"(char>=? #\\a #\\b #\\a)", L"#f");
    assert_eval(s, L"(char-ci=?)", L"#t");
    assert_eval(s, L"(char-ci=? #\\A)", L"#t");
    assert_eval(s, L"(char-ci=? #\\a #\\A #\\a)", L"#t");
    assert_eval(s, L"(char-ci=? #\\a #\\B)", L"#f");
    assert_eval(s, L"(char-ci=? #\\a #\\A #\\b)", L"#f");
    assert_fail(s, L"(char-ci=? #\\a 'aa #\\b)");
    assert_eval(s, L"(char-ci<? #\\a #\\B #\\d)", L"#t");
    assert_eval(s, L"(char-ci<? #\\A #\\B #\\b)", L"#f");
    assert_eval(s, L"(char-ci>? #\\C #\\b #\\C)", L"#f");
    assert_eval(s, L"(char-ci>? #\\4 #\\3 #\\1)", L"#t");
    assert_eval(s, L"(char-ci>? #\\b #\\A #\\a)", L"#f");
    assert_eval(s, L"(char-ci>? #\\C #\\b #\\A)", L"#t");
    assert_eval(s, L"(char-ci<=? #\\A #\\B #\\d)", L"#t");
    assert_eval(s, L"(char-ci<=? #\\A #\\B #\\b)", L"#t");
    assert_eval(s, L"(char-ci<=? #\\D #\\B #\\c)", L"#f");
    assert_eval(s, L"(char-ci>=? #\\3 #\\3 #\\2)", L"#t");
    assert_eval(s, L"(char-ci>=? #\\C #\\b #\\A)", L"#t");
    assert_eval(s, L"(char-ci>=? #\\a #\\b #\\A)", L"#f");
}

void test_symbols() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(symbol? (quote a))", L"#t");
    assert(s->eval(L"(symbol? 'a)") == S_TRUE);
    assert(s->eval(L"(symbol? '1)") == S_FALSE);
    assert(s->eval(L"(symbol? '())") == S_FALSE);
    assert(s->eval(L"(symbol? 1)") == S_FALSE);
    assert(s->eval(L"(symbol? '-1-1)") == S_TRUE);
    assert(s->eval(L"(symbol? '1-1)") == S_TRUE);
    assert(SchemeObject::createSymbol(L"a") == SchemeObject::createSymbol(L"a"));
    assert(SchemeObject::createSymbol(L"a") != SchemeObject::createSymbol(L"b"));
    assert_eval(s, L"(eq? (string->symbol \"f\") (string->symbol \"F\"))", L"#f");
}

void test_math() {
    Scheme* s = new Scheme();
    assert_eval(s, L"-3" , L"-3");
    assert_eval(s, L"-3.0" , L"-3");
    assert_eval(s, L"+3" , L"3");
    assert_eval(s, L"1e1" , L"10");
    assert_eval(s, L"(+ 1 2 3)" , L"6");
    assert_eval(s, L"(+)" , L"0");
    assert_eval(s, L"(- 3)" , L"-3");
    assert_eval(s, L"(- 3 2)" , L"1");
    assert_eval(s, L"(- 3 4 5)" , L"-6");
    assert_fail(s, L"(- 'a)");
    assert_fail(s, L"(-)");
    assert_eval(s, L"(/ 2)" , L"0.5");
    assert_eval(s, L"(/ 10 2)" , L"5");
    assert_eval(s, L"(/ 10 2 2)" , L"2.5");
    assert_fail(s, L"(/ 'a)");
    assert_fail(s, L"(/)");
    assert_eval(s, L"(* 2 3 4)" , L"24");
    assert_eval(s, L"(*)" , L"1");
    assert_fail(s, L"(* 'a 1)");
    assert_eval(s, L"(min 5)" , L"5");
    assert_eval(s, L"(min 3.0 1 2)" , L"1");
    assert_fail(s, L"(min)");
    assert_eval(s, L"(max 5)" , L"5");
    assert_eval(s, L"(max 3.0 1 2)" , L"3");
    assert_fail(s, L"(max)");
    assert_eval(s, L"(expt 3 4)" , L"81");
    assert_eval(s, L"(expt 0 0)" , L"1");
    assert_eval(s, L"(expt 0 3)" , L"0");
    assert_fail(s, L"(expt 0 'a)");
    assert_fail(s, L"(expt 'a 0)");
    assert_fail(s, L"(expt 1)");
    assert_eval(s, L"(< 1 2 3)" , L"#t");
    assert_eval(s, L"(< 1 2 2 3)" , L"#f");
    assert_eval(s, L"(<= 1 2 2 3)" , L"#t");
    assert_eval(s, L"(<= 3 2 2 1)" , L"#f");
    assert_eval(s, L"(> 3 2 1)" , L"#t");
    assert_eval(s, L"(> 3 2 2 1)" , L"#f");
    assert_eval(s, L"(>= 3 2 2 1)" , L"#t");
    assert_eval(s, L"(>= 1 2 2 3)" , L"#f");
    assert_eval(s, L"(= 2 2 2 3)" , L"#f");
    assert_eval(s, L"(= 2 2 2 2)" , L"#t");
    assert_fail(s, L"(= 'a)");
    assert_fail(s, L"(= 1 'a)");
    assert_eval(s, L"(even? 10)" , L"#t");
    assert_eval(s, L"(even? -9)" , L"#f");
    assert_eval(s, L"(even? 0)" , L"#t");
    assert_eval(s, L"(odd? 31137)" , L"#t");
    assert_eval(s, L"(odd? 0)" , L"#f");
    assert_eval(s, L"(odd? -1)" , L"#t");
    assert_fail(s, L"(odd? 'a)");
    assert_eval(s, L"(zero? 0)" , L"#t");
    assert_eval(s, L"(zero? -1)" , L"#f");
    assert_eval(s, L"(negative? 0)" , L"#f");
    assert_eval(s, L"(negative? -10)" , L"#t");
    assert_eval(s, L"(negative? 2)" , L"#f");
    assert_eval(s, L"(positive? 0)" , L"#f");
    assert_eval(s, L"(positive? -10)" , L"#f");
    assert_eval(s, L"(positive? 2)" , L"#t");
    assert_eval(s, L"(integer? 2)" , L"#t");
    assert_eval(s, L"(integer? 2.1)" , L"#f");
    assert_eval(s, L"(integer? 2.0)" , L"#t");
    assert_eval(s, L"(exact? 2.1)" , L"#f");
    assert_eval(s, L"(exact? 2)" , L"#t");
    assert_eval(s, L"(inexact? 2.1)" , L"#t");
    assert_eval(s, L"(inexact? 2)" , L"#f");
    assert_eval(s, L"(complex? 2)" , L"#t");
    assert_eval(s, L"(complex? 'a)" , L"#f");
    assert_eval(s, L"(real? 2)" , L"#t");
    assert_eval(s, L"(real? 'a)" , L"#f");
    assert_eval(s, L"(rational? 2)" , L"#t");
    assert_eval(s, L"(rational? 'a)" , L"#f");

    assert_eval(s, L"(round 2.1)" , L"2");
    assert_eval(s, L"(round 2.8)" , L"3");
    assert_fail(s, L"(round 'a)");
    assert_fail(s, L"(round 2.1 2.3)");
    assert_eval(s, L"(floor -4.3)" , L"-5");
    assert_fail(s, L"(floor 'a)");
    assert_eval(s, L"(ceiling -4.3)" , L"-4");
    assert_eval(s, L"(truncate -4.3)" , L"-4");
    assert_eval(s, L"(round -4.3)" , L"-4");
    assert_eval(s, L"(floor 3.5)" , L"3");
    assert_eval(s, L"(ceiling 3.5)" , L"4");
    assert_eval(s, L"(truncate 3.5)" , L"3");
    assert_eval(s, L"(round 3.5)" , L"4");
    assert_eval(s, L"(round 2.5)" , L"2"); // Round to nearest even integer
    assert_eval(s, L"(round 7)" , L"7");

    assert_eval(s, L"(modulo 13 4)" , L"1");
    assert_eval(s, L"(remainder 13 4)" , L"1");
    assert_eval(s, L"(quotient 13 4)" , L"3");
    assert_eval(s, L"(modulo -13 4)" , L"3");
    assert_eval(s, L"(remainder -13 4)" , L"-1");
    assert_eval(s, L"(quotient -13 4)" , L"-3");
    assert_eval(s, L"(modulo 13 -4)" , L"-3");
    assert_eval(s, L"(remainder 13 -4)" , L"1");
    assert_eval(s, L"(quotient 13 -4)" , L"-3");
    assert_eval(s, L"(modulo -13 -4)" , L"-1");
    assert_eval(s, L"(remainder -13 -4)" , L"-1");    
    assert_eval(s, L"(quotient -13 -4)" , L"3");
    assert_eval(s, L"(gcd)" , L"0");
    assert_eval(s, L"(gcd 5)" , L"5");
    assert_eval(s, L"(gcd -4)" , L"4"); // Guile 1.8.1 gets this one wrong.
    assert_eval(s, L"(gcd -4 0)" , L"4");
    assert_eval(s, L"(gcd 0 4)" , L"4");
    assert_eval(s, L"(gcd 0 -4)" , L"4");
    assert_eval(s, L"(gcd 32 -36)" , L"4");
    assert_eval(s, L"(gcd 32 36 4 4 12)" , L"4");
    assert_fail(s, L"(gcd 'a)");  // Guile 1.8.1 gets this one wrong.
    assert_fail(s, L"(gcd 1.1)");
    assert_eval(s, L"(lcm)" , L"1");
    assert_eval(s, L"(lcm 0 0)" , L"0");
    assert_eval(s, L"(lcm 32 -36)" , L"288");
    assert_eval(s, L"(lcm 10 15 4)" , L"60");
    assert_eval(s, L"(lcm 10 15 -4)" , L"60");
    assert_fail(s, L"(lcm 'a)");
    assert_fail(s, L"(lcm 1.1)");
}

void test_equals() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(equal? 1 1)" , L"#t");
    assert_eval(s, L"(equal? 1 2)" , L"#f");
    assert_eval(s, L"(equal? \"abc\" \"abc\")" , L"#t");
    assert_eval(s, L"(equal? '(1 2 3) '(1 2 3))" , L"#t");
    assert_eval(s, L"(equal? '(1 2 (a  b) 3) '(1 2 (a b) 3))" , L"#t");
    assert_eval(s, L"(equal? '(1 2 (a c) 3) '(1 2 (a b) 3))" , L"#f");

    assert_eval(s, L"(eq? 'a 'a)" , L"#t");
    assert_eval(s, L"(eq? (list 'a) (list 'a))" , L"#f");
    assert_eval(s, L"(eq? '() '())" , L"#t");
    assert_eval(s, L"(eq? car car)" , L"#t");
    assert_eval(s, L"(eq? (cons 1 2) (cons 1 2))" , L"#f");

    assert_eval(s, L"(eq? 'a 'a)" , L"#t");
    assert_eval(s, L"(eq? (list 'a) (list 'a))" , L"#f");
    assert_eval(s, L"(eq? '() '())" , L"#t");
    assert_eval(s, L"(eq? car car)" , L"#t");

    assert_eval(s, L"(equal? #f '())", L"#f");
    assert_eval(s, L"(eqv? #f '())", L"#f");
    assert_eval(s, L"(eq? #f '())", L"#f");
    assert_eval(s, L"(eqv? #\\a #\\a)", L"#t");
    assert_eval(s, L"(eqv? #\\space #\\spAce)", L"#t");
}

void test_pairs_and_lists() {
    SchemeObject* p = s_cons(SchemeObject::createSymbol(L"x"),SchemeObject::createSymbol(L"y"));
    assert(p->toString() == L"(x . y)");
    
    Scheme* s = new Scheme();
    assert_eval(s, L"(list? '())", L"#t");
    assert_eval(s, L"(list? '(1 2 3))", L"#t");
    assert_eval(s, L"(list? 1)", L"#f");
    assert_eval(s, L"(list? '(1 2 . 3))", L"#f");
    
    // From R^5RS 6.3.2. Tests that list? returns #f on circular lists
    assert_eval(s, L"(let ((x (list 'a))) (set-cdr! x x) (list? x))", L"#f");
    assert(s->eval(L"(pair? 1)") == S_FALSE);
    assert(s->eval(L"(pair? '())") == S_FALSE);
    assert(s->eval(L"(pair? '(1 2))") == S_TRUE);
    assert(s->eval(L"(pair? '(1 2 . 3))") == S_TRUE);
    assert(s->eval(L"(pair? '#(a b))") == S_FALSE);    
    assert(s->eval(L"(null? '(1 2 3))") == S_FALSE);
    assert(s->eval(L"(null? '())") == S_TRUE);
    assert(s->eval(L"(null? 1)") == S_FALSE);

    assert_eval(s, L"(cons 1 2)", L"(1 . 2)");
    
    assert_eval(s, L"(list)", L"()");
    assert_eval(s, L"(list 1)", L"(1)");
    assert_eval(s, L"(list '())", L"(())");
    assert_eval(s, L"(list 1 2 (+ 1 2) 4)", L"(1 2 3 4)");

    assert_eval(s, L"(car (cons 1 2))", L"1");
    assert_eval(s, L"(cdr (cons 1 2))", L"2");
    assert_eval(s, L"(cdr (list 1 2))", L"(2)");
    assert_eval(s, L"(cadr '((a b) (c d)))", L"(c d)");
    assert_eval(s, L"(cdar '((a b) (c d)))", L"(b)");
    assert_eval(s, L"(caar '((a b) (c d)))", L"a");
    assert_eval(s, L"(caadr '((a b) (c d)))", L"c");

    assert_eval(s, L"(reverse '(a b c))", L"(c b a)");
    assert_eval(s, L"(reverse '(a (b c) d (e (f))))", L"((e (f)) d (b c) a)");
    assert_eval(s, L"(reverse '())", L"()");
    assert_fail(s, L"(reverse '(a b c d . e))");
    assert_fail(s, L"(reverse)");
    assert_fail(s, L"(reverse 'a)");
    assert_fail(s, L"(reverse 'a 'b)");
    
    assert_eval(s, L"(member 3 '(1 2 3 4 5))", L"(3 4 5)");
    assert_eval(s, L"(member 10 '(1 2 3 4 5))", L"#f");
    assert_eval(s, L"(member 10 '())", L"#f");
    assert_eval(s, L"(member (list 'a) '(b (a) c))", L"((a) c)");
    assert_eval(s, L"(memq (list 'a) '(b (a) c))", L"#f");
    
    assert_eval(s, L"(list-tail '(1 2 3 4 5) 0)", L"(1 2 3 4 5)");
    assert_eval(s, L"(list-tail '(1 2 3 4 5) 1)", L"(2 3 4 5)");
    assert_eval(s, L"(list-tail '() 0)", L"()");

    assert_eval(s, L"(list-ref '(1 2 3) 0)", L"1");
    assert_eval(s, L"(list-ref '(1 2 3) 1)", L"2");
    assert_eval(s, L"(list-ref '(1 2 3) 2)", L"3");
    
    assert_eval(s, L"(append '() '(a b c) '(a b) '())", L"(a b c a b)");
    assert_eval(s, L"(append)", L"()");
    assert_eval(s, L"(append '() 'a)", L"a");
    assert_eval(s, L"(append '() '() 'b)", L"b");
    assert_eval(s, L"(append '() '() '())", L"()");
    assert_eval(s, L"(append 'a)", L"a");
    assert_eval(s, L"(append '(a b c) 'e)", L"(a b c . e)");
    assert_eval(s, L"(append '(a b c) '(1 . 2))", L"(a b c 1 . 2)");
    assert_eval(s, L"(append '(a (b)) '((c)))", L"(a (b) (c))");
    assert_fail(s, L"(append '(a . b) '(a b c))");
    assert_fail(s, L"(append 'a '(a b c))");

    s->eval(L"(define e '((a 1) (b 2) (c 3)))");
    assert_eval(s, L"(assq 'a e)", L"(a 1)");
    assert_eval(s, L"(assq 'b e)", L"(b 2)");
    assert_eval(s, L"(assq 'd e)", L"#f");
    assert_eval(s, L"(assoc (list 'a) '(((a)) ((b)) ((c))))", L"((a))");
    assert_eval(s, L"(assq (list 'a) '(((a)) ((b)) ((c))))", L"#f");
    assert_eval(s, L"(assv 5 '((2 3) (5 7) (11 13)))", L"(5 7)");
    
    s->eval(L"(define e (list 'a 'b 'c 'd))");
    assert_eval(s, L"(set-car! e 'f) e", L"(f b c d)");
    assert_eval(s, L"(set-cdr! e 'g) e", L"(f . g)");
    assert_eval(s, L"(set-cdr! e '()) e", L"(f)");
    s->eval(L"(define (f) (list 'not-a-constant-list))");
    s->eval(L"(define (g) '(constant-list a b c))");
    assert_eval(s, L"(set-car! (f) 3)", L"#<unspecified>");
    assert_fail(s, L"(set-car! (g) 3)");
    delete s;
}

void test_lambda() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(procedure? (lambda (x) x))", L"#t");
    assert_eval(s, L"(procedure? cons)", L"#t");
    assert_eval(s, L"(procedure? if)", L"#t");
    assert_eval(s, L"(procedure? 1)", L"#f");
    assert_eval(s, L"(procedure? 'car)", L"#f");
    assert_eval(s, L"(procedure? (lambda (x) (* x x)))", L"#t");
    assert_eval(s, L"(procedure? '(lambda (x) (* x x)))", L"#f");
    assert_eval(s, L"((lambda () 3))", L"3");
    assert_eval(s, L"((lambda (x) (* 2 x)) 10)", L"20");
    assert_eval(s, L"((lambda (x y) (+  y x)) 7 10)", L"17");
    assert_eval(s, L"((lambda (x y z) (list z y x)) 3 4 5)", L"(5 4 3)");
    // Some examples from R^5RS
    assert_eval(s, L"((lambda x x) 3 4 5 6)", L"(3 4 5 6)");
    assert_eval(s, L"((lambda (x y . z) z) 3 4 5 6)", L"(5 6)");
    assert_eval(s, L"((lambda (x) (+ x x)) 4)", L"8");
    assert_eval(s, L"(define add4 (let ((x 4)) (lambda (y) (+ x y)))) (add4 6)", L"10");
    
    assert_eval(s, L"(let () (define (f . x) x) (f))", L"()");
    assert_eval(s, L"(let () (define (f . x) x) (f 1 2 3 4 5 6 7))", L"(1 2 3 4 5 6 7)");
    assert_eval(s, L"(let () (define f (lambda x x)) (f))", L"()");
    assert_fail(s, L"((lambda))");
    // Duplicate formals
    assert_fail(s, L"((lambda (x y y z) x) 10 10 10 10)");  
}

void test_macros() {
    Scheme* s = new Scheme();
    s->eval(L"(define-macro (greater-than x y) `(> ,x ,y))");
    assert_eval(s, L"(greater-than 10 20)", L"#f");
    assert_eval(s, L"(greater-than 20 10)", L"#t");

    s->eval(L"(define-macro (when test . consequent) `(if ,test (begin ,@consequent)))");
    assert_eval(s, L"(when #t 'kaj)", L"kaj");
}

void test_define_and_set() {
    Scheme* s = new Scheme();
    s->eval(L"(define x 17)");
    assert_eval(s, L"x", L"17");
    s->eval(L"(set! x 20)");
    assert_eval(s, L"x", L"20");
    s->eval(L"(let () (set! x 50))");
    assert_eval(s, L"x", L"50");
    s->eval(L"(define (square x) (* x x))");
    assert_eval(s, L"(square 9)", L"81");
    assert_fail(s, L"(set! x 50 5)"); // Extra args

    // A R^5RS spec that guile 1.6.8 fails but we don't... Fixed in guile 1.8.1
    s->eval(L"(define (selftest . x) x)");
    assert_eval(s, L"(selftest 1 2 3 4)", L"(1 2 3 4)");

    s->eval(L"(define (fact n) (if (equal? n 1) 1 (* n (fact (- n 1)))))");
    assert_eval(s, L"(fact 6)", L"720");

    // Duplicate formals
    assert_fail(s, L"(define (duplicate-formals x y y z) x)");
    assert_fail(s, L"(define (duplicate-formals x y z . y) x)");
}

void test_string() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(string? 1)", L"#f");
    assert_eval(s, L"(string? \"\")", L"#t");
    assert_eval(s, L"(string? \"a\")", L"#t");
    assert_eval(s, L"(string? ((lambda () \"a\")))", L"#t");
    assert_eval(s, L"(make-string 3)", L"\"   \"");
    assert_eval(s, L"(make-string 5 #\\z)", L"\"zzzzz\"");
    assert_eval(s, L"(string #\\z #\\x #\\\" #\\a)", L"\"zx\\\"a\"");
    assert_eval(s, L"(string-length \"abcdef\")", L"6");
    assert_eval(s, L"(string-length \"\")", L"0");
    assert_eval(s, L"(string-length (make-string 200))", L"200");
    assert_eval(s, L"(string-ref \"scheme\" 0)", L"#\\s");
    assert_eval(s, L"(string-ref \"scheme\" 2)", L"#\\h");
    assert_eval(s, L"(symbol->string 'aaa)", L"\"aaa\"");
    assert_eval(s, L"(string->symbol \"aaa\")", L"aaa");
    assert_eval(s, L"(eq? (string->symbol \"f\") (string->symbol \"F\"))", L"#f");
    assert_eval(s, L"(string-append)", L"\"\"");
    assert_eval(s, L"(string-append \"zzz\")", L"\"zzz\"");
    assert_eval(s, L"(string-append \"zzz\" \"xxx\") ", L"\"zzzxxx\"");
    assert_eval(s, L"(string-copy \"zzz\")", L"\"zzz\"");
    assert_eval(s, L"(define z \"zzz\") (define x (string-copy z)) (string-set! x 0 #\\x) z", L"\"zzz\"");
    assert_eval(s, L"(string->number \"100\")", L"100");
    assert_eval(s, L"(string->number \"2.5\")", L"2.5");
    assert_eval(s, L"(string->number \"100\" 8)", L"64");
    assert_eval(s, L"(string->number \"\")", L"#f");
    assert_eval(s, L"(string->number \"+\")", L"#f");
    assert_eval(s, L"(string->number \"-\")", L"#f");
    assert_eval(s, L"(string->number \"d\")", L"#f");
    assert_eval(s, L"(string->number \"i\")", L"#f");
    assert_eval(s, L"(string->number \"I\")", L"#f");
    assert_eval(s, L"(string->number \"3i\")", L"#f");
    assert_eval(s, L"(string->number \"3.3i\")", L"#f");
    assert_eval(s, L"(string->number \".\")", L"#f");
    assert_eval(s, L"(string->number \"d\")", L"#f");
    assert_eval(s, L"(string->number \"#x10\")", L"16");
    assert_eval(s, L"(string->number \"#x100\")", L"256");
    assert_eval(s, L"(string->number \"#xff\")", L"255");
    assert_eval(s, L"(string->number \"#xFF\")", L"255");
    assert_eval(s, L"(string->number \"#b1000\")", L"8");
    assert_eval(s, L"(string->number \"#b11111111\")", L"255");
    assert_eval(s, L"(string->number \"#o123\")", L"83");
    assert_eval(s, L"(string->number \"#o1000\")", L"512");
    assert_eval(s, L"(string->number \"20e3\")", L"20000");
    assert_eval(s, L"(string->number \"42e-2\")", L"0.42");
    assert_eval(s, L"(string->number \".2e2\")", L"20");
    assert_eval(s, L"(string->number \"8e+3\")", L"8000");
    assert_eval(s, L"(string->number \"8f3\")", L"8000");
    assert_fail(s, L"(string->number \"#xffffffffffffffffffffff\")");
    assert_eval(s, L"(string->number \"#xff.10\")", L"#f");
    assert_eval(s, L"(string->number \"#xffs10\")", L"#f");
    assert_eval(s, L"(number->string 256)", L"\"256\"");
    assert_eval(s, L"(number->string 256 16)", L"\"100\"");
    assert_eval(s, L"(string->list \"\")", L"()");
    assert_eval(s, L"(string->list \"String\")", L"(#\\S #\\t #\\r #\\i #\\n #\\g)");
    assert_eval(s, L"(list->string '())", L"\"\"");
    assert_eval(s, L"(list->string '(#\\S #\\t #\\r #\\i #\\n #\\g))", L"\"String\"");
    assert_eval(s, L"(define ss (string #\\S #\\t #\\r #\\i #\\n #\\g)) ss", L"\"String\"");
    assert_eval(s, L"(string-set! ss 3 #\\u) ss", L"\"Strung\"");
    assert_fail(s, L"(string-set! ss 10 #\\u) ss");
    assert_fail(s, L"(define (g) \"***\") (string-set! (g) 0 #\?)");
    assert_fail(s, L"(string-set! (symbol->string 'immutable) 0 #\?)");
    assert_eval(s, L"(substring \"ab\" 0 0)", L"\"\"");
    assert_eval(s, L"(substring \"ab\" 1 1)", L"\"\"");
    assert_eval(s, L"(substring \"ab\" 2 2)", L"\"\"");
    assert_eval(s, L"(substring \"ab\" 1 2)", L"\"b\"");
    assert_eval(s, L"(substring \"ab\" 0 1)", L"\"a\"");
    assert_eval(s, L"(substring \"ab\" 0 2)", L"\"ab\"");
    assert_fail(s, L"(substring \"abcdef\" 3 1)");
    assert_eval(s, L"(string=?  \"abcdef\"  \"abcdef\"  \"abcdef\")", L"#t");
    assert_eval(s, L"(string<?  \"abcdef\"  \"abcdeg\"  \"abddef\")", L"#t");
    assert_eval(s, L"(string>? \"z\" \"a\")", L"#t");
    assert_eval(s, L"(string<? \"z\" \"a\")", L"#f");
    assert_eval(s, L"(string>? \"9\" \"0\")", L"#t");
    assert_eval(s, L"(string>? \"aab\" \"aa\")", L"#t");
    assert_eval(s, L"(string>? \"aaa\" \"aa\" \"a\")", L"#t");
    assert_eval(s, L"(string>? \"aaa\" \"a\" \"aa\")", L"#f");
    assert_eval(s, L"(string<? \"a\" \"aa\" \"aaa\")", L"#t");
    assert_eval(s, L"(string<? \"a\" \"aaa\" \"aa\")", L"#f");
    assert_eval(s, L"(string>? \"a\" \"az\")", L"#f");
    assert_eval(s, L"(string=? \"9\" \"0\")", L"#f");
    assert_eval(s, L"(string=? \"9\" \"9\")", L"#t");
    assert_eval(s, L"(string<? \"9\" \"0\")", L"#f");
    assert_eval(s, L"(string<? \"0\" \"9\")", L"#t");
    assert_eval(s, L"(string<? \"1\" \"5\" \"9\")", L"#t");
    assert_eval(s, L"(string<? \"a\" \"z\")", L"#t");
    assert_eval(s, L"(string>? \"z\" \"a\")", L"#t");
    assert_eval(s, L"(string>? \"zz\" \"aa\")", L"#t");
    assert_eval(s, L"(string>? \"c\" \"b\" \"a\")", L"#t");
    assert_eval(s, L"(string<?  \"abcdef\"  \"abcdef\"  \"abddef\")", L"#f");
    assert_eval(s, L"(string<=? \"abcdef\"  \"abcdef\"  \"abddef\")", L"#t");
    assert_eval(s, L"(string>?  \"abcdef\"  \"abcdee\"  \"abcded\")", L"#t");
    assert_eval(s, L"(string>?  \"abcdef\"  \"abcdef\"  \"abcded\")", L"#f");
    assert_eval(s, L"(string>=? \"abcdef\"  \"abcdef\"  \"abcded\")", L"#t");
    assert_eval(s, L"(string>=? \"abcdef\"  \"abcdef\"  \"abcdeg\")", L"#f");
    assert_eval(s, L"(string>=? \"abcdef\"  \"abcdee\"  \"abcded\")", L"#t");
    assert_fail(s, L"(string>=? \"abcdef\"  \"abcdee\" 'a)");
    assert_eval(s, L"(string-ci=?  \"abcDeF\"  \"abCDef\"  \"ABcdef\")", L"#t");
    assert_eval(s, L"(string-ci<?  \"AbCDeF\"  \"abCDEg\"  \"abDDef\")", L"#t");
    assert_eval(s, L"(string-ci<?  \"ABCdef\"  \"abcDEF\"  \"abDDef\")", L"#f");
    assert_eval(s, L"(string-ci<=? \"abcdef\"  \"abcdef\"  \"abddef\")", L"#t");
    assert_eval(s, L"(string-ci>?  \"abcDEF\"  \"abcdEE\"  \"ABCded\")", L"#t");
    assert_eval(s, L"(string-ci>?  \"abcdEF\"  \"abcDEF\"  \"Abcded\")", L"#f");
    assert_eval(s, L"(string-ci>=? \"aBcdEF\"  \"abcDEF\"  \"aBcded\")", L"#t");
    assert_eval(s, L"(string-ci>=? \"aBcdeF\"  \"abcdEF\"  \"abCdeg\")", L"#f");
    assert_eval(s, L"(string-ci>=? \"abCDef\"  \"abcdEE\"  \"abCded\")", L"#t");
    assert_fail(s, L"(string-ci>=? \"abcdeF\"  \"Abcdee\" 'a)");
}

void test_begin() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(begin 1)", L"1");
    assert_eval(s, L"(begin 1 2)", L"2");
    assert_eval(s, L"(begin 1 2 3)", L"3");
    assert_eval(s, L"(begin)", L"#<unspecified>");
}

void test_let() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(let () 'a 'b)", L"b");
    assert_eval(s, L"(let ((i 10)) i)", L"10");
    assert_eval(s, L"(let ((i 10)(j 20)) (* j i))", L"200");
    assert_eval(s, L"(let ((x 2) (y 3)) (let* ((x 7) (z (+ x y))) (* z x)))", L"70");
    assert_eval(s, L"(let ((x 0)) (let ((x 1) (y (* x 1))) y))", L"0");
    assert_eval(s, L"(let loop ((i 10) (j 100)) (if (= i 20) j (loop (+ 1 i) (- j 1))))", L"90");
    assert_eval(s, L"(letrec ((even? (lambda (n) (if (zero? n) #t (odd? (- n 1))))) (odd? (lambda (n) (if (zero? n) #f (even? (- n 1)))))) (even? 88))", L"#t");

    assert_fail(s, L"(let)");
    assert_fail(s, L"(let 'a)");
    assert_fail(s, L"(let 'a 'b)");
    assert_fail(s, L"(let loop 'b)");
    assert_fail(s, L"(let loop 'b 'c)");
    assert_fail(s, L"(let*)");
    assert_fail(s, L"(let* 'a)");
    assert_fail(s, L"(let* 'a 'b)");
    assert_fail(s, L"(letrec)");
    assert_fail(s, L"(letrec 'a)");
    assert_fail(s, L"(letrec 'a 'b)");


    // From http://sisc-scheme.org/r5rs_pitfall.scm
    assert_eval(s, L"(let ((ls (list 1 2 3 4))) (append ls ls '(5)))", L"(1 2 3 4 1 2 3 4 5)");
    assert_eval(s, L"(let ((f -)) (let f ((n (f 1))) n))", L"-1");
    assert_eval(s, L"(let ((f -)) ((letrec ((f (lambda (n) n))) f) (f 1)))", L"-1");

    assert_eval(s, L"(let - ((n (- 1))) n)", L"-1");

    assert_eval(s, L"(let loop ((i 10)) i)", L"10");
    assert_eval(s, L"(let loop ((i 10)(j '())) (if (= 0 i) j (loop (- i 1) (cons i j))))", L"(1 2 3 4 5 6 7 8 9 10)");
}

void test_do() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(do () (#t 'b))", L"b");
    assert_eval(s, L"(do () (#t))", L"#<unspecified>");
    assert_fail(s, L"(do)");
    assert_fail(s, L"(do 'a)");
    assert_fail(s, L"(do () ())");
    assert_fail(s, L"(do (a) (#t))");
    assert_fail(s, L"(do ((a 1) . (b 1)) (#t))");

    // From R^5RS 4.2.4 
    assert_eval(s, L"(do ((vec (make-vector 5)) (i 0 (+ i 1))) ((= i 5) vec) (vector-set! vec i i))", L"#(0 1 2 3 4)");
    assert_eval(s, L"(let ((x '(1 3 5 7 9))) (do ((x x (cdr x)) (sum 0 (+ sum (car x)))) ((null? x) sum)))", L"25");
}

void test_quote() {
    Scheme* s = new Scheme();
    assert_eval(s, L"'()", L"()");
    assert_eval(s, L"'(a b c)", L"(a b c)");
    assert_eval(s, L"'a", L"a");
    assert_eval(s, L"'1", L"1");
    assert_eval(s, L"(number? 1)", L"#t");
    assert_eval(s, L"(boolean? '#t)", L"#t");

    assert_eval(s, L"`a", L"a");
    assert_eval(s, L"`()", L"()");
    assert_eval(s, L"`1", L"1");
    assert_eval(s, L"`(a b c)", L"(a b c)");
    assert_eval(s, L"`(a b . c)", L"(a b . c)");
    assert_eval(s, L"`(a (+ 1 2) c)", L"(a (+ 1 2) c)");
    assert_eval(s, L"`(a b . ,(+ 1 2))", L"(a b . 3)");
    assert_eval(s, L"`(a ,(+ 1 2) c)", L"(a 3 c)");
    assert_eval(s, L"`(a ,1)", L"(a 1)");
    assert_eval(s, L"`(a `(b `c))", L"(a (quasiquote (b (quasiquote c))))");
    assert_eval(s, L"`,(+ 2 3)", L"5");
    assert_eval(s, L"(quasiquote (unquote (+ 2 3)))", L"5");
    assert_eval(s, L"`(a `(b ,(+ 1 2)))", L"(a (quasiquote (b (unquote (+ 1 2)))))");
    assert_eval(s, L"`(a ,(list 1 2 ) c)", L"(a (1 2) c)");
    assert_eval(s, L"`(a ,@(list 1 2 ) c)", L"(a 1 2 c)");
    assert_eval(s, L"(define (sqt x) (do ((i 1 (+ i 1))) ((> (* i i) x) (- i 1))))", L"#<unspecified>");
    assert_eval(s, L"`#(10 5 ,(sqt 4) ,@(map sqt '(16 9)) 8)", L"#(10 5 2 4 3 8)");
    
    // From R^5RS 4.2.6
    assert_eval(s, L"`(list ,(+ 1 2) 4)", L"(list 3 4)");
    assert_eval(s, L"(let ((name 'a)) `(list ,name ',name))", L"(list a (quote a))");
    assert_eval(s, L"`(a ,(+ 1 2) ,@(map abs '(4 -5 6)) b)", L"(a 3 4 5 6 b)");
    assert_eval(s, L"`(( foo ,(- 10 3)) ,@(cdr '(c)) . ,(car '(cons)))", L"((foo 7) . cons)");
    assert_eval(s, L"`#(10 5 ,(sqrt 4) ,@(map sqrt '(16 9)) 8)", L"#(10 5 2 4 3 8)");
    assert_eval(s, L"`(a `(b ,(+ 1 2) ,(foo ,(+ 1 3) d) e) f)", L"(a (quasiquote (b (unquote (+ 1 2)) (unquote (foo 4 d)) e)) f)");
    assert_eval(s, L"(let ((name1 'x) (name2 'y)) `(a `(b ,,name1 ,',name2 d) e))", L"(a (quasiquote (b (unquote x) (unquote (quote y)) d)) e)");
    assert_eval(s, L"(quasiquote (list (unquote (+ 1 2)) 4))", L"(list 3 4)");
    assert_eval(s, L"'(quasiquote (list (unquote (+ 1 2)) 4))", L"(quasiquote (list (unquote (+ 1 2)) 4))");
}

void test_map() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(map + '(1 2 3) '(10 20 30))", L"(11 22 33)");
    assert_eval(s, L"(map car '((a b) (d e) (g h)))", L"(a d g)");
    assert_eval(s, L"(map car (list (list 'a 'b) (list 1 2) (list 'c 'd)))", L"(a 1 c)");
    assert_eval(s, L"(map (lambda (n) (expt n n)) '(1 2 3 4 5))", L"(1 4 27 256 3125)");
    assert_eval(s, L"(let ((count 0)) (map (lambda (ignored) (set! count (+ count 1)) count) '(a b)))", L"(1 2)");
    assert_eval(s, L"(map cadr '())", L"()");
    assert_eval(s, L"(for-each car '((a b) (d e) (g h)))", L"#<unspecified>");
    assert_fail(s, L"(map)");
    assert_fail(s, L"(map +)");
    assert_fail(s, L"(map 'a '(1 2 3) '(1 2 3))");
    assert_fail(s, L"(map + '(1 2 3) '(1 2))");
    assert_fail(s, L"(map + '(1 2) '(1 2 3))");
    assert_fail(s, L"(map + '(1 2) 'a)");
    assert_fail(s, L"(map + 'a '(1 2))");
}

struct test_vector : public Test {
    void run() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(make-vector 3)", L"#(#<unspecified> #<unspecified> #<unspecified>)");
    assert_eval(s, L"(make-vector 5 'a)", L"#(a a a a a)");
    assert_eval(s, L"(make-vector 2 (+ 5 1))", L"#(6 6)");
    assert_eval(s, L"(make-vector 0 'a)", L"#()");
    assert_fail(s, L"(make-vector -2 'a)");
    assert_eval(s, L"(vector? (make-vector 5 'a))", L"#t");
    assert_eval(s, L"(vector-length (make-vector 7))", L"7");
    assert_eval(s, L"(vector? 5)", L"#f");
    assert_eval(s, L"(vector 5 'a (+ 1 2) \"z\")", L"#(5 a 3 \"z\")");
    assert_eval(s, L"(vector)", L"#()");
    assert_eval(s, L"'#(a b c)", L"#(a b c)");
    assert_eval(s, L"(vector? (vector))", L"#t");
    assert_eval(s, L"(vector-length (vector))", L"0");
    assert_eval(s, L"(vector-length (vector 'a 'b))", L"2");
    assert_eval(s, L"(vector-length #())", L"0");
    assert_eval(s, L"(vector-length #(1 (1 2) 3))", L"3");
    assert_fail(s, L"(vector-length 'a)");
    assert_eval(s, L"(list->vector '(a b c))", L"#(a b c)");
    assert_eval(s, L"(list->vector '())", L"#()");
    assert_fail(s, L"(list->vector 1)");
    assert_eval(s, L"(vector->list #(a b c))", L"(a b c)");
    assert_eval(s, L"(vector-ref #(a b c) 0)", L"a");
    assert_eval(s, L"(vector-ref #(a b c) 1)", L"b");
    assert_eval(s, L"(vector-ref #(a b c) 2)", L"c");
    assert_fail(s, L"(vector-ref #(a b c) 3)");
    assert_fail(s, L"(vector-ref #(a b c) -1)");
    assert_fail(s, L"(vector-set! '#(0 1 2) 1 \"doe\")");   // Modifying immutable vector
    assert_eval(s, L"(define v (vector 'a 'b 'c))(vector-set! v 1 '(1 2 3))v", L"#(a (1 2 3) c)");
    assert_eval(s, L"(define z (make-vector 4))(vector-fill! z 'a)z", L"#(a a a a)");
    assert_fail(s, L"(vector-set! (make-vector 5) -1 'a)");
    assert_fail(s, L"(vector-set! (make-vector 5) 5 'a)");
    delete s;
}
};

void test_io() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(input-port? (current-input-port))", L"#t");
    assert_eval(s, L"(output-port? (current-input-port))", L"#f");
    assert_eval(s, L"(input-port? (current-output-port))", L"#f");
    assert_eval(s, L"(output-port? (current-output-port))", L"#t");
}

void test_call_cc() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(call-with-current-continuation procedure?)", L"#t");
    assert_eval(s, L"(call/cc (lambda (exit) (for-each (lambda (x) (if (negative? x) (exit x))) '(54 0 37 -3 245 19)) #t))", L"-3");
    assert_eval(s, L"(call-with-values (lambda () (values 4 5)) (lambda (a b) b))", L"5");
    assert_eval(s, L"(call-with-values * -)", L"-1");
}

void test_eval() {
    Scheme* s = new Scheme();
    assert_eval(s, L"(eval '(* 7 3) (scheme-report-environment 5))", L"21");
    assert_eval(s, L"(eval '(if #t 1 2) (null-environment 5))", L"1");
    assert_eval(s, L"(let ((f (eval '(lambda (f x) (f x x)) (null-environment 5)))) (f + 10))", L"20");
}

// Throw insane forms at the interpreter. It shouldn't crash but report errors back.
void test_error_handling() {
//    wchar_t* procs[] = {L"let", L"let*", L"do", L"if", L"and", L"for-each", L"+", L"list"};        
//    char* args[] = {"1", L"#f", L"()", L"((x 10))", L"(())", L"(() x)", L"((#f)x)", L"(x 1)", L"(x 1 20)", L"1 2 3"};
}

void test_garbagecollect() {
    Scheme* s = new Scheme();
    s->eval(L"(define a (make-vector 100000 'a))");
    s->eval(L"(define b (make-vector 100000 'b))");
    s->eval(L"(define c (make-vector 100000 'a))");
    s->forceGarbageCollection();
    assert_eval(s, L"(vector-length a)", L"100000");
    assert_eval(s, L"(vector-length b)", L"100000");
    assert_eval(s, L"(vector-length c)", L"100000");
    s->eval(L"(define b #f)");
    s->forceGarbageCollection();
    assert_eval(s, L"(equal? a c)", L"#t");
}

class test_bigint : public Test {
    public:
	void run() {

    // Constructor
    assertTrue(BigInt(123456789) == BigInt("123456789"));

    // Copy constructor   
    BigInt b1 = BigInt("10");
    BigInt b2 = b1;
    b1 += 10;
    assertTrue(b2 == BigInt("10"));    
    assertTrue(b1 == BigInt("20"));    
    assertTrue(BigInt(0) * 10 == BigInt(0));

    // Constructor radix test
    assertTrue(BigInt("ff",16) == BigInt("255"));
    assertTrue(BigInt("10001",2) == BigInt("17"));
    
    // toString()
    assertTrue(BigInt(1000).toString() == "1000");
    assertTrue(BigInt(65535).toString(16) == "ffff");
    assertTrue(BigInt(-1000).toString() == "-1000");
    assertTrue(BigInt("123456789123456789").toString() == "123456789123456789");
    
    // Zero handling
    assertTrue(BigInt("0") == BigInt("-0"));
    assertTrue(BigInt("0").is_zero());
    assertTrue(BigInt("-0").is_zero());
    
    // Negatives
    assertTrue(BigInt(-100) == BigInt("-100"));
    assertTrue(-BigInt(100) == BigInt(-100));
    assertTrue(BigInt(-1000) + BigInt(1000) == BigInt(0));
    assertTrue(BigInt("-1000") + BigInt("1000") == BigInt("0"));

    // Subtraction
    assertTrue(BigInt(100) - 20 == BigInt(80));
    assertTrue(BigInt(100) - BigInt(3) == BigInt(97));
    assertTrue(BigInt("3333333333333333333") - BigInt("2222222222222222222") == BigInt("1111111111111111111"));
    
    // Addition
    assertTrue((BigInt(123456789) + 123456789) + 123456789 == BigInt("370370367"));
    assertTrue(BigInt("999999999999999999999999999999999") + BigInt("999999999999999999999999999999999") == BigInt("1999999999999999999999999999999998"));
    assertTrue(BigInt("-111111111111111111").abs() == BigInt("111111111111111111"));

    // Multiply
    assertTrue(BigInt("1000") * 10 == BigInt("10000"));
    assertTrue(BigInt("123456789123456789") * BigInt("123456789123456789") == BigInt("15241578780673678515622620750190521"));
    assertTrue((BigInt("123456789") * 123456789) * 123456789 == BigInt("1881676371789154860897069"));
    
    // expt
    assertTrue(BigInt("100").expt(2) == BigInt("10000"));
    assertTrue(BigInt("2").expt(0) == BigInt(1));
    assertTrue(BigInt(0).expt(100) == BigInt(0));
    assertTrue(BigInt(31).expt(19) == BigInt("21670662219970396194714277471"));
    assertTrue(BigInt(17).expt(1000) * BigInt(17).expt(500) == BigInt(17).expt(1500));
    assertTrue(BigInt(31).expt(1000) * BigInt(31).expt(1500) == BigInt(31).expt(2500));
    
    
    // Division
    /*
    assertTrue(BigInt("9999999999999999999") / 3 == BigInt("3333333333333333333"));
    assertTrue(BigInt(-1000) / 10 == BigInt(-100));
    assertTrue(BigInt(1000) / (-10) == BigInt(-100));
    assertTrue(BigInt(-1000) / (-10) == BigInt(100));
    //cout << "Result " << (BigInt(100) / BigInt(2)) << endl;
    assertTrue(BigInt(100) / BigInt(2) == BigInt(50));
    assertTrue(BigInt("123456789123456789") / BigInt("123456789123456789") == BigInt(1));
    assertTrue(BigInt("10000000000") / BigInt("1000000000") == BigInt(10));
    assertTrue(BigInt("10000000000") / BigInt("10000000") == BigInt(1000));
    assertTrue(BigInt("993850124034") / BigInt("1209237") == BigInt("821882"));
    assertTrue(BigInt("993850124034") / BigInt("821882") == BigInt("1209237"));
    //assertTrue(BigInt("") / BigInt("") == BigInt(""));
    
    assertTrue(BigInt("123456789123456789") / BigInt(1) == BigInt("123456789123456789"));
    assertTrue(BigInt("10000000000") / BigInt("10") == BigInt("1000000000"));
    */
    //assertTrue(BigInt("15241578780673678515622620750190521") / BigInt("123456789123456789") == BigInt("123456789123456789"));
    
    // Remainder
    assertTrue(BigInt(100) % 10 == 0);
    assertTrue(BigInt("10000000000000000000000") % 3 == 1);
    assertTrue(BigInt("-99999999999999999992") % 3 == -2);

    // Square root
    //assertTrue(BigInt(100).sqrt() == BigInt(10));
    //assertTrue(BigInt(10000).sqrt() == BigInt(100));
    //assertTrue(BigInt("10000000000000000").sqrt() == BigInt("100000000"));
    //assertTrue(BigInt("15241578780673678515622620750190521").sqrt() == BigInt("123456789123456789"));

    // Comparators
    assertTrue(BigInt(50) > BigInt(25));
    assertTrue(BigInt("999999999999999999") > 
           BigInt("888888888888888888"));
    assertTrue(BigInt("111111111111111111") < 
           BigInt("222222222222222222"));
    assertTrue(BigInt("999999999999999999999999999999999") <= 
           BigInt("999999999999999999999999999999999"));
    assertTrue(BigInt("8888888888888888888") <= 
           BigInt("9999999999999999999"));
    assertTrue(BigInt("3333333333333333333") >= 
           BigInt("3333333333333333333"));
    assertTrue(BigInt("3333333333333333333") >= 
           BigInt("2222222222222222222"));
           
    // Bitsizes
    assertTrue(BigInt("1000",2).sizeInBits() == 4);
    assertTrue(BigInt("ffffffffffffffffffff",16).sizeInBits() == 80);
	}
};

int main(int argc, char *argv[]) {
    TestSuite suite;
    suite.add("BigInt", new test_bigint());
    suite.add("Vector", new test_vector());
    suite.run();
    suite.printStatus();

    try {
        cout << "Test tokenizer...       ";
        test_tokenizer();
        cout << " OK" << endl;

        cout << "Test objects...         ";
        test_objects();
        cout << " OK" << endl;
        
        cout << "Test parser...          ";
        test_parser();
        cout << " OK" << endl;
        
        cout << "Test interpreter...     ";
        test_interpreter();
        cout << " OK" << endl;

        cout << "Test bools...           ";
        test_bools();
        cout << " OK" << endl;
        
        cout << "Test symbols...         ";
        test_symbols();
        cout << " OK" << endl;

        cout << "Test equals...          ";
        test_equals();
        cout << " OK" << endl;

        cout << "Test math  ...          ";
        test_math();
        cout << " OK" << endl;
        
        cout << "Test pairs and lists... ";
        test_pairs_and_lists();
        cout << " OK" << endl;

        cout << "Test quote...           ";
        test_quote();
        cout << " OK" << endl;

        cout << "Test lambda...          ";
        test_lambda();
        cout << " OK" << endl;

        cout << "Test macros...          ";
        test_macros();
        cout << " OK" << endl;

        cout << "Test let...             ";
        test_let();
        cout << " OK" << endl;

        cout << "Test do...              ";
        test_do();
        cout << " OK" << endl;

        cout << "Test char...            ";
        test_char();
        cout << " OK" << endl;

        cout << "Test string...          ";
        test_string();
        cout << " OK" << endl;

        cout << "Test map...             ";
        test_map();
        cout << " OK" << endl;

        cout << "Test call/cc...         ";
        test_call_cc();
        cout << " OK" << endl;

        cout << "Test eval...            ";
        test_eval();
        cout << " OK" << endl;

        cout << "Test I/O...             ";
        test_io();
        cout << " OK" << endl;

        cout << "Test define and set...  ";
        test_define_and_set();
        cout << " OK" << endl;

        cout << "Test garbagecollect...  ";
        test_garbagecollect();
        cout << " OK" << endl;

        cout << "Test error handling...  ";
        test_error_handling();
        cout << " OK" << endl;

        test_begin();

    } catch (scheme_exception e) {
	wcerr << L"Exception: " << e.toString() << endl;
        return EXIT_FAILURE;
    } catch (exception e) {
    	cerr << "Exception: " << e.what() << endl;
        return EXIT_FAILURE;
            
    }

    return errors_found == 0 && !suite.hasFailures() ? EXIT_SUCCESS : EXIT_FAILURE;
}




