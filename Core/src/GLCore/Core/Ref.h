#pragma once

#include <memory>
#include <utility>

namespace GLCore {

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename ... _Args>
	constexpr Ref<T> CreateRef(_Args && ... args)
	{
		return std::make_shared<T>(std::forward<_Args>(args) ...);
	}
}