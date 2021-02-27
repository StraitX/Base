#include "core/string.hpp"

namespace StraitX{

bool String::Contain(const char *string, const char *internal){
    bool contain = false;
    for(;*string; ++string){
        for(;*internal;++internal){
            if(*string != *internal)
                goto wrong; 
        }
        contain = true;
wrong:    
        ((void)0);
    }
    return contain;
}

void String::ToUpperCase(char *string){
    for(;*string; ++string){
        if(*string <= 'z' && *string >= 'a')
            *string += 'A'-'a';
    }
}

void String::ToLowerCase(char *string){
    for(;*string; ++string){
        if(*string <= 'Z' && *string >= 'A')
            *string += 'a'-'A';
    }
}

size_t String::Length(const char *string){
    size_t length = 1;
    while(*string++){
        ++length;
    }
    return length;
}


s32 String::Compare(const char *first, const char *second){
    while(*first && (*first == *second)){
        ++first;
        ++second;
    }
    return *first - *second;
}

bool String::Equals(const char *first, const char *second){
    return Compare(first, second) == 0;
}

}//namespace StraitX::