#ifndef STRAITX_VALIDABLE_HPP
#define STRAITX_VALIDABLE_HPP

namespace StraitX{

struct Validable{
    virtual bool IsValid()const = 0;
};

}//namespace StraitX::

#endif//STRAITX_VALIDABLE_HPP