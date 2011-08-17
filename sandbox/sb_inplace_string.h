/*
 *  sb_inplace_string.h
 *  SR
 *
 *  Created by Андрей Куницын on 10.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_INPLACE_STRING_H
#define SB_INPLACE_STRING_H

#include <cstring>
#include <string>
#include "sb_assert.h"

namespace Sandbox {
	
	class InplaceString {
	private:
		const char* m_begin;
		const char*	m_end;
	public:
		InplaceString() : m_begin(0),m_end(0) {}
		InplaceString(const char* begin_,const char* end_) : m_begin(begin_),m_end(end_) { }
		explicit InplaceString(const char* str_) :m_begin(str_),m_end(0) {
			if (str_) m_end = m_begin + ::strlen(str_);
		}
		bool empty() const { return m_begin == m_end; }
		size_t length() const { return m_end ? m_end-m_begin : 0 ;}
		const char* begin() const { return m_begin;}
		const char* end() const { return m_end;}
		const char* find(char c) { const char* str_ = m_begin ; while ( (str_!=m_end) && (*str_!=c)) {str_++;} return str_; }
		const char* find(const char* any) { 
			const char* str_ = m_begin ; 
			InplaceString d(any);
			while ( (str_!=m_end) && (d.find(*str_)==d.end()) ) {
				str_++;
			} 
			return str_; 
		}
		const char* rfind(char c) { const char* str_ = m_end ; while ( (str_!=m_begin) && (*str_!=c)) {str_--;} return str_; }
		bool operator == (const InplaceString& str_) const {
			return (length() == str_.length()) && (::strncmp(begin(),str_.begin(),length())==0);
		}
		bool operator == (const char* str_) const { 
			return (::strncmp(begin(),str_,length())==0) && ( str_[length()]=='\0' );
		} 
		bool operator != (const InplaceString& str_) const { return !(str_==*this);}
		bool operator != (const char* str_) const { return !(*this==str_);}
		
		std::string str() const { return std::string(begin(),end());}
	};
}
#endif /*SB_INPLACE_STRING_H*/