/*
		Generic Scope Exit

        Usage example:
        HMODULE h = LoadLibraryW(L"abc.dll");
        if(!h)
            return;
        auto auto_free_lib = generic_guard::make_scope_exit( [h]() -> void { FreeLibrary(h); } );
        From now you may exit from current scope at any time, library will be freed automatically
*/

#pragma once

#include <memory>

#ifndef noexcept
    #define noexcept
#endif

#ifndef noexcept_p
    #define noexcept_p(a)
#endif

namespace generic_guard {
	template <typename EF> struct scope_exit {
		explicit scope_exit(EF&& f_) noexcept : _exit_function(std::move(f_)), _execute_on_destruction(true){}
		// move
		scope_exit(scope_exit&& rhs_) noexcept : _exit_function(std::move(rhs_._exit_function)), _execute_on_destruction(rhs_._execute_on_destruction){
			rhs.release();
		}
		// release
		~scope_exit() noexcept_p(noexcept_p(this->_exit_function())){
			if(_execute_on_destruction)
				this->_exit_function();
		}
		void release() noexcept {
			this->_execute_on_destruction = false;
		}
	private:
		scope_exit(scope_exit const&) = delete;
		void operator=(scope_exit const&) = delete;
		scope_exit& operator=(scope_exit&&) = delete;
		EF _exit_function;
		bool _execute_on_destruction; // exposition only
	};

	template <typename EF> scope_exit<EF> make_scope_exit(EF&& exit_func_) noexcept {
		return scope_exit<std::remove_reference_t<EF>>(std::forward<EF>(exit_func_));
	}

}
