
#ifndef R6RS_LIB_IO_PORTS_H
#define R6RS_LIB_IO_PORTS_H

#include "scheme.h"

/**
 * Implementation of 
 *
 * (rnrs io ports (6))
 */
struct R6RSLibIOPorts {
    static void bind(Scheme* scheme, SchemeObject* envt);
};

class Codec {
    public:
        Codec() {};
        virtual ~Codec() {};
    //    virtual void put(ostream* os, wchar_t c) = 0;
        virtual wchar_t get(SchemeObject* port) = 0;
        virtual void unget(SchemeObject* port) = 0;
        wchar_t peek(SchemeObject* is);
};

class Latin1Codec : public Codec {
    public: 
        Latin1Codec() {};
        ~Latin1Codec() {};
        void put(SchemeObject* port, wchar_t c);
        wchar_t get(SchemeObject* port);
        void unget(SchemeObject* port);
};

class UTF8Codec : public Codec {
    public: 
        UTF8Codec() {};
        ~UTF8Codec() {};
        void put(SchemeObject* port, wchar_t c);
        wchar_t get(SchemeObject* port);
        void unget(SchemeObject* port);
};

class UTF16Codec : public Codec {
    public: 
        UTF16Codec() {};
        ~UTF16Codec() {};
        void put(SchemeObject* port, wchar_t c);
        wchar_t get(SchemeObject* port);
        void unget(SchemeObject* port);
};

class RawUnicodeCodec : public Codec {
    public: 
        RawUnicodeCodec() {};
        ~RawUnicodeCodec() {};
        void put(SchemeObject* port, wchar_t c);
        wchar_t get(SchemeObject* port);
        void unget(SchemeObject* port);
};


#define assert_arg_input_port_type(procname, argnum, arg) {     \
    if (i_input_port_p(arg) == S_FALSE) {                       \
        wostringstream ss;                                 \
        ss << "Wrong argument-type (expecting input-port) in position ";         \
        ss << argnum;                                     \
        ss << " in call to ";                             \
        ss << wstring(procname);                           \
        ss << ": " << (arg)->toString();                    \
        throw scheme_exception(ss.str());                 \
    }                                                     \
}

#define assert_arg_binary_port_type(procname, argnum, arg) {     \
    if (i_binary_port_p(arg) == S_FALSE) {                       \
        wostringstream ss;                                 \
        ss << "Wrong argument-type (expecting binary port) in position ";         \
        ss << argnum;                                     \
        ss << " in call to ";                             \
        ss << wstring(procname);                           \
        ss << ": " << (arg)->toString();                    \
        throw scheme_exception(ss.str());                 \
    }                                                     \
}

#define assert_arg_textual_port_type(procname, argnum, arg) {     \
    if (i_textual_port_p(arg) == S_FALSE) {                       \
        wostringstream ss;                                 \
        ss << "Wrong argument-type (expecting textual port) in position ";         \
        ss << argnum;                                     \
        ss << " in call to ";                             \
        ss << wstring(procname);                           \
        ss << ": " << (arg)->toString();                    \
        throw scheme_exception(ss.str());                 \
    }                                                     \
}


#endif

