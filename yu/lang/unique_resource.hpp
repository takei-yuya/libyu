// Copyright 2025 TAKEI Yuya (https://github.com/takei-yuya/)
#ifndef YU_LANG_UNIQUE_RESOURCE_HPP_
#define YU_LANG_UNIQUE_RESOURCE_HPP_

#include <utility>
#include <type_traits>

#if defined(__has_cpp_attribute)
#  if __has_cpp_attribute(no_unique_address)
#    define YU_NO_UNIQUE_ADDRESS [[no_unique_address]]
#  else
#    define YU_NO_UNIQUE_ADDRESS
#  endif
#  if __has_cpp_attribute(nodiscard)
#    define YU_NODISCARD [[nodiscard]]
#  else
#    define YU_NODISCARD
#  endif
#else
#  define YU_NO_UNIQUE_ADDRESS
#  define YU_NODISCARD
#endif

namespace yu {
namespace lang {

/**
 * @brief A RAII wrapper for a resource with a custom deleter.
 * The resource is released when the UniqueResource is destroyed.
 * @tparam T The type of the resource.
 * @tparam F The type of the deleter function/functor. It should be callable with a single argument of type T.
 */
template <typename T, typename F>
class UniqueResource {
 public:
  /**
   * @brief Constructs a UniqueResource that takes ownership of the given resource and deleter.
   * @param resource The resource to manage.
   * @param deleter The deleter function/functor to call when the resource is released.
   */
  UniqueResource(T resource, F deleter) noexcept
    : resource_(std::move(resource)), deleter_(std::move(deleter)), deleted_(false) {}
  /**
   * @brief Destructor. Calls the deleter on the resource if it has not been released.
   */
  ~UniqueResource() {
    reset();
  }

  // Movable
  UniqueResource(UniqueResource&& other) noexcept
    : resource_(std::move(other.resource_)),
      deleter_(std::move(other.deleter_)),
      deleted_(other.deleted_) {
    (void)other.release();
  }
  UniqueResource& operator=(UniqueResource&& other) noexcept {
    if (this != &other) {
      reset();
      resource_ = std::move(other.resource_);
      deleter_ = std::move(other.deleter_);
      deleted_ = other.deleted_;
      (void)other.release();
    }
    return *this;
  }

  // Non-copyable
  UniqueResource(const UniqueResource&) = delete;
  UniqueResource& operator=(const UniqueResource&) = delete;

  /**
   * @brief Releases the managed resource by calling the deleter if it has not been released yet.
   * After calling this function, the UniqueResource no longer manages the resource.
   */
  void reset() {
    if (!deleted_) {
      deleted_ = true;
      deleter_(resource_);
    }
  }

  /**
   * @brief Releases the current resource (if any) and takes ownership of a new resource.
   * @param resource The new resource to manage.
   */
  void reset(T resource) {
    reset();
    resource_ = std::move(resource);
    deleted_ = false;
  }

  /**
   * @brief Releases ownership of the managed resource without calling the deleter.
   * After calling this function, the UniqueResource no longer manages the resource.
   * @return The managed resource.
   */
  YU_NODISCARD T release() noexcept {
    deleted_ = true;
    return std::move(resource_);
  }

  /**
   * @brief Returns a const reference to the managed resource.
   * @return A const reference to the managed resource.
   */
  const T& get() const noexcept { return resource_; }

  /**
   * @brief Implicit conversion operator to const T&.
   * Allows using UniqueResource in places where a const T& is expected.
   * @return A const reference to the managed resource.
   */
  operator const T&() const noexcept { return resource_; }

  /**
   * @brief Returns a const reference to the deleter function/functor.
   * @return A const reference to the deleter function/functor.
   */
  const F& get_deleter() const noexcept { return deleter_; }

 private:
  T resource_;
  YU_NO_UNIQUE_ADDRESS F deleter_;
  bool deleted_;
};

template <typename T, typename F>
UniqueResource<T, F> make_unique_resource(T resource, F deleter) noexcept {
  return UniqueResource<T, F>(std::move(resource), std::move(deleter));
}

}  // namespace lang
}  // namespace yu

#undef YU_NODISCARD
#undef YU_NO_UNIQUE_ADDRESS

#endif  // YU_LANG_UNIQUE_RESOURCE_HPP_
