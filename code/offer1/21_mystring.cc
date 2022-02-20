#include <iostream>
#include <cstring>
#include <cassert>

class CMyString {
    char * m_pData;

    void print(const char * name, const char * pData) {
        std::cout << name << " : " << (pData ? pData : "nullptr") << std::endl;
    }

public:
    CMyString(char * pData = nullptr) {
        this->print("CMyString(char * pData = nullptr)", pData);

        if (pData == nullptr) {
            this->m_pData = nullptr;
        } else {
            size_t len = strlen(pData) + 1;
            m_pData = new char[len];
            strcpy(this->m_pData, pData);
        }
    }

    CMyString(const CMyString & str) {
        if (str.m_pData == nullptr) {
            this->m_pData = nullptr;
        } else {
            size_t len = strlen(str.m_pData) + 1;
            m_pData = new char[len];
            strcpy(this->m_pData, str.m_pData);
        }
    }

    ~CMyString(void) {
        this->print("~CMyString(void)", this->m_pData);

        delete this->m_pData;
    }

    char * getMPData(void) {
        return this->m_pData;
    }

    CMyString & operator=(const CMyString & str) {
        if (this != &str && this->m_pData != str.m_pData) {
            CMyString tmp(str);
            
            char * pData = tmp.m_pData;
            tmp.m_pData = this->m_pData;
            this->m_pData = pData;
        }
        return *this;
    }
};

//
// build:
// g++ -g -O3 -Wall -Wextra -Werror -o 21_mystring 21_mystring.cc
int main(void) {
    CMyString str;
    assert(str.getMPData() == nullptr);

    // exit 不会触发析构
    // exit(EXIT_SUCCESS);

    return EXIT_SUCCESS;
}