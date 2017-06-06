#include "stdafx.h"
#include "EnglishTraining.h"
#include "EnglishTrainingDlg.h"
#include <algorithm>

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

int CEnglishTrainingDlg::check_translation(const wstring& user_transl_, int rus_2_eng_){
    auto find = [](const wstring& s_, const wstring& what_)->int{
        if(what_.length() < 3)return 0;
        return s_ == what_ ? 1 : (s_.find(what_) != wstring::npos ? 2 : 0);
    };
    return rus_2_eng_ ? find(_curr_pair.first, user_transl_) : find(_curr_pair.second, user_transl_);
}

void CEnglishTrainingDlg::find_and_replace(std::string& src_, char const* find_what_, char const* replace_by_){
    size_t findLen = strlen(find_what_);
    size_t replaceLen = strlen(replace_by_);
    size_t pos = 0;
    //search for the next occurrence of find within source
    while((pos = src_.find(find_what_, pos)) != std::string::npos){
        // replace the found string with the replacement
        src_.replace(pos, findLen, replace_by_);
        // the next line keeps you from searching your replace string, so your could replace "hello" with "hello world" and not have it blow chunks.
        pos += replaceLen;
    }
}

#include <functional> 
#include <cctype>

// trim from start (in place)
static inline void ltrim(wstring &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        not1(std::ptr_fun<int, int>(isspace))));
}

// trim from end (in place)
static inline void rtrim(wstring &s) {
    s.erase(find_if(s.rbegin(), s.rend(),
        not1(std::ptr_fun<int, int>(isspace))).base(), s.end());
}

void CEnglishTrainingDlg::parse_split_buffer(const wstring& ws_, wstring& word_, wstring& transl_, wstring& syns_){
    // extract translation
    size_t seprtr_idx = ws_.find(L';');
    wstring s_word = ws_.substr(0, seprtr_idx);
    transl_ = ws_.substr(seprtr_idx + 1, wstring::npos);
    // extract transcription from s_word
    auto open_brace = s_word.find_first_of(L'{'), close_brace = s_word.find_first_of(L'}');
    wstring transcr;
    if(open_brace != wstring::npos && close_brace != wstring::npos){
        transcr = s_word.substr(open_brace, close_brace - open_brace + 1);
        wstring s1 = s_word.substr(0, open_brace), s2 = s_word.substr(close_brace + 1, wstring::npos);
        rtrim(s1);
        ltrim(s2);
        rtrim(s2);
        s_word = s1 + s2;
    }
    // extract synonyms
    auto open_bracket = s_word.find_first_of(L'('), close_bracket = s_word.find_first_of(L')');
    if(open_bracket != wstring::npos && close_bracket != wstring::npos){
        syns_ = s_word.substr(open_bracket + 1, close_bracket - open_bracket - 1);
        wstring s1 = s_word.substr(0, open_bracket), s2 = s_word.substr(close_bracket + 1, wstring::npos);
        rtrim(s1);
        ltrim(s2);
        rtrim(s2);
        s_word = s1 + s2;
    }
    word_ = s_word;
    // add transcr back to word
    if(!transcr.empty()){
        word_ += L' ';
        word_ += transcr;
    }
}

void CEnglishTrainingDlg::parse_and_insert_str(wstring& s_, size_t end_ofLine_idx_, bool also_help_map_){
    if(s_.length()){
        wstring s_word, s_transl, s_syns;
        parse_split_buffer(s_.substr(0, end_ofLine_idx_), s_word, s_transl, s_syns);
        if(!s_transl.empty())
            std::transform(s_transl.begin(), s_transl.end(), s_transl.begin(), ::tolower);
        if(!s_word.empty() && !s_transl.empty()){
            _words_map.insert(MAP_PAIR(s_word, s_transl));
            if(also_help_map_)
                _most_active_words_map.insert(MAP_PAIR(s_word, s_transl));
            if(!s_syns.empty())
                _syns.insert(MAP_PAIR(s_word, s_syns));
        }
    }
}

