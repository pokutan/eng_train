#pragma once

#include <map>
#include <vector>
#include <string>
using std::string;
using std::wstring;
using std::map;
#include "afxwin.h"
#include "gen_random.h"

#define ONE_SEC 1000
struct OPTIONS{
    struct A{
        long _left, _top;
        size_t _to;
        int _vocab_from_rus2eng, _vocab_auto, _is_random, _url_index;
        enum APP_REGIME{ _study_ = 1, _vocab_ }_regime;
        wchar_t _all_search_urls[20][MAX_PATH];
        A() : _left(50), _top(100), _to(60*ONE_SEC), _regime(_study_), _url_index(0){ ZeroMemory(_all_search_urls, 20 * MAX_PATH * sizeof(wchar_t)); }
    }_static_data;
    long left()const{ return _static_data._left; }
    long top()const{ return _static_data._top; }
    size_t to()const{ return _static_data._to; }
    bool rand()const{ return _static_data._is_random==1; }
    bool is_auto()const{ return _static_data._vocab_auto==1; }
    A::APP_REGIME regime()const{ return _static_data._regime; }
    map<int, wstring> _timeouts;
    HANDLE _cfg_file;
    OPTIONS();
    ~OPTIONS();
    void set_show_timeout(wchar_t const* to_);
    void set_rand(bool rand_);
};

// CEnglishTrainingDlg dialog
class CEnglishTrainingDlg : public CDialogEx{
// Construction
public:
	CEnglishTrainingDlg(CWnd* pParent = NULL);

// Dialog Data
	enum { IDD = IDD_ENGLISHTRAINING_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
#ifdef _DEBUG
    #define OPT_FILENAME L"\\eng_train_d.cfg"
#else
    #define OPT_FILENAME L"\\eng_train.cfg"
#endif
    string _caption;
    OPTIONS _opt;
    typedef std::pair<wstring, wstring> MAP_PAIR;
    typedef map<wstring, wstring> WORDS_MAP;
    typedef WORDS_MAP::iterator MAP_IT;
    typedef WORDS_MAP::const_iterator MAP_CIT;
    WORDS_MAP _words_map;
    WORDS_MAP _most_active_words_map;
    WORDS_MAP _syns;
    MAP_PAIR _curr_pair;
    MAP_CIT _it_lasting;
    char _source_file[MAX_PATH];
    bool _random_pair;
    void read_source_file();
    void fill_main_combo(int rus_);
    MAP_IT get_random_pair(_In_ bool main_dict_, _In_opt_ bool reset_ = false){
        //MAP_IT i = _words_map.begin();
        //for(; i != _words_map.end(); ++i)
        //    if(i->second == L"1. скромный, смиренный; 2. простой, бедный")
        //        break;
        //return i;
        //return _words_map.find(L"indecisive");
//        _most_active_words_map.insert(MAP_PAIR(std::wstring(L"help"), wstring(L"oooooooooooooo")));
//        static bool f = false;
//        if(f){ return _most_active_words_map.find(L"help"); }
//        f=true;
        auto _get_rnd_ = [&](map<wstring, wstring>& words_)->MAP_IT{
            _rnd->set_range(0, words_.size());
            int r = _rnd->rand();
            MAP_IT it = words_.begin();
            for(int i = 0; i < r; ++i)++it;
            return it;
        };
        auto _get_next_ = [&](_In_ bool main_map_)->MAP_IT{
            static MAP_IT it_main = _words_map.begin(), it_most = _most_active_words_map.begin();
            if(reset_)
                it_main = _words_map.begin(), it_most = _most_active_words_map.begin();
            if(!main_map_ && !_most_active_words_map.size())
                main_map_ = true;
            if(main_map_){
                if(it_main == _words_map.end())
                    it_main = _words_map.begin();
                return it_main++;
            }
            if(it_most == _most_active_words_map.end())
                it_most = _most_active_words_map.begin();
            return it_most++;
        };
        return _random_pair ? ((!main_dict_ && _most_active_words_map.size()) ? _get_rnd_(_most_active_words_map) : _get_rnd_(_words_map)) : _get_next_(main_dict_);
    }
    void fill_ui_data(_In_ bool update_prev_, _In_opt_ bool reset_ = false);
    void fill_to_combo();
    void parse_and_insert_str(wstring& ws, size_t end_ofLine_idx_, bool also_help_map_);
    void find_and_replace(std::string& src_, char const* find_what_, char const* replace_by_);
    // returns:
    // 0 - not found; 1 - found exact translation; 2 - found substring
    int check_translation(const wstring& user_transl_, int rus_2_eng_);
    enum URLS{
        url_prononce = 1,
        url_webster,
        url_examples,
        url_synonym,
        url_bbc,
        url_vocab // keep it last
    };
    void vocab_mode_next_word();
    void open_url(URLS url_index_);
    int _my_timer, _vocab_auto_timer;
    int _rus2eng_learn;
    wstring _text_err, _curr_right_transl;
    wstring _last_eng_word;
    wchar_t _comp_name[MAX_PATH];
    std::map<URLS, string> _urls;
    bool _mode_learn;
    HKL _eng_kbd, _rus_kbd;
    gen_random<int>* _rnd;
public:
    CStatic SourceWord, Stat_Result, PrevTranslation;
    CComboBox Translations, ComboTO, ComboSearchUrl;
    CButton RadioLearn, RadioChoose, CheckTranslateFromEng, CheckOnTop, CheckBoxRandom, CheckBoxAuto;
    CButton BtnSyns, BtnForgetWord, BtnAddWord, BtnPrononc, BtnVocabWebster, BtnVocabMueller, BtnPauseContinue, BtnMoveForward, CheckUsePreferMap;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedBtnSubmit();
    afx_msg void OnCbnSelchangeComboTo();
    afx_msg void OnBnClickedBtnEditfile();
    afx_msg void OnBnClickedBtnReload();
    afx_msg void OnBnClickedRadioVocab();
    afx_msg void OnBnClickedRadioLearn();
    afx_msg void OnBnClickedBtnPrononce();
    afx_msg void OnBnClickedBtnExamp();
    afx_msg void OnBnClickedBtnHelp();
    afx_msg void OnBnClickedBtnSetFile();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedBtnDict();
    afx_msg void OnBnClickedCheckFromEngToRus();
    afx_msg void OnDblclkStatPrev();
    afx_msg void OnStnDblclickStatSorceWord();
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnStnDblclickStatRes();
    afx_msg void OnBnClickedBtnClearWord();
    afx_msg void OnBnClickedCheckOntop();
    afx_msg void OnBnClickedBtnPlusWord();
    afx_msg void OnBnClickedBtnSyns();
    afx_msg void OnBnClickedCheckRandom();
    afx_msg void OnCbnKillfocusComboSiteUrl();
    afx_msg void OnBnClickedCheckAuto();
    afx_msg void OnBnClickedBtnPause();
    afx_msg void OnBnClickedBtnForw();
    afx_msg void OnBnClickedBtnAddToMost();
    afx_msg void OnBnClickedCheckUsePreferMap();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
