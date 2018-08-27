/**
 * Distributed under the MIT License (MIT)
 * Copyright (c) 2018 icekylin
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef DM_ANY_H
#define DM_ANY_H
#include <type_traits>
#include <typeinfo>
#include "error.h"

namespace dm {
class any {
public:
  class place_holder {
  public:
    virtual ~place_holder() {}
    virtual const std::type_info &type() const = 0;
    virtual place_holder *clone() const  = 0;
  };
  template <typename _Value_type>
  class holder : public place_holder {
  public:
    holder(const _Value_type &value) : held(value) { }
    virtual const std::type_info &type() const {
      return typeid(held);
    }
    virtual place_holder *clone() const {
      return new holder(held);
    }
  private:
    holder &operator=(const holder &);
  public:
    _Value_type held;
  };
public:
  any() : content(NULL) {}
  ~any() {
    delete content;
  }
  any(const any &other) :
      content(other.content ? other.content->clone() : NULL) {
  }

  template<typename _Value_type>
  any(const _Value_type &value)
      : content(new holder<typename std::remove_cv<_Value_type>::type>(value)) {
  }

  any &swap(any & rhs) {
    std::swap(content, rhs.content);
    return *this;
  }

  template<typename ValueType>
  any &operator=(const ValueType & rhs) {
    any(rhs).swap(*this);
    return *this;
  }

  const std::type_info &type() const {
    return content ? content->type() : typeid(void);
  }
private:
  template<typename _Value_type>
  friend _Value_type * any_cast(any *);

  template<typename _Value_type>
  friend _Value_type * unsafe_any_cast(any *);
public:
  place_holder *content;
};

template <typename _Value_type>
_Value_type *any_cast(any *operand) {
  return operand && operand->type() == typeid(_Value_type)
         ? &static_cast<any::holder<
              typename std::remove_cv<_Value_type>::type> *>(
                operand->content)->held : 0;
}

template <typename _Value_type>
inline const _Value_type *any_cast(const any *operand) {
  return any_cast<_Value_type>(const_cast<any *>(operand));
}

template <typename _Value_type>
_Value_type any_cast(any &operand) {
  typedef typename std::remove_reference<_Value_type>::type nonref;
  nonref *result = any_cast<nonref>(&operand);
  if (!result) throw dm_error("bad any cast");
  typedef typename std::conditional<std::is_reference<_Value_type>::value,
          _Value_type, typename std::add_lvalue_reference<
              _Value_type>::type>::type ref_type;
  return static_cast<ref_type>(*result);
}

template <typename _Value_type>
inline _Value_type any_cast(const any &operand) {
  typedef typename std::remove_reference<_Value_type>::type nonref;
  return any_cast<const nonref &>(const_cast<any &>(operand));
}

template <typename _Value_type>
inline _Value_type *unsafe_any_cast(any *operand) {
  return &static_cast<any::holder<_Value_type> *>(operand->content)->held;
}

template<typename _Value_type>
inline const _Value_type * unsafe_any_cast(const any * operand) {
  return unsafe_any_cast<_Value_type>(const_cast<any *>(operand));
}

template <unsigned N>
struct anys {
    typedef any Type[N > 0 ? N : 1];
};

}	  // namespace dm
#endif