%module li_std_stream

%inline %{
  struct A;  
%}

%include <std_iostream.i>
%include <std_sstream.i>



%callback(1) A::bar;

%inline %{

  struct B {
    virtual ~B()
    {
    }
    
  };
  
  struct A : B
  {
    void __add__(int a)
    {
    }

    void __add__(double a)
    {
    }

    static int bar(int a){
      return a;
    }

    static int foo(int a, int (*pf)(int a))
    {
      return pf(a);
    }


    std::ostream& __rlshift__(std::ostream& out)
    {
      out << "A class";
      return out;
    }    
  };
%}

%extend std::basic_ostream<char, std::char_traits<char> >{
  std::basic_ostream<char, std::char_traits<char> >& 
    operator<<(const A& a)
    {
      *self << "A class";
      return *self;
    }
}

