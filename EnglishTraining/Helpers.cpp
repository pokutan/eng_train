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

void CEnglishTrainingDlg::parse_and_insert_str(wstring& ws, size_t end_ofLine_idx_, bool also_help_map_){
    if(ws.length()){
        size_t seprtr_idx = ws.find(L';');
        wstring s1 = ws.substr(0, seprtr_idx);
        wstring s2 = ws.substr(seprtr_idx + 1, end_ofLine_idx_ - seprtr_idx - 1);
        std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
        std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
        if(s1.length() && s2.length()){
            // check for English synonyms
            if((seprtr_idx = s1.find(L'(')) != wstring::npos){
                // check that it's not inside transcription
                size_t close_bracket = s1.find(L'}');
                if(close_bracket == wstring::npos || close_bracket < seprtr_idx){
                    size_t end_brace = s1.find(L')');
                    if(end_brace != wstring::npos){
                        wstring syns = s1.substr(seprtr_idx + 1, end_brace - seprtr_idx - 1);
                        s1 = s1.substr(0, seprtr_idx);
                        _syns.insert(MAP_PAIR(s1, syns));
                    }
                }
            }
            _words_map.insert(MAP_PAIR(s1, s2));
            if(also_help_map_)
                _most_active_words_map.insert(MAP_PAIR(s1, s2));
            //            fill_ui_data(false);
        }
    }
}

