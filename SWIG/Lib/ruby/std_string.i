//
// SWIG typemaps for std::string
// Luigi Ballabio
// Apr 8, 2002
//
// Ruby implementation


// ------------------------------------------------------------------------
// std::string is typemapped by value
// This can prevent exporting methods which return a string
// in order for the user to modify it.
// However, I think I'll wait until someone asks for it...
// ------------------------------------------------------------------------

%include exception.i

%{
#include <string>
%}

namespace std {

    // Ruby wants class names to start with a capital letter
    %rename(String) string;
    class string;

    %typemap(in) string {
        if (TYPE($input) == T_STRING)
            $1 = std::string(STR2CSTR($input));
        else
            SWIG_exception(SWIG_TypeError, "not a string");
    }

    %typemap(in) const string & (std::string temp) {
        if (TYPE($input) == T_STRING) {
            temp = std::string(STR2CSTR($input));
            $1 = &temp;
        } else {
            SWIG_exception(SWIG_TypeError, "not a string");
        }
    }

    %typemap(out) string {
        $result = rb_str_new2($1.c_str());
    }

    %typemap(out) const string & {
        $result = rb_str_new2($1->c_str());
    }

}
