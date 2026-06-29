/*!
 * UFOMap: An Efficient Probabilistic 3D Mapping Framework That Embraces the Unknown
 *
 * @author Daniel Duberg (dduberg@kth.se)
 * @see https://github.com/UnknownFreeOccupied/ufomap
 * @version 1.0
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022, Daniel Duberg, KTH Royal Institute of Technology
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Daniel Duberg, KTH Royal Institute of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UFO_CONTAINER_TREE_DATA_HPP
#define UFO_CONTAINER_TREE_DATA_HPP

// UFO
#include <ufo/compute/compute.hpp>
#include <ufo/container/tree/container.hpp>
#include <ufo/container/tree/index.hpp>
#include <ufo/utility/type_traits.hpp>

// STL
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <utility>

namespace ufo
{
template <class Derived, class... Ts>
class TreeData
{
 public:
	using Data = TreeContainer<Ts...>;

	using Index    = TreeIndex;
	using pos_t    = Index::pos_t;
	using offset_t = Index::offset_t;

	static constexpr std::size_t const NumBuffers = sizeof...(Ts);

 public:
	~TreeData() { gpuRelease(); }

	[[nodiscard]] Data& data() { return data_; }

	[[nodiscard]] Data const& data() const { return data_; }

	/*!
	 * @brief Checks if a block is valid.
	 *
	 * @param block the block to check
	 * @return `true` if the block is valid, `false` otherwise.
	 */
	[[nodiscard]] bool valid(pos_t block) const { return data_.capacity() > block; }

	bool gpuInit(WGPUPowerPreference power_preference = WGPUPowerPreference_HighPerformance,
	             WGPUBackendType     backend_type     = WGPUBackendType_Undefined)
	{
		if (nullptr != device_) {
			return false;
		}

		instance_ = compute::createInstance();
		adapter_ = compute::createAdapter(instance_, nullptr, power_preference, backend_type);
		auto required_limits = requiredLimits(adapter_);
		device_              = compute::createDevice(adapter_, &required_limits);
		queue_               = compute::queue(device_);

		return true;
	}

	bool gpuInit(WGPULimits const&   required_limits,
	             WGPUSurface         compatible_surface = nullptr,
	             WGPUPowerPreference power_preference = WGPUPowerPreference_HighPerformance,
	             WGPUBackendType     backend_type     = WGPUBackendType_Undefined)
	{
		if (nullptr != device_) {
			return false;
		}

		instance_ = compute::createInstance();
		adapter_  = compute::createAdapter(instance_, compatible_surface, power_preference,
		                                   backend_type);
		device_   = compute::createDevice(adapter_, &required_limits);
		queue_    = compute::queue(device_);

		return true;
	}

	bool gpuInit(WGPUAdapter adapter)
	{
		assert(nullptr != adapter);

		return gpuInit(adapter, requiredLimits(adapter));
	}

	bool gpuInit(WGPUAdapter adapter, WGPULimits const& required_limits)
	{
		if (nullptr != device_) {
			return false;
		}

		assert(nullptr != adapter);

		// Increase reference count
		wgpuAdapterAddRef(adapter);

		adapter_ = adapter;
		device_  = compute::createDevice(adapter, &required_limits);
		queue_   = compute::queue(device_);

		return true;
	}

	bool gpuInit(WGPUDevice device)
	{
		if (nullptr != device_) {
			return false;
		}

		assert(nullptr != device);

		// Increase reference count
		wgpuDeviceAddRef(device);

		device_ = device;
		queue_  = compute::queue(device);

		return true;
	}

	void gpuRelease()
	{
		for (auto& buffers : buffers_) {
			for (WGPUBuffer& buf : buffers) {
				if (nullptr != buf) {
					wgpuBufferRelease(buf);
				}
			}
			buffers.clear();
		}

		if (nullptr != queue_) {
			wgpuQueueRelease(queue_);
			queue_ = nullptr;
		}

		if (nullptr != device_) {
			wgpuDeviceRelease(device_);
			device_ = nullptr;
		}

		if (nullptr != adapter_) {
			wgpuAdapterRelease(adapter_);
			adapter_ = nullptr;
		}

		if (nullptr != instance_) {
			wgpuInstanceRelease(instance_);
			instance_ = nullptr;
		}
	}

	[[nodiscard]] WGPUDevice gpuDevice() const { return device_; }

	[[nodiscard]] WGPUQueue gpuQueue() const { return queue_; }

	template <class T>
	[[nodiscard]] std::size_t gpuNumBuffers() const
	{
		return buffers_[index_v<T, Ts...>].size();
	}

	template <class T>
	[[nodiscard]] WGPUBuffer gpuBuffer(std::size_t index = 0) const
	{
		return buffers_[index_v<T, Ts...>][index];
	}

	template <class T>
	[[nodiscard]] std::size_t gpuBufferSize(std::size_t index = 0) const
	{
		return wgpuBufferGetSize(gpuBuffer<T>(index));
	}

	// template <class Predicate>
	// void gpuUpdateBuffers(Predicate const& pred)
	// {
	// 	// TODO: Implement
	// 	derived().onGpuUpdateBuffers(pred);
	// }

	bool gpuUpdate() { return (gpuUpdate<Ts>() | ...); }

	template <class T>
	bool gpuUpdate()
	{
		assert(nullptr != device_);
		assert(nullptr != queue_);

		std::size_t const size = data_.template serializedSize<T>();

		auto& buffers = buffers_[index_v<T, Ts...>];

		if (0 == size) {
			for (auto& buf : buffers) {
				wgpuBufferRelease(buf);
			}
			bool empty = buffers.empty();
			buffers.clear();
			return !empty;
		}

		constexpr std::size_t const bucket_size = Data::template serializedBucketSize<T>();

		std::size_t const buckets_per_buffer = max_buffer_size_ / bucket_size;
		std::size_t const buffer_size        = bucket_size * buckets_per_buffer;

		std::size_t const num_buffers = 1 + (size - 1) / buffer_size;

		buffers.reserve(num_buffers);

		bool updated = false;

		auto it   = data_.template beginBucket<T>();
		auto last = data_.template endBucket<T>();

		for (std::size_t i{}; num_buffers > i; ++i) {
			if (buffers.size() <= i) {
				updated = true;

				auto& buffer = buffers.emplace_back(compute::createBuffer(
				    device_, "", buffer_size, WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst,
				    true));

				assert(nullptr != buffer);

				void* buf = wgpuBufferGetMappedRange(buffer, 0, buffer_size);

				for (std::size_t i{}; buckets_per_buffer > i && it != last; ++i, ++it) {
					auto& [data, modified] = *it;

					std::memcpy(buf, data.data(), bucket_size);
					buf      = static_cast<void*>(static_cast<unsigned char*>(buf) + bucket_size);
					modified = false;
				}

				wgpuBufferUnmap(buffer);
			} else {
				WGPUBuffer& buffer = buffers[i];

				std::size_t offset = 0;
				for (std::size_t i{}; buckets_per_buffer > i && it != last; ++i, ++it) {
					auto& [data, modified] = *it;

					if (modified) {
						wgpuQueueWriteBuffer(queue_, buffer, offset, data.data(), bucket_size);
						modified = false;
					}
					offset += bucket_size;
				}
			}
		}

		// FIXME: Probably do not want to release them but instead reuse later when needed,
		// like how std::vector works. Need a function similar to `shrink_to_fit`. Also, need
		// to keep track of which ones are "active" and not.
		for (std::size_t i = num_buffers; buffers.size() > i; ++i) {
			updated = true;
			wgpuBufferRelease(buffers[i]);
		}
		buffers.resize(num_buffers);

		return updated;
	}

	void swap(TreeData& other)
	{
		using std::swap;
		swap(data_, other.data_);
		swap(instance_, other.instance_);
		swap(adapter_, other.adapter_);
		swap(device_, other.device_);
		swap(queue_, other.queue_);
		swap(buffers_, other.buffers_);
	}

 protected:
	[[nodiscard]] std::size_t size() const { return data_.size(); }

	void reserve(std::size_t cap) { data_.reserve(cap); }

	void clear() { data_.clear(); }

	[[nodiscard]] pos_t create() { return data_.create(); }

	[[nodiscard]] pos_t createThreadSafe() { return data_.createThreadSafe(); }

	void eraseBlock(pos_t block) { data_.eraseBlock(block); }

	template <class T>
	[[nodiscard]] T& data(pos_t block)
	{
		return data_.template get<T>(block);
	}

	template <class T>
	[[nodiscard]] T const& data(pos_t block) const
	{
		return data_.template get<T>(block);
	}

	template <class T>
	[[nodiscard]] T& data(TreeIndex node)
	{
		return data<T>(node.pos);
	}

	template <class T>
	[[nodiscard]] T const& data(TreeIndex node) const
	{
		return data<T>(node.pos);
	}

 private:
	[[nodiscard]] WGPULimits requiredLimits(WGPUAdapter adapter)
	{
		WGPULimits required  = WGPU_LIMITS_INIT;
		WGPULimits supported = WGPU_LIMITS_INIT;

		wgpuAdapterGetLimits(adapter, &supported);

		// These two limits are different because they are "minimum" limits,
		// they are the only ones we may forward from the adapter's supported limits.
		required.minUniformBufferOffsetAlignment = supported.minUniformBufferOffsetAlignment;
		required.minStorageBufferOffsetAlignment = supported.minStorageBufferOffsetAlignment;

		max_buffer_size_ =
		    std::min(max_buffer_size_, static_cast<std::size_t>(supported.maxBufferSize));
		max_buffer_size_ =
		    std::min(max_buffer_size_,
		             static_cast<std::size_t>(supported.maxStorageBufferBindingSize));

		required.maxBufferSize               = max_buffer_size_;
		required.maxStorageBufferBindingSize = max_buffer_size_;

		required.maxComputeWorkgroupStorageSize    = 16352;
		required.maxComputeInvocationsPerWorkgroup = 256;
		required.maxComputeWorkgroupSizeX          = 256;
		required.maxComputeWorkgroupSizeY          = 256;
		required.maxComputeWorkgroupSizeZ          = 64;
		required.maxComputeWorkgroupsPerDimension  = 65535;

		required.maxUniformBuffersPerShaderStage = 12;
		required.maxUniformBufferBindingSize     = 65536;

		return required;
	}

 protected:
	Data data_;

	WGPUInstance                                    instance_ = nullptr;
	WGPUAdapter                                     adapter_  = nullptr;
	WGPUDevice                                      device_   = nullptr;
	WGPUQueue                                       queue_    = nullptr;
	std::array<std::vector<WGPUBuffer>, NumBuffers> buffers_{};

	std::size_t max_buffer_size_ = 1'073'741'824 / 2;
};

template <class Derived, class... Ts>
void swap(TreeData<Derived, Ts...>& lhs, TreeData<Derived, Ts...>& rhs)
{
	lhs.swap(rhs);
}
}  // namespace ufo

#endif  // UFO_CONTAINER_TREE_DATA_HPP