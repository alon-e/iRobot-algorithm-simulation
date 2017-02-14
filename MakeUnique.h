#ifndef MAKE_UNIQUE_H_
#define MAKE_UNIQUE_H_

#include <memory>
#include <type_traits>

using namespace std;

template <typename T, typename... Args>
unique_ptr<T> make_unique_helper(false_type, Args&&... args) { return unique_ptr<T>(new T(forward<Args>(args)...)); }

template <typename T, typename... Args>
unique_ptr<T> make_unique_helper(true_type, Args&&... args) {
   static_assert(extent<T>::value == 0, "make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");

   typedef typename remove_extent<T>::type U;
   return unique_ptr<T>(new U[sizeof...(Args)]{forward<Args>(args)...});
}

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) { return make_unique_helper<T>(is_array<T>(), forward<Args>(args)...); }

#endif /* MAKE_UNIQUE_H_ */