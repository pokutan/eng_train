#include "stdafx.h"

#include "EnglishTraining.h"
#include "EnglishTrainingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

//#include "scope_guard.h"
#include <fstream>
#include <sstream>
#include <locale>
#include <list>

using namespace std;

OPTIONS::OPTIONS() {
    wchar_t opt_file[MAX_PATH]={};
    SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, opt_file);
    wcscat_s(opt_file, MAX_PATH, OPT_FILENAME);
    OutputDebugStringW(opt_file);
    _cfg_file = CreateFileW(opt_file, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD d = sizeof(_static_data);
    if(_cfg_file != INVALID_HANDLE_VALUE && GetLastError() == ERROR_ALREADY_EXISTS)
        if(ReadFile(_cfg_file, &_static_data, d, &d, NULL))
            SetFilePointer(_cfg_file, 0, NULL, FILE_BEGIN);
    wcscpy_s(_static_data._all_search_urls[0], L"cnn.com");
    wcscpy_s(_static_data._all_search_urls[1], L"bbc.co.uk");
    wcscpy_s(_static_data._all_search_urls[2], L"bbc.com");
    wcscpy_s(_static_data._all_search_urls[3], L"washingtonpost.com");
    wcscpy_s(_static_data._all_search_urls[4], L"ft.com");
    wcscpy_s(_static_data._all_search_urls[5], L"wsj.com");
    wcscpy_s(_static_data._all_search_urls[6], L"berkshirehathaway.com");
    wcscpy_s(_static_data._all_search_urls[7], L"nationalgeographic.com");
    wcscpy_s(_static_data._all_search_urls[8], L"health.com");
    wcscpy_s(_static_data._all_search_urls[9], L"webmd.com");
    wcscpy_s(_static_data._all_search_urls[10], L"youtube.com");
#ifdef _DEBUG
    _timeouts.insert(std::pair<int,wstring>(1*ONE_SEC, L"1"));
#endif
    _timeouts.insert(std::pair<int,wstring>(5*ONE_SEC, L"5"));
    _timeouts.insert(std::pair<int,wstring>(10*ONE_SEC, L"10"));
    _timeouts.insert(std::pair<int,wstring>(30*ONE_SEC, L"30"));
    _timeouts.insert(std::pair<int,wstring>(60*ONE_SEC, L"60"));
    _timeouts.insert(std::pair<int,wstring>(120*ONE_SEC, L"120"));
    _timeouts.insert(std::pair<int,wstring>(180*ONE_SEC, L"180"));
    _timeouts.insert(std::pair<int,wstring>(300*ONE_SEC, L"300"));
}

OPTIONS::~OPTIONS(){
    if(_cfg_file != INVALID_HANDLE_VALUE){
        DWORD d = sizeof(_static_data);
        WriteFile(_cfg_file, &_static_data, sizeof(_static_data), &d, NULL);
        CloseHandle(_cfg_file);
    }
}

void OPTIONS::set_show_timeout(wchar_t const* to_){
    if(to_ && to_[0])
        for(auto it : _timeouts)
            if(it.second == to_)
                _static_data._to = it.first;
}

void OPTIONS::set_rand(bool rand_){ _static_data._is_random = rand_ ? 1 : 0; }

CEnglishTrainingDlg::CEnglishTrainingDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CEnglishTrainingDlg::IDD,pParent), _random_pair(true), _my_timer(-1), _vocab_auto_timer(-1){
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    _rnd = new gen_random<int>;
//    _urls[url_vocab] = "http://www.classes.ru/dictionary-english-russian-Apresyan.htm";
    _urls[url_vocab] = "http://www.classes.ru/dictionary-english-russian-Mueller.htm";
    _urls[url_prononce] = "http://howjsay.com/pronunciation-of-";
    _urls[url_webster] = "http://www.merriam-webster.com/dictionary/";
    _urls[url_examples] = "http://www.macmillandictionary.com/dictionary/british/";
    //_urls[url_bbc] = "https://www.google.com/search?q={_word_}+site%3Abbc.co.uk&oq={_word_}+site%3A_{site}_&aqs=chrome..69i57.11152j0j7&sourceid=chrome&ie=UTF-8";
    _urls[url_bbc] = "https://www.google.com/search?q={_word_}+site%3A_{site}_";
    _urls[url_youglish] = "https://youglish.com/search/";
    _urls[url_synonym] = "http://www.thesaurus.com/browse/";
}

void CEnglishTrainingDlg::DoDataExchange(CDataExchange* pDX){
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, ID_STAT_SORCE_WORD, SourceWord);
    DDX_Control(pDX, ID_COMBO_TRANSLATE, Translations);
    DDX_Control(pDX, ID_STAT_RES, Stat_Result);
    DDX_Control(pDX, ID_COMBO_TO, ComboTO);
    DDX_Control(pDX, ID_STAT_PREV, PrevTranslation);
    DDX_Control(pDX, IDC_RADIO1, RadioLearn);
    DDX_Control(pDX, IDC_RADIO2, RadioChoose);
    DDX_Control(pDX, ID_CHECK_FROM_ENG, CheckTranslateFromEng);
    DDX_Control(pDX, ID_CHECK_ONTOP, CheckOnTop);
    DDX_Control(pDX, ID_BTN_SYNS, BtnSyns);
    DDX_Control(pDX, ID_CHECK_RANDOM, CheckBoxRandom);
    DDX_Control(pDX, ID_COMBO_SITE_URL, ComboSearchUrl);
    DDX_Control(pDX, ID_CHECK_AUTO, CheckBoxAuto);
    DDX_Control(pDX, ID_BTN_CLEAR_WORD, BtnForgetWord);
    DDX_Control(pDX, ID_BTN_PLUS_WORD, BtnAddWord);
    DDX_Control(pDX, ID_BTN_PRONON, BtnPrononc);
    DDX_Control(pDX, ID_BTN_EXAMP, BtnVocabWebster);
    DDX_Control(pDX, ID_BTN_DICT, BtnVocabMueller);
    DDX_Control(pDX, ID_BTN_PAUSE, BtnPauseContinue);
    DDX_Control(pDX, ID_BTN_FORW, BtnMoveForward);
    DDX_Control(pDX, ID_CHECK_USE_PREFER_MAP, CheckBoxUsePreferMap);
}

BEGIN_MESSAGE_MAP(CEnglishTrainingDlg,CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_BN_CLICKED(ID_BTN_SUBMIT,&CEnglishTrainingDlg::OnBnClickedBtnSubmit)
    ON_CBN_SELCHANGE(ID_COMBO_TO,&CEnglishTrainingDlg::OnCbnSelchangeComboTo)
    ON_BN_CLICKED(ID_BTN_EDITFILE,&CEnglishTrainingDlg::OnBnClickedBtnEditfile)
    ON_BN_CLICKED(ID_BTN_RELOAD,&CEnglishTrainingDlg::OnBnClickedBtnReload)
    ON_BN_CLICKED(IDC_RADIO2,&CEnglishTrainingDlg::OnBnClickedRadioVocab)
    ON_BN_CLICKED(IDC_RADIO1,&CEnglishTrainingDlg::OnBnClickedRadioLearn)
    ON_BN_CLICKED(ID_BTN_PRONON,&CEnglishTrainingDlg::OnBnClickedBtnPrononce)
    ON_BN_CLICKED(ID_BTN_EXAMP,&CEnglishTrainingDlg::OnBnClickedBtnExamp)
    ON_BN_CLICKED(ID_BTN_HELP,&CEnglishTrainingDlg::OnBnClickedBtnHelp)
    ON_BN_CLICKED(ID_BTN_SET_FILE,&CEnglishTrainingDlg::OnBnClickedBtnSetFile)
    ON_WM_DESTROY()
    ON_BN_CLICKED(ID_BTN_DICT,&CEnglishTrainingDlg::OnBnClickedBtnDict)
    ON_BN_CLICKED(ID_CHECK_FROM_ENG,&CEnglishTrainingDlg::OnBnClickedCheckFromEngToRus)
    ON_STN_DBLCLK(ID_STAT_PREV,&CEnglishTrainingDlg::OnDblclkStatPrev)
    ON_STN_DBLCLK(ID_STAT_SORCE_WORD,&CEnglishTrainingDlg::OnStnDblclickStatSorceWord)
    ON_WM_ACTIVATE()
    ON_STN_DBLCLK(ID_STAT_RES,&CEnglishTrainingDlg::OnStnDblclickStatRes)
    ON_BN_CLICKED(ID_BTN_CLEAR_WORD,&CEnglishTrainingDlg::OnBnClickedBtnClearWord)
    ON_BN_CLICKED(ID_CHECK_ONTOP,&CEnglishTrainingDlg::OnBnClickedCheckOntop)
    ON_BN_CLICKED(ID_BTN_PLUS_WORD,&CEnglishTrainingDlg::OnBnClickedBtnPlusWord)
    ON_BN_CLICKED(ID_BTN_SYNS,&CEnglishTrainingDlg::OnBnClickedBtnSyns)
    ON_BN_CLICKED(ID_CHECK_RANDOM, &CEnglishTrainingDlg::OnBnClickedCheckRandom)
    ON_CBN_KILLFOCUS(ID_COMBO_SITE_URL, &CEnglishTrainingDlg::OnCbnKillfocusComboSiteUrl)
    ON_BN_CLICKED(ID_CHECK_AUTO, &CEnglishTrainingDlg::OnBnClickedCheckAuto)
    ON_BN_CLICKED(ID_BTN_PAUSE, &CEnglishTrainingDlg::OnBnClickedBtnPause)
    ON_BN_CLICKED(ID_BTN_FORW, &CEnglishTrainingDlg::OnBnClickedBtnForw)
    ON_BN_CLICKED(ID_BTN_ADD_TO_MOST, &CEnglishTrainingDlg::OnBnClickedBtnAddToMost)
    ON_BN_CLICKED(ID_CHECK_USE_PREFER_MAP, &CEnglishTrainingDlg::OnBnClickedCheckUsePreferMap)
    ON_WM_KEYDOWN()
    ON_BN_CLICKED(ID_BTN_APPHELP, &CEnglishTrainingDlg::OnBnClickedBtnApphelp)
END_MESSAGE_MAP()

void CEnglishTrainingDlg::fill_main_combo(int rus_){
    for(int i = Translations.GetCount() - 1; i >= 0; i--)
        Translations.DeleteString(i);
    for(MAP_IT it = _words_map.begin(); it != _words_map.end(); ++it)
        Translations.AddString(rus_ ? it->first.c_str() : it->second.c_str());
    for(MAP_IT it = _most_active_words_map.begin(); it != _most_active_words_map.end(); ++it)
        Translations.AddString(rus_ ? it->first.c_str() : it->second.c_str());
    Translations.SetCurSel(-1);
    Translations.SetDroppedWidth(!rus_ ? 500 : 100);
}

void CEnglishTrainingDlg::fill_ui_data(_In_ bool update_prev_, _In_opt_ bool reset_/* = false */){
    if(!_words_map.size()){
        Stat_Result.SetWindowTextW(L"Error loading words");
        SetWindowTextA(m_hWnd, _caption.c_str());
        return;
    }
    _try_counter = 1;
    _curr_cue_idx = 0;
    if(_opt._static_data._vocab_from_rus2eng)
        CheckTranslateFromEng.SetCheck(BST_CHECKED);
    if(_opt._static_data._vocab_auto)
        CheckBoxAuto.SetCheck(BST_CHECKED);
    static gen_random<int> gr(0,2);
    wstring s;
    if(_mode_learn)
        _rus2eng_learn = gr;
    else{
        MAP_IT it = get_random_pair(true, reset_);
        wstring s(it->first);
        s += L" - ";
        s += it->second;
        Stat_Result.SetWindowTextW(s.c_str());
    }
//    _rus2eng_learn = 0;
    fill_main_combo(_mode_learn ? !_rus2eng_learn : _opt._static_data._vocab_from_rus2eng);
    if(_mode_learn){
        bool base_map = true;
        if(_most_active_words_map.size()){
            static int cnt = 0;
            if(++cnt == 5){
                cnt = 0;
                base_map = false;
            }
        }
        if(base_map && CheckBoxUsePreferMap.GetCheck()==BST_CHECKED)
            base_map = false;
        MAP_CIT it = _words_map.begin();
        if(_random_pair)
            it = get_random_pair(base_map, reset_);
        else{
            if(_it_lasting == _words_map.end())
                _it_lasting = _words_map.begin();
            it = _it_lasting;
            ++_it_lasting;
        }
        SourceWord.SetWindowText(_rus2eng_learn ? it->first.c_str() : it->second.c_str());
        _curr_right_transl = !_rus2eng_learn ? it->first : it->second;
        Stat_Result.SetWindowTextW(L"Choose Translation");
        if(update_prev_ && _curr_pair.first.length()){
            s = _curr_pair.first;
            s += L" - ";
            s += _curr_pair.second;
            PrevTranslation.SetWindowTextW(s.c_str());
        }
        _curr_pair = *it;
        it = _syns.find(_curr_pair.first);
        BtnSyns.EnableWindow(it != _syns.end());
    }else
        _curr_pair = MAP_PAIR(L"", L"");
    ActivateKeyboardLayout(_mode_learn ? (_rus2eng_learn ? _rus_kbd : _eng_kbd) : (!_opt._static_data._vocab_from_rus2eng ? _rus_kbd : _eng_kbd), KLF_SETFORPROCESS);
    string ss = _caption;
    ss += " - ";
    ss += _source_file;
    ss += " (";
    ss += std::to_string(_words_map.size() + _most_active_words_map.size());
    ss += " words)";
    SetWindowTextA(m_hWnd, ss.c_str());
}

void CEnglishTrainingDlg::fill_to_combo(){
    int curr_to_idx = 0;
    bool to_set = false;
    for(std::map<int, wstring>::iterator i = _opt._timeouts.begin(); i != _opt._timeouts.end(); ++i){
        ComboTO.AddString(i->second.c_str());
        if(i->first == _opt.to())
            to_set = true;
        if(!to_set)
            ++curr_to_idx;
    }
    ComboTO.SetCurSel(curr_to_idx);
}

BOOL CEnglishTrainingDlg::OnInitDialog(){
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon,TRUE);
    SetIcon(m_hIcon,FALSE);
    _caption = "English by pokutan";
    // https://msdn.microsoft.com/en-us/library/dd318693.aspx
    _eng_kbd = LoadKeyboardLayoutW(L"00000409",KLF_SETFORPROCESS|KLF_ACTIVATE);
    _rus_kbd = LoadKeyboardLayoutW(L"00000419",KLF_SETFORPROCESS|KLF_ACTIVATE);
    DWORD d = MAX_PATH;
    GetComputerNameW(_comp_name,&d);
    if(!_wcsicmp(_comp_name,L"pokutan")){
        strcpy_s(_source_file, MAX_PATH, "F:\\Dropbox\\eng\\my_app\\words.txt");
        if(!IsFileExists(_source_file))
            strcpy_s(_source_file, MAX_PATH, "f:\\Dropbox\\eng\\my_app\\words.txt");
    }else if(!_wcsicmp(_comp_name,L"IVANZ1-WS"))
        strcpy_s(_source_file, MAX_PATH, IsFileExists("\\\\ivanz-tp\\Dropbox\\eng\\my_app\\words.txt") ? "\\\\ivanz-tp\\Dropbox\\eng\\my_app\\words.txt" : "C:\\dev_my\\bin\\words.txt");
    else if(!_wcsicmp(_comp_name, L"IVANZ-TP"))
        strcpy_s(_source_file, MAX_PATH, "C:\\Private\\Dropbox\\eng\\my_app\\words.txt");
    if(!IsFileExists(_source_file)){
        ::GetCurrentDirectoryA(MAX_PATH, _source_file);
        strcat_s(_source_file, MAX_PATH, "\\words.txt");
    }
    OutputDebugStringA(_source_file);
#ifdef _DEBUG
    if(IsFileExists("\\\\ivanz-tp\\Dropbox\\eng\\my_app\\words_debug.txt"))
        strcpy_s(_source_file, MAX_PATH, "\\\\ivanz-tp\\Dropbox\\eng\\my_app\\words_debug.txt");
#endif
    read_source_file();
    _mode_learn = (_opt.regime() == OPTIONS::A::APP_REGIME::_study_) ? true : false;
    CheckBoxRandom.SetCheck(_random_pair = _opt.rand());
    CheckBoxUsePreferMap.EnableWindow(FALSE);
    if(_mode_learn)
        OnBnClickedRadioLearn();
    fill_to_combo();
    if(!_mode_learn)
        OnBnClickedRadioVocab();
#ifndef _DEBUG
    SetWindowPos(&wndTopMost, _opt.left(), _opt.top(), 0, 0, SWP_NOSIZE);
    CheckOnTop.SetCheck(BST_CHECKED);
#endif
    for(int i = 0; _opt._static_data._all_search_urls[i][0]; ++i)
        ComboSearchUrl.AddString(_opt._static_data._all_search_urls[i]);
    ComboSearchUrl.SetCurSel(_opt._static_data._url_index);
    if(!_mode_learn)
        BtnSyns.EnableWindow(_opt._static_data._vocab_auto == 0), BtnAddWord.EnableWindow(_opt._static_data._vocab_auto == 0), BtnVocabWebster.EnableWindow(_opt._static_data._vocab_auto == 0), BtnPauseContinue.EnableWindow(_opt._static_data._vocab_auto == 1), BtnMoveForward.EnableWindow(_opt._static_data._vocab_auto == 1);
    return TRUE;
}

void CEnglishTrainingDlg::OnPaint(){
    if(IsIconic()){
        CPaintDC dc(this); // device context for painting
        SendMessage(WM_ICONERASEBKGND,reinterpret_cast<WPARAM>(dc.GetSafeHdc()),0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x,y,m_hIcon);
    }else
        CDialogEx::OnPaint();
}

HCURSOR CEnglishTrainingDlg::OnQueryDragIcon(){ return static_cast<HCURSOR>(m_hIcon); }

void CEnglishTrainingDlg::read_source_file(){
    //if(!IsFileExists(_source_file))
    //    if(!DownloadFileSynch("http://www.pokutan.com/tmp/words.txt",_source_file))
    //        return;
    std::stringstream ss;
    std::ifstream ifs(_source_file);
    _words_map.clear();
    _syns.clear();
    _last_eng_word.clear();
    if(ifs.is_open()){
        ss << ifs.rdbuf();
        string s = ss.str();
        if(!(s.size() % sizeof(wchar_t))){
            wstring ws;
            ws.resize(s.size() / sizeof(wchar_t));
            std::memcpy(&ws[0], isalpha(s[0]) ? s.c_str() : s.c_str() + sizeof(wchar_t), isalpha(s[0]) ?  s.size() : s.size() - 1);
            for(;;){
                bool f = true;
                size_t seprtr_idx = ws.find(L';');
                size_t new_line_idx = ws.find(L"\r\n");
                if(new_line_idx == wstring::npos){
                    new_line_idx = ws.length();
                    f = false;
                }
                if(ws[0] != L'#')
                    parse_and_insert_str(ws, new_line_idx, false);
                if(!f)
                    break;
                ws = &ws[new_line_idx + 2];
            }
            _it_lasting = _words_map.begin();
        }
        ifs.close();
    }
    merge_word_maps();
    _most_active_words_map.clear();
    _rnd->set_range(0,_words_map.size());
}

void CEnglishTrainingDlg::merge_word_maps(){
    list<wstring> to_del;
    MAP_CIT it;
    for(it = _words_map.begin(); it != _words_map.end(); ++it)
        if(_most_active_words_map.find(it->first) != _most_active_words_map.end())
            to_del.push_back(it->first);
    for(list<wstring>::iterator i = to_del.begin(); i != to_del.end(); ++i)
        if((it = _words_map.find(*i)) != _words_map.end())
        _words_map.erase(it);
    to_del.clear();
}

void CEnglishTrainingDlg::vocab_mode_next_word(){
    ASSERT(!_mode_learn);
    bool base_map = CheckBoxUsePreferMap.GetCheck()!=BST_CHECKED;
    if(!base_map && !_most_active_words_map.size())
        base_map = true;
    MAP_CIT it = base_map ? _words_map.end() : _most_active_words_map.end();
    if(_random_pair)
        it = get_random_pair(base_map);
    else{
        if(_it_lasting == _words_map.end()){
            if(AfxMessageBox(L"Reached end of words list. Start again?", MB_YESNO) != IDYES)
                return;
            // TODO
            _it_lasting = _words_map.begin();
        }
        it = _it_lasting;
        if(++_it_lasting == _words_map.end()){
            if(_vocab_auto_timer != -1){
                KillTimer(_vocab_auto_timer);
                _vocab_auto_timer = -1;
            }
            bool cont = AfxMessageBox(L"Reached end of words list. Start again?", MB_YESNO) == IDYES;
            if(cont)
                _it_lasting = _words_map.begin();
            wchar_t s[2]={};
            BtnPauseContinue.GetWindowTextW(s, 2);
            if(cont && s[0] == L'p')
                _vocab_auto_timer = SetTimer(2, _opt.to(), NULL);
            if(!cont)
                return;
        }
    }
    wstring s(it->first);
    s += L" - ";
    s += it->second;
    Stat_Result.SetWindowTextW(s.c_str());
    _curr_pair = MAP_PAIR(it->first, it->second);
}

void CEnglishTrainingDlg::OnTimer(UINT_PTR nIDEvent){
    if(nIDEvent == _my_timer){
        KillTimer(_my_timer);
        _my_timer = -1;
        fill_ui_data(true);
        ShowWindow(SW_SHOW);
    }else if(nIDEvent == _vocab_auto_timer){
        if(!_mode_learn)
            vocab_mode_next_word();
        else{
            KillTimer(_vocab_auto_timer);
            _vocab_auto_timer = -1;
        }
    }
    CDialogEx::OnTimer(nIDEvent);
}

void CEnglishTrainingDlg::OnBnClickedBtnSubmit(){
    wchar_t curr_translation[MAX_PATH]={};
    size_t len = Translations.GetWindowTextW(curr_translation, MAX_PATH);
    if(!curr_translation[0])
        return;
    locale loc;
    //for(string::size_type n = 0; n < len; ++n)
    //    curr_translation[n] = tolower(curr_translation[n], loc);
    MAP_IT it;
    int rus_to_eng = _mode_learn ? !_rus2eng_learn : _opt._static_data._vocab_from_rus2eng;
    Stat_Result.SetWindowTextW(_mode_learn ? L"Choose Translation" : L"Choose Word");
    switch(check_translation(curr_translation, rus_to_eng)){
    case 1:
        _last_eng_word = _curr_pair.first;
        if(!_mode_learn){
            wstring s = _curr_pair.first + L" - " + _curr_pair.second;
            Stat_Result.SetWindowTextW(s.c_str());
        }else{
            ShowWindow(SW_HIDE);
            wstring src(_curr_pair.first);
            src += L" - ";
            src += _curr_pair.second;
            PrevTranslation.SetWindowTextW(src.c_str());
            if(_opt.to() != -1)
                _my_timer = SetTimer(1, _opt.to(), NULL);
            _try_counter = 1;
            _curr_cue_idx = 0;
        }
        break;
    case 2:
        Translations.SetCurSel(Translations.FindStringExact(-1, _curr_right_transl.c_str()));
        break;
    case 0:
        // after 3 wrong tries we start to suggest translation
        if(_mode_learn){
            if(++_try_counter <= 3)
                _text_err = L"Wrong! Try Again: ";
            else if(_curr_cue_idx < _curr_right_transl.length())
                _text_err += _curr_right_transl[_curr_cue_idx++];
            Stat_Result.SetWindowTextW(_text_err.c_str());
        }
    }
}

void CEnglishTrainingDlg::OnCbnSelchangeComboTo(){
    wchar_t s[10]={};
    ComboTO.GetWindowTextW(s, 10);
    _opt.set_show_timeout(s);
    if(_vocab_auto_timer != -1){
        KillTimer(_vocab_auto_timer);
        _vocab_auto_timer = SetTimer(2, _opt.to(), NULL);
    }
}

void CEnglishTrainingDlg::OnBnClickedBtnEditfile(){
    ShellExecuteA(m_hWnd, "edit", _source_file, NULL, NULL, SW_SHOWNA);
}

void CEnglishTrainingDlg::OnBnClickedBtnReload(){
    Stat_Result.SetWindowTextW(_mode_learn ? L"Choose Translation" : L"Choose word from combo");
    PrevTranslation.SetWindowTextW(L"");
    read_source_file();
    fill_ui_data(false, true);
    Translations.SetFocus();
}

void CEnglishTrainingDlg::OnBnClickedRadioVocab(){
    PrevTranslation.SetWindowTextW(L"Vocabulary mode");
    CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO2);
    CheckTranslateFromEng.EnableWindow(TRUE);
    CheckBoxAuto.EnableWindow(TRUE);
    if(_opt._static_data._vocab_auto){
        if(_opt.to() != -1 && _vocab_auto_timer == -1){
            if(_it_lasting == _words_map.end())
                _it_lasting = _words_map.begin();
            _vocab_auto_timer = SetTimer(2, _opt.to(), NULL);
        }
    }
    if(CWnd* edit_combo = Translations.GetWindow(GW_CHILD)){
        Translations.ModifyStyle(CBS_DROPDOWN, CBS_DROPDOWNLIST);
        edit_combo->SendMessage(EM_SETREADONLY ,TRUE ,0);
    }
    _mode_learn = false;
    Stat_Result.SetWindowTextW(L"Choose word from combo");
    SourceWord.SetWindowTextW(L"");
    BtnSyns.EnableWindow(_opt._static_data._vocab_auto == 0), BtnAddWord.EnableWindow(_opt._static_data._vocab_auto == 0), BtnVocabWebster.EnableWindow(_opt._static_data._vocab_auto == 0), BtnPauseContinue.EnableWindow(_opt._static_data._vocab_auto == 1), BtnMoveForward.EnableWindow(_opt._static_data._vocab_auto == 1);
    fill_ui_data(true);
    KillTimer(_my_timer);
}

void CEnglishTrainingDlg::OnBnClickedRadioLearn(){
    CheckRadioButton(IDC_RADIO1,IDC_RADIO2,IDC_RADIO1);
    CheckTranslateFromEng.EnableWindow(FALSE);
    CheckBoxAuto.EnableWindow(FALSE);
    if(CWnd* edit_combo = Translations.GetWindow(GW_CHILD)){
        Translations.ModifyStyle(CBS_DROPDOWNLIST, CBS_DROPDOWN);
        edit_combo->SendMessage(EM_SETREADONLY, FALSE, 0);
    }
    _mode_learn = true;
    Stat_Result.SetWindowTextW(L"Choose Translation");
    SourceWord.SetWindowTextW(L"");
    PrevTranslation.SetWindowTextW(L"");
    fill_ui_data(true);
    BtnSyns.EnableWindow(TRUE), BtnAddWord.EnableWindow(TRUE), BtnVocabWebster.EnableWindow(TRUE), BtnPauseContinue.EnableWindow(FALSE), BtnMoveForward.EnableWindow(FALSE);
}

void CEnglishTrainingDlg::open_url(URLS url_index_){
    if(url_index_ < url_vocab && !_curr_pair.first.length() && _mode_learn)
        return;
    string url;
    char* s = nullptr;
    auto trim = [](char* s_){
        char* p = nullptr;
        if(p = strchr(s_, ','))*p = '\0';
        if(p = strchr(s_, ' '))*p = '\0';
        if(p = strchr(s_, '('))*p = '\0';
        if(p = strchr(s_, '{'))*p = '\0';
        if(p = strchr(s_, '-'))*p = '\0';
    };
    auto build_url = [&](wstring& s_){
        if(s_.length()){
            url = _urls[url_index_];
            size_t len = s_.length() + 1;
            s = new char[len];
            WC2MB(s_.c_str(), s, len, CP_ACP);
            trim(s);
            url += s;
            delete[] s;
        }
    };
    if(url_index_ == url_synonym || url_index_ == url_youglish){
        build_url(url_index_ == url_synonym ? _curr_pair.first : _last_eng_word);
    }else if(url_index_ != url_vocab){
        if(_opt.is_auto() && !_mode_learn){
            wchar_t s[MAX_PATH]={};
            Stat_Result.GetWindowTextW(s, MAX_PATH);
            if(wchar_t* p = wcschr(s, L' '))
                *p = L'\0';
            _last_eng_word = s;
        }
        if(_last_eng_word.length()){
            size_t len = _last_eng_word.length() + 1;
            s = new char[len];
            WC2MB(_last_eng_word.c_str(), s, len, CP_ACP);
            trim(s);
            url = _urls[url_index_];
            if(url_index_ != url_bbc)
                url += s;
            else{
                wchar_t url_w[MAX_PATH]={};
                char url_a[MAX_PATH]={};
                size_t n = MAX_PATH;
                ComboSearchUrl.GetWindowTextW(url_w, MAX_PATH);
                if(!WC2MB(url_w, url_a, n, CP_ACP))
                    return;
                find_and_replace(url, "_{site}_", url_a);
                find_and_replace(url, "{_word_}", s);
            }
            delete[] s;
        }
    }else
        url = _urls[url_index_];
    if(url.length())
        ShellExecuteA(m_hWnd, "open", url.c_str(), NULL, NULL, SW_SHOWNA);
}

void CEnglishTrainingDlg::OnBnClickedBtnPrononce(){ open_url(url_prononce); }

void CEnglishTrainingDlg::OnBnClickedBtnExamp(){ open_url(url_webster); }

void CEnglishTrainingDlg::OnBnClickedBtnHelp(){
    Stat_Result.SetWindowTextW(_curr_right_transl.c_str());
    Translations.SetCurSel(Translations.FindStringExact(0, _curr_right_transl.c_str()));
    wchar_t src[MAX_PATH]={};
    SourceWord.GetWindowTextW(src, MAX_PATH);
    _most_active_words_map.insert(_curr_pair);
    _words_map.erase(_curr_pair.first);
    CheckBoxUsePreferMap.EnableWindow(TRUE);
    Translations.SetFocus();
}

void CEnglishTrainingDlg::OnBnClickedBtnSetFile(){
    CFileDialog fd(TRUE);
    fd.DoModal();
    OPENFILENAME ofn = fd.GetOFN();
    if(ofn.lpstrFile[0]){
        WideCharToMultiByte(CP_ACP,0,ofn.lpstrFile,-1,_source_file, MAX_PATH,NULL,NULL);
        OnBnClickedBtnReload();
    }
}

void CEnglishTrainingDlg::OnDestroy(){
    CDialogEx::OnDestroy();
    if(_my_timer != -1)KillTimer(_my_timer);
    if(-_vocab_auto_timer != -1)KillTimer(_vocab_auto_timer);
    if(_rnd)delete _rnd;
    //UnloadKeyboardLayout(_eng_kbd);
    //UnloadKeyboardLayout(_rus_kbd);
    RECT rc;
    GetWindowRect(&rc);
    _opt._static_data._left = rc.left;
    _opt._static_data._top = rc.top;
    _opt._static_data._regime = _mode_learn ? OPTIONS::A::APP_REGIME::_study_ : OPTIONS::A::APP_REGIME::_vocab_;
    for(int i = 0; i < 20; ++i){
        if(!ComboSearchUrl.GetCount())
            break;
        ComboSearchUrl.SetCurSel(0);
        ComboSearchUrl.GetWindowTextW(_opt._static_data._all_search_urls[i], MAX_PATH);
        ComboSearchUrl.DeleteString(0);
    }
}

void CEnglishTrainingDlg::OnBnClickedBtnDict(){ open_url(url_vocab); }

void CEnglishTrainingDlg::OnBnClickedCheckFromEngToRus(){
    _opt._static_data._vocab_from_rus2eng = (CheckTranslateFromEng.GetCheck() == BST_CHECKED) ? 1 : 0;
    fill_ui_data(true);
}

void CEnglishTrainingDlg::OnBnClickedCheckAuto(){
    _opt._static_data._vocab_auto = (CheckBoxAuto.GetCheck() == BST_CHECKED) ? 1 : 0;
    ASSERT(!_mode_learn);
    BtnSyns.EnableWindow(_opt._static_data._vocab_auto == 0), BtnAddWord.EnableWindow(_opt._static_data._vocab_auto == 0), BtnVocabWebster.EnableWindow(_opt._static_data._vocab_auto == 0), BtnPauseContinue.EnableWindow(_opt._static_data._vocab_auto == 1), BtnMoveForward.EnableWindow(_opt._static_data._vocab_auto == 1);
    if(_opt._static_data._vocab_auto){
        ASSERT(_vocab_auto_timer == -1);
        MAP_IT it = _words_map.end();
        if(_opt.to() != -1){
            MAP_IT it = _random_pair ? get_random_pair(true) : _words_map.begin();
            wstring s(it->first);
            s += L" - ";
            s += it->second;
            Stat_Result.SetWindowTextW(s.c_str());
            _vocab_auto_timer = SetTimer(2, _opt.to(), NULL);
        }
    }else{
        ASSERT(_vocab_auto_timer != -1);
        Stat_Result.SetWindowTextW(L"Choose word from combo");
        KillTimer(_vocab_auto_timer);
        _vocab_auto_timer = -1;
    }
}

void CEnglishTrainingDlg::OnDblclkStatPrev(){ open_url(url_examples); }

void CEnglishTrainingDlg::OnStnDblclickStatRes(){ open_url(url_bbc); }

void CEnglishTrainingDlg::OnStnDblclickStatSorceWord(){ open_url(url_youglish); }

void CEnglishTrainingDlg::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized){
    CDialogEx::OnActivate(nState,pWndOther,bMinimized);
    Translations.SetFocus();
}

void CEnglishTrainingDlg::OnBnClickedBtnClearWord(){
    MAP_IT it = _words_map.find(_curr_pair.first);
    if(it != _words_map.end())
        _words_map.erase(it);
    it = _most_active_words_map.find(_curr_pair.first);
    if(it != _most_active_words_map.end()){
        _most_active_words_map.erase(it);
        if(!_most_active_words_map.size())
            CheckBoxUsePreferMap.EnableWindow(FALSE);
    }
    fill_ui_data(false);
    Translations.SetFocus();
}

void CEnglishTrainingDlg::OnBnClickedCheckOntop(){ SetWindowPos(CheckOnTop.GetCheck() == BST_CHECKED ? &wndTopMost : &wndNoTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE); }

void CEnglishTrainingDlg::OnBnClickedBtnPlusWord(){
    CString s;
    Translations.GetWindowText(s);
    parse_and_insert_str(wstring(s.GetBuffer()), s.GetLength(), true);
    fill_ui_data(false);
}

void CEnglishTrainingDlg::OnBnClickedBtnSyns(){
    if(!_curr_pair.first.length())
        return;
    wstring syns;
    MAP_IT it = _syns.find(_curr_pair.first);
    if(it != _syns.end()){
        syns = it->second;
        syns += L"\nCheck synonyms online?";
        if(MessageBox(syns.c_str(), L"Synonym found", MB_YESNO) == IDYES)
            open_url(url_synonym);
    }
}

void CEnglishTrainingDlg::OnBnClickedCheckRandom(){ _opt.set_rand(_random_pair = CheckBoxRandom.GetCheck() == BST_CHECKED); }

void CEnglishTrainingDlg::OnCbnKillfocusComboSiteUrl(){
    //ActivateKeyboardLayout(_eng_kbd, KLF_SETFORPROCESS);
    //auto a = generic_guard::make_scope_exit([&](){
    //    ActivateKeyboardLayout(_mode_learn ? (_rus2eng_learn ? _rus_kbd : _eng_kbd) : (!_opt._static_data._vocab_from_rus2eng ? _rus_kbd : _eng_kbd), KLF_SETFORPROCESS); });
    //wchar_t s[MAX_PATH]={};
    //if(!ComboSearchUrl.GetWindowTextW(s, MAX_PATH)){
    //    ComboSearchUrl.DeleteString(ComboSearchUrl.FindStringExact(0, _opt.url()));
    //    ComboSearchUrl.SetCurSel(0);
    //    if(ComboSearchUrl.GetWindowTextW(s, MAX_PATH))
    //        _opt.set_search_url(s);
    //    return;
    //}
    //// trim possible prefixes: http, //, www...
    //wchar_t* p = wcsstr(s, L"www.");
    //if(p)p += 4;
    //else if(p = wcsstr(s, L"://"))p += 3;
    //else p = s;
    //if(!wcschr(p, L'.')){
    //    EditURL.SetWindowTextW(_opt.url());
    //    return;
    //}
    //int index = ComboSearchUrl.FindString(0, p);
    //if(index > -1)
    //    ComboSearchUrl.SetCurSel(index);
    //else
    //    ComboSearchUrl.SetCurSel(ComboSearchUrl.AddString(p));
    //_opt.set_search_url(p);
    _opt._static_data._url_index = ComboSearchUrl.GetCurSel();
}

void CEnglishTrainingDlg::OnBnClickedBtnPause(){
    static bool running = true;
    if(running && _vocab_auto_timer != -1){
        KillTimer(_vocab_auto_timer);
        _vocab_auto_timer = -1;
    }else if(_vocab_auto_timer == -1)
        _vocab_auto_timer = SetTimer(2, _opt.to(), NULL);
    running = !running;
    BtnPauseContinue.SetWindowTextW(running ? L"p" : L"c");
}

void CEnglishTrainingDlg::OnBnClickedBtnForw(){ vocab_mode_next_word(); }

void CEnglishTrainingDlg::OnBnClickedBtnAddToMost(){
    if(_curr_pair.first.length()){
        _most_active_words_map.insert(_curr_pair);
        _words_map.erase(_curr_pair.first);
        CheckBoxUsePreferMap.EnableWindow(TRUE);
    }
}

void CEnglishTrainingDlg::OnBnClickedCheckUsePreferMap(){
}

void CEnglishTrainingDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
    if(nChar!=VK_ESCAPE)
        CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CEnglishTrainingDlg::PreTranslateMessage(MSG* pMsg){ return (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) ? TRUE : CDialogEx::PreTranslateMessage(pMsg); }

void CEnglishTrainingDlg::OnBnClickedBtnApphelp(){
    if(CDialog* p = new CDialog(ID_DLG_HELP, this)){
        p->DoModal();
        delete p;
    }
}
