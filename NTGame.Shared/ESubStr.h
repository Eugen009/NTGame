#ifndef __E_SUB_STR_H__
#define __E_SUB_STR_H__
#include <assert.h>
#include <string>
using namespace Platform;

namespace EResource{

	static const wchar_t* END_LETTERS = L"\n\r";
	static const wchar_t* BLANK_LETTERS = L"\t ";

	class ESubStr{
	public:
		ESubStr(){ 
			m_pStr = nullptr;
			startPos = 0;
			endPos = 0;
			m_pStart = NULL;
		}
		ESubStr(String^ pstr, unsigned start=0){ 
			if (pstr != nullptr && start < (int)pstr->Length()){
				m_pStr = pstr; startPos = start; endPos = pstr->Length();
				m_pStart = m_pStr->Begin() + startPos;
			} else{
				m_pStr = nullptr;
				startPos = 0;
				endPos = 0;
				m_pStart = NULL;
			}
		}
		inline unsigned getLen() const{ return endPos - startPos; }
		inline const wchar_t* begin() const{ return m_pStart; }
		inline const wchar_t* end() const{ return m_pStr->Begin() + endPos; }
		inline const unsigned getStartPos() const{ return startPos; }
		inline const unsigned getEndPos() const { return endPos; }
		inline bool empty() const{ return endPos == startPos; }
		inline bool operator==(const wchar_t* compareStr){
			const wchar_t* cur = begin();
			const wchar_t* endPtr = end();
			while (cur != endPtr && compareStr != L'\0'){
				if (*cur != *compareStr){
					return false;
				}
				cur++;
				compareStr++;
			}
			if (cur != endPtr) return false;
			return true;
		}
		inline bool operator==(const ESubStr& subStr){
			const wchar_t* cur = begin();
			const wchar_t* endPtr = end();
			const wchar_t* comCur = subStr.begin();
			const wchar_t* comEnd = subStr.end();
			while (cur != endPtr && comCur != comEnd){
				if (*cur != *comCur){
					return false;
				}
				cur++; comCur++;
			}
			if (cur != endPtr || comCur != comEnd) return false;
			return true;
		}
		wchar_t operator[](unsigned index) const{
			assert(index < getLen() && index >= 0);
			return m_pStr->Data()[startPos + index];
		}
		inline unsigned findFirstNotOf(const wchar_t* notOfLetters, unsigned startOffset = 0) const{
			assert(m_pStr != nullptr);
			assert(notOfLetters);
			unsigned index = startOffset;
			unsigned len = getLen();
			if (startOffset >= getLen()) return getLen();
			const wchar_t* src = begin() + startOffset;
			const wchar_t* pEnd = end();
			while ( src != pEnd && index < len){
				const wchar_t* c = notOfLetters;
				while (*c != L'\0'){
					if (*c == *src) break;
					c++;
				}
				if (*c == L'\0') return index;
				src++;
				index++;
			}
			return index;
		}
		unsigned findFirstOf(const wchar_t* letters, unsigned startOffset = 0) const{
			assert(m_pStr);
			assert(letters);
			if (startOffset > getLen()) return getLen();
			unsigned index = startOffset;
			unsigned len = getLen();
			const wchar_t* src = begin() + startOffset;
			const wchar_t* pEnd = end();
			while ( src != pEnd && index < len){
				const wchar_t* c = letters;
				while (*c != L'\0'){
					if (*c == *src) return index;
					c++;
				}
				src++;
				index++;
			}
			return index;
		}
		inline unsigned findLastNotOf(const wchar_t* notOfLetters, unsigned startOffset = 0) const{
			assert(m_pStr);
			assert(notOfLetters);
			unsigned len = getLen();
			if (startOffset >= len) return len;
			unsigned index = startOffset;
			const wchar_t* pBegin = begin();
			const wchar_t* src = end() - (startOffset + 1);
			while ( index < len){
				const wchar_t* c = notOfLetters;
				while (*c != L'\0'){
					if (*c == *src) break;
					c++;
				}
				if (*c == L'\0') break;
				src--;
				index++;
			}
			if (index > len)
				return len;
			return len - index;
		}
		ESubStr getLine(unsigned offset = 0) const{
			if (offset >= getLen()) return ESubStr();
			ESubStr line( m_pStr, startPos + offset );
			const wchar_t* endLetters = L"\r\n";
			line.endPos = this->findFirstOf(endLetters, line.startPos);
			line.endPos += startPos;
			return line;
		}
		ESubStr getWord(int offset = 0) const{
			const wchar_t* cur = begin() + offset;
			const wchar_t* end = this ->end();
			ESubStr word( m_pStr, offset + startPos );
			//word.m_pStr = m_pStr;
			//word.startPos = offset + startPos;
			word.endPos = word.startPos;
			while ( cur != end && word.endPos < endPos){
				if (isAlpha(*cur)){
					cur++;
					word.endPos++;
				} else break;
			}
			return word;
		}
		ESubStr subStr(unsigned from, unsigned len = 0 ) const{
			//next.m_pStr = m_pStr;
			unsigned curLen = getLen();
			if (from > curLen) from = curLen;
			unsigned leftLen = curLen - from;
			if (len == 0) len = getLen();
			if (len > leftLen) len = leftLen;
			ESubStr next( m_pStr, startPos + from );
			//next.startPos = from + startPos;
			next.endPos = next.startPos + len;
			return next;
		}
		void trim(){
			startPos = findFirstNotOf(BLANK_LETTERS) + startPos;
			endPos = findLastNotOf(BLANK_LETTERS) + startPos;
		}
		static bool isAlpha(const wchar_t c){
			if ((c >= L'a'&& c <= L'z') ||
				(c >= L'A'&& c <= L'Z')){
				return true;
			}
			return false;
		}
		std::wstring toString() const{
			return std::wstring(begin(), getLen());
		}
		std::string asChar() const{
			std::string temp;// (getLen());
			temp.resize(getLen());
			const wchar_t* cur = begin();
			int i = 0;
			for (; cur != end(); cur++){
				assert(*cur < 256);
				temp[i++] = (char)*cur;
			}
			return temp;
		}
		bool toFloat(float& val) const{
			ESubStr temp = *this;
			temp.trim();
			val = (float)atof(temp.asChar().c_str());
			return true;
		}
		bool toInt(int& val) const{
			ESubStr temp = *this;
			temp.trim();
			val = atoi(temp.asChar().c_str());
			return true;
		}
		int getIntFrom( int index, unsigned& nextIndex ) const {
			int startIndex = findFirstNotOf(BLANK_LETTERS, index );
			nextIndex = findFirstOf(BLANK_LETTERS, startIndex);
			int ti = 0;
			this->subStr(startIndex, nextIndex).toInt(ti);
			return ti;
		}
		

	protected:
		String^ m_pStr;
		const wchar_t* m_pStart;
		unsigned startPos;
		unsigned endPos;
	};
}

#endif