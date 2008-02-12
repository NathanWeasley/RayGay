
#include "r6rs-lib-hashtables.h"

using namespace std;

Scheme* myscheme;

SchemeObject* s_make_eq_hashtable(SchemeObject* k) {
    return NULL;
}

SchemeObject* s_make_eqv_hashtable(SchemeObject* k) {
    return NULL;
}

SchemeObject* s_make_hashtable(SchemeObject* hash_func, SchemeObject* equiv_func, SchemeObject* k) {
    int64_t size;
    if (k == S_UNSPECIFIED) {
        size = 256;
    } else {
        size = scm2int(k);
    }
    SchemeObject* buckets = i_make_vector(size, S_EMPTY_LIST);
    return SchemeObject::createHashtable(buckets, hash_func, equiv_func);
}

SchemeObject* s_hashtable_p(SchemeObject* o) {
    return i_hashtable_p(o);
}

SchemeObject* s_hashtable_clear_e(SchemeObject* hashtable, SchemeObject* k) {
    assert_arg_mutable_hashtable_type(L"hashtable-clear!", 1, hashtable);
    int64_t size;
    if (k == S_UNSPECIFIED) {
        size = 256;
    } else {
        size = scm2int(k);
    }
    SchemeObject* buckets = i_make_vector(size, S_EMPTY_LIST);
    hashtable->buckets = buckets;
    return S_UNSPECIFIED;
}

SchemeObject* s_hashtable_set_e(SchemeObject* hashtable, SchemeObject* key, SchemeObject* obj) {
    assert_arg_mutable_hashtable_type(L"hashtable-set!", 1, hashtable);
    SchemeObject* hash_func  = i_car(hashtable->s_hashtable_funcs);
    SchemeObject* equiv_func = i_cdr(hashtable->s_hashtable_funcs);
    SchemeObject* s_hash = myscheme->callProcedure_1(hash_func, key);
    int64_t buckets_num = hashtable->buckets->length;
    int64_t hash = scm2int(s_hash) % buckets_num;
    SchemeObject* bucket_begin = hashtable->buckets->elems[hash];
    SchemeObject* bucket = bucket_begin;
    while (bucket != S_EMPTY_LIST) {
        SchemeObject* entry = i_car(bucket);
        if (myscheme->callProcedure_2(equiv_func, key, i_car(entry)) != S_FALSE) {
            i_set_cdr_e(entry, obj);
            return S_UNSPECIFIED;
        }
        bucket = i_cdr(bucket);
    }
    hashtable->buckets->elems[hash] = i_cons(i_cons(key,obj), bucket_begin);
    return S_UNSPECIFIED;
}

SchemeObject* s_hashtable_ref(SchemeObject* hashtable, SchemeObject* key, SchemeObject* defaul) {
    assert_arg_hashtable_type(L"hashtable-ref", 1, hashtable);
    SchemeObject* hash_func  = i_car(hashtable->s_hashtable_funcs);
    SchemeObject* equiv_func = i_cdr(hashtable->s_hashtable_funcs);
    SchemeObject* s_hash = myscheme->callProcedure_1(hash_func, key);
    int64_t buckets_num = hashtable->buckets->length;
    int64_t hash = scm2int(s_hash) % buckets_num;
    SchemeObject* bucket = hashtable->buckets->elems[hash];
    while (bucket != S_EMPTY_LIST) {
        SchemeObject* entry = i_car(bucket);
        if (myscheme->callProcedure_2(equiv_func, key, i_car(entry)) != S_FALSE) {
            return i_cdr(entry);
        }
        bucket = i_cdr(bucket);
    }
    return defaul;
}

SchemeObject* s_hashtable_delete_e(SchemeObject* hashtable, SchemeObject* key) {
    assert_arg_mutable_hashtable_type(L"hashtable-delete!", 1, hashtable);
    throw scheme_exception(L"Not implemented yet");
}

SchemeObject* s_hashtable_contains_p(SchemeObject* hashtable, SchemeObject* key) {
    assert_arg_hashtable_type(L"hashtable-contains?", 1, hashtable);
    SchemeObject* hash_func  = i_car(hashtable->s_hashtable_funcs);
    SchemeObject* equiv_func = i_cdr(hashtable->s_hashtable_funcs);
    SchemeObject* s_hash = myscheme->callProcedure_1(hash_func, key);
    int64_t buckets_num = hashtable->buckets->length;
    int64_t hash = scm2int(s_hash) % buckets_num;
    SchemeObject* bucket = hashtable->buckets->elems[hash];
    while (bucket != S_EMPTY_LIST) {
        SchemeObject* entry = i_car(bucket);
        if (myscheme->callProcedure_2(equiv_func, key, i_car(entry)) != S_FALSE) {
            return S_TRUE;
        }
        bucket = i_cdr(bucket);
    }
    return S_FALSE;
}

int64_t i_hashtable_size(SchemeObject* hashtable) {
    SchemeObject* buckets = hashtable->buckets;
    int64_t result = 0;
    for(int32_t i = 0; i < buckets->length; i++) {
        result += i_length(buckets->elems[i]);
    }
    return result;
}

SchemeObject* s_hashtable_size(SchemeObject* hashtable) {
    assert_arg_hashtable_type(L"hashtable-size", 1, hashtable);
    return int2scm(i_hashtable_size(hashtable));
}

SchemeObject* s_hashtable_keys(SchemeObject* hashtable) {
    assert_arg_hashtable_type(L"hashtable-clear!", 1, hashtable);
    int64_t entries_num = i_hashtable_size(hashtable);
    SchemeObject* keys = i_make_vector(entries_num, S_UNSPECIFIED);
    SchemeObject* buckets = hashtable->buckets;
    uint64_t j = 0;
    for(int32_t i = 0; i < buckets->length; i++) {
        SchemeObject* bucket = buckets->elems[i];
        while (bucket != S_EMPTY_LIST) {
            SchemeObject* entry = i_car(bucket);
            keys->elems[j++] = i_car(entry);
            bucket = i_cdr(bucket);
        }
    }
    return keys;
}

SchemeObject* s_hashtable_entries(SchemeObject* hashtable) {
    assert_arg_hashtable_type(L"hashtable-clear!", 1, hashtable);
    int64_t entries_num = i_hashtable_size(hashtable);
    SchemeObject* keys = i_make_vector(entries_num, S_UNSPECIFIED);
    SchemeObject* values = i_make_vector(entries_num, S_UNSPECIFIED);
    SchemeObject* buckets = hashtable->buckets;
    uint64_t j = 0;
    for(int32_t i = 0; i < buckets->length; i++) {
        SchemeObject* bucket = buckets->elems[i];
        while (bucket != S_EMPTY_LIST) {
            SchemeObject* entry = i_car(bucket);
            keys->elems[j] = i_car(entry);
            values->elems[j] = i_cdr(entry);
            j++;
            bucket = i_cdr(bucket);
        }
    }
    return i_cons(keys,i_cons(values,S_EMPTY_LIST));
}


SchemeObject* s_hashtable_equivalence_function(SchemeObject* hashtable) {
    assert_arg_hashtable_type(L"hashtable-equivalence-function", 1, hashtable);
    return i_cdr(hashtable->s_hashtable_funcs);
}

SchemeObject* s_hashtable_hash_function(SchemeObject* hashtable) {
    assert_arg_hashtable_type(L"hashtable-hash-function", 1, hashtable);
    return i_car(hashtable->s_hashtable_funcs);
}

SchemeObject* s_hashtable_mutable_p(SchemeObject* hashtable) {
    assert_arg_hashtable_type(L"hashtable-mutable?", 1, hashtable);
    return hashtable->immutable() ? S_FALSE : S_TRUE;
}

uint32_t string_hash(std::wstring str) {
    uint32_t h = 1;
    for(uint32_t i = 0; i < str.size(); i++) {
        h *= str[i] + 37;
    }
    return h;
}

SchemeObject* s_string_hash(SchemeObject* o) {
    assert_arg_type(L"string-hash", 1, s_string_p, o);
    return int2scm(string_hash(o->str));
}

SchemeObject* s_string_ci_hash(SchemeObject* o) {
    assert_arg_type(L"string-ci-hash", 1, s_string_p, o);
    throw scheme_exception(L"Not implemented");
}

SchemeObject* s_symbol_hash(SchemeObject* o) {
    assert_arg_type(L"symbol-hash", 1, s_symbol_p, o);
    return int2scm(string_hash(o->str));
}

SchemeObject* s_equal_hash(SchemeObject* o) {
    SchemeObject::ObjectType t = o->type();
    if (t == SchemeObject::INTEGER_NUMBER) {
        return o;
    } else if (t == SchemeObject::PAIR) {
        return int2scm(scm2int(s_equal_hash(i_car(o))) + 37 * scm2int(s_equal_hash(i_cdr(o))));    
    } else if (o == S_EMPTY_LIST) {
        return int2scm(123456789);
    } else if (t == SchemeObject::STRING) {
        return s_string_hash(o);    
    } else if (t == SchemeObject::SYMBOL) {
        return s_symbol_hash(o);    
    } else {
        return int2scm(string_hash(o->toString()));
    }
    return NULL;
}

void R6RSLibHashtables::bind(Scheme* scheme, SchemeObject* envt) {
    myscheme = scheme;
    scheme->assign(L"make-eq-hashtable"    ,0,1,0, (SchemeObject* (*)()) s_make_eq_hashtable, envt);
    scheme->assign(L"make-eqv-hashtable"   ,0,1,0, (SchemeObject* (*)()) s_make_eqv_hashtable, envt);
    scheme->assign(L"make-hashtable"       ,2,1,0, (SchemeObject* (*)()) s_make_hashtable, envt);
    scheme->assign(L"hashtable?"           ,1,0,0, (SchemeObject* (*)()) s_hashtable_p, envt);
    scheme->assign(L"hashtable-size"       ,1,0,0, (SchemeObject* (*)()) s_hashtable_size, envt);
    scheme->assign(L"hashtable-ref"        ,3,0,0, (SchemeObject* (*)()) s_hashtable_ref, envt);
    scheme->assign(L"hashtable-set!"       ,3,0,0, (SchemeObject* (*)()) s_hashtable_set_e, envt);
    scheme->assign(L"hashtable-delete!"    ,2,0,0, (SchemeObject* (*)()) s_hashtable_delete_e, envt);
    scheme->assign(L"hashtable-contains?"  ,2,0,0, (SchemeObject* (*)()) s_hashtable_contains_p, envt);
    scheme->assign(L"hashtable-clear!"     ,1,1,0, (SchemeObject* (*)()) s_hashtable_clear_e, envt);
    scheme->assign(L"hashtable-keys"       ,1,0,0, (SchemeObject* (*)()) s_hashtable_keys, envt);
    scheme->assign(L"hashtable-entries"    ,1,0,0, (SchemeObject* (*)()) s_hashtable_entries, envt);
    scheme->assign(L"hashtable-equivalence-function",1,0,0, (SchemeObject* (*)()) s_hashtable_equivalence_function, envt);
    scheme->assign(L"hashtable-hash-function",1,0,0, (SchemeObject* (*)()) s_hashtable_hash_function, envt);
    scheme->assign(L"hashtable-mutable?"   ,1,0,0, (SchemeObject* (*)()) s_hashtable_mutable_p, envt);
    scheme->assign(L"equal-hash"           ,1,0,0, (SchemeObject* (*)()) s_equal_hash, envt);
    scheme->assign(L"string-hash"          ,1,0,0, (SchemeObject* (*)()) s_string_hash, envt);
    scheme->assign(L"string-ci-hash"       ,1,0,0, (SchemeObject* (*)()) s_string_ci_hash, envt);
    scheme->assign(L"symbol-hash"          ,1,0,0, (SchemeObject* (*)()) s_symbol_hash, envt);
}
