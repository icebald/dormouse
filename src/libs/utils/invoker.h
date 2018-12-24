//
// Created by icekylin on 18-12-24.
//

#ifndef DM_INVOKE_H
#define DM_INVOKE_H

#include <chrono>
#include <memory>
#include <tuple>
#include <cerrno>
#include <utility>
#include <bits/functexcept.h>
#include <bits/functional_hash.h>
#include <bits/invoke.h>
#include <bits/gthr.h>

namespace dm {
namespace util {

// A call wrapper that does INVOKE(forwarded tuple elements...)
template<typename _Tuple>
struct invoker {
    _Tuple _M_t;

    template<size_t _Index>
    static std::__tuple_element_t<_Index, _Tuple>&& _S_declval();

    template<size_t... _Ind>
    auto
    _M_invoke(std::_Index_tuple<_Ind...>)
    noexcept(noexcept(std::__invoke(_S_declval<_Ind>()...)))
    -> decltype(std::__invoke(_S_declval<_Ind>()...))
    { return std::__invoke(std::get<_Ind>(std::move(_M_t))...); }

    using _Indices
    = typename std::_Build_index_tuple<std::tuple_size<_Tuple>::value>::__type;

    auto
    operator()()
    noexcept(noexcept(std::declval<invoker&>()._M_invoke(_Indices())))
    -> decltype(std::declval<invoker&>()._M_invoke(_Indices()))
    { return _M_invoke(_Indices()); }
};

namespace invoke {
    template<typename... _Tp>
    using __decayed_tuple = std::tuple<typename std::decay<_Tp>::type...>;

    template<typename _Callable, typename... _Args>
    static invoker<__decayed_tuple<_Callable, _Args...>>
    __make_invoker(_Callable&& __callable, _Args&&... __args) {
        return { __decayed_tuple<_Callable, _Args...>{
            std::forward<_Callable>(__callable), std::forward<_Args>(__args)...
        } };
    }
}
}
}
#endif //DM_INVOKE_H
