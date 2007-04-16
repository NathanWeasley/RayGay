
#ifndef SCHEME_INTERPRETER_H
#define SCHEME_INTERPRETER_H

#include "objects.h"
#include "scheme.h"
#include <setjmp.h>
#include <vector>

typedef void*(*fn_ptr)();

class Interpreter
{
    public:
	    Interpreter(SchemePair* parsetree, BindingEnvironment* top_level);
   	    SchemeObject* interpret();
        SchemeObject* call_procedure_n(SchemeObject* procedure, SchemeObject* args);
   	    
   	private:
		BindingEnvironment* top_level_bindings;
        SchemePair* parsetree;
};

SchemeObject* trampoline(fn_ptr);
fn_ptr eval();
fn_ptr eval_sequence();
fn_ptr eval_define();
fn_ptr eval_multi();
fn_ptr eval_procedure_call();
fn_ptr eval_let();
fn_ptr eval_letstar();
fn_ptr eval_letrec();
fn_ptr eval_named_let();
fn_ptr eval_cond();
fn_ptr eval_case();
fn_ptr eval_and();
fn_ptr eval_or();
fn_ptr eval_set_e();
fn_ptr eval_for_each();
fn_ptr eval_map();
fn_ptr eval_combo();
fn_ptr eval_if();
fn_ptr eval_define_macro();
fn_ptr eval_call_macro();
fn_ptr eval_apply();
fn_ptr eval_quasiquote();
fn_ptr eval_lambda();
fn_ptr eval_do();
fn_ptr eval_begin();
fn_ptr eval_list();


#endif
