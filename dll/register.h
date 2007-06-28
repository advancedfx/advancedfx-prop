
// Load time register

#ifndef REGISTER_H
#define REGISTER_H

template <class T>
class RegisterWithFunction
{
	typedef void (*Regfunc_t)(T reg);
public:
	RegisterWithFunction(T in, Regfunc_t func) { (*func)(in); }
};


#endif