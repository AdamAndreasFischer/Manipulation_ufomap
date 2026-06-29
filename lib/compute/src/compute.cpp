// UFO
#include <ufo/compute/compute.hpp>

// STL
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>

namespace ufo::compute
{
WGPUInstance createInstance(WGPUInstanceDescriptor const* descriptor)
{
	return wgpuCreateInstance(descriptor);
}

WGPUAdapter createAdapter(WGPUInstance instance, WGPUSurface compatible_surface,
                          WGPUPowerPreference power_preference,
                          WGPUBackendType backend_type, WGPUBool force_fallback_adapter)
{
	assert(nullptr != instance);

	WGPUAdapter adapter       = nullptr;
	bool        request_ended = false;

	WGPURequestAdapterOptions options = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
	options.powerPreference           = power_preference;
	options.compatibleSurface         = compatible_surface;
	options.backendType               = backend_type;
	options.forceFallbackAdapter      = force_fallback_adapter;

	WGPURequestAdapterCallbackInfo callback_info = WGPU_REQUEST_ADAPTER_CALLBACK_INFO_INIT;

	callback_info.callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter,
	                            WGPUStringView message, void* userdata1, void* userdata2) {
		if (WGPURequestAdapterStatus_Success == status) {
			*static_cast<WGPUAdapter*>(userdata1) = adapter;
		} else {
			std::cerr << "Could not get WebGPU adapter: " << message.data << std::endl;
		}
		*static_cast<bool*>(userdata2) = true;
	};
	callback_info.userdata1 = static_cast<void*>(&adapter);
	callback_info.userdata2 = static_cast<void*>(&request_ended);

	wgpuInstanceRequestAdapter(instance, &options, callback_info);

	// We wait until request_ended gets true
#ifdef __EMSCRIPTEN__
	while (!request_ended) {
		emscripten_sleep(100);
	}
#endif  // __EMSCRIPTEN__

	assert(request_ended);
	assert(nullptr != adapter);

	return adapter;
}

WGPUDevice createDevice(WGPUAdapter adapter, WGPULimits const* required_limits)
{
	assert(nullptr != adapter);

	WGPUDevice device        = nullptr;
	bool       request_ended = false;

	WGPUDeviceDescriptor desc = WGPU_DEVICE_DESCRIPTOR_INIT;
	// desc.label       = (window_name_ + " Device").c_str();
	// desc.requiredFeatureCount = 0;
	// desc.requiredFeatures     = nullptr;

	desc.requiredLimits = required_limits;

	// desc.defaultQueue.label       = (window_name_ + " Default Queue").c_str();

	// TODO: Fix
	// desc.deviceLostCallbackInfo.callback =
	//     [](WGPUDeviceImpl* const, WGPUDeviceLostReason reason, WGPUStringView message,
	//        void* /* userdata1 */, void* /* userdata2 */) {
	// 	    std::cout << "Device lost: reason " << reason;
	// 	    if (0 < message.length) {
	// 		    std::cout << " (" << message.data << ")";
	// 	    }
	// 	    std::cout << std::endl;
	//     };

	// desc.uncapturedErrorCallbackInfo.nextInChain = nullptr;
	// desc.uncapturedErrorCallbackInfo.userdata    = nullptr;
	// desc.uncapturedErrorCallbackInfo.callback    = nullptr;

	WGPURequestDeviceCallbackInfo callback_info = WGPU_REQUEST_DEVICE_CALLBACK_INFO_INIT;

	callback_info.callback = [](WGPURequestDeviceStatus status, WGPUDevice device,
	                            WGPUStringView message, void* userdata1, void* userdata2) {
		if (WGPURequestDeviceStatus_Success == status) {
			*static_cast<WGPUDevice*>(userdata1) = device;
		} else {
			std::cout << "Could not get WebGPU device: " << message.data << std::endl;
		}
		*static_cast<bool*>(userdata2) = true;
	};
	callback_info.userdata1 = static_cast<void*>(&device);
	callback_info.userdata2 = static_cast<void*>(&request_ended);

	wgpuAdapterRequestDevice(adapter, &desc, callback_info);

	// We wait until request_ended gets true
#ifdef __EMSCRIPTEN__
	while (!request_ended) {
		emscripten_sleep(100);
	}
#endif  // __EMSCRIPTEN__

	assert(request_ended);
	assert(nullptr != device);

	return device;
}

WGPUQueue queue(WGPUDevice device)
{
	assert(nullptr != device);
	return wgpuDeviceGetQueue(device);
}

std::size_t bufferPaddedSize(std::size_t size)
{
	static constexpr std::size_t const COPY_BUFFER_ALIGNMENT = 4;
	static constexpr std::size_t const align_mask            = COPY_BUFFER_ALIGNMENT - 1;
	return std::max((size + align_mask) & ~align_mask, COPY_BUFFER_ALIGNMENT);
}

std::size_t bufferPaddedSize(std::size_t width, std::size_t bytes_per_pixel)
{
	static constexpr std::size_t const COPY_BYTES_PER_ROW_ALIGNMENT = 256;

	std::size_t const unpadded_bytes_per_row = width * bytes_per_pixel;
	std::size_t const padded_bytes_per_row_padding =
	    (COPY_BYTES_PER_ROW_ALIGNMENT -
	     unpadded_bytes_per_row % COPY_BYTES_PER_ROW_ALIGNMENT) %
	    COPY_BYTES_PER_ROW_ALIGNMENT;
	return unpadded_bytes_per_row + padded_bytes_per_row_padding;
}

WGPUBuffer createBuffer(WGPUDevice device, std::string label, std::size_t size,
                        WGPUBufferUsage usage, bool mapped_at_creation)
{
	std::size_t padded_size = bufferPaddedSize(size);

	WGPUBufferDescriptor desc = WGPU_BUFFER_DESCRIPTOR_INIT;
	desc.label                = {label.c_str(), label.length()};
	desc.size                 = padded_size;
	desc.usage                = usage;
	desc.mappedAtCreation     = mapped_at_creation;
	return wgpuDeviceCreateBuffer(device, &desc);
}

WGPUBuffer createBufferInit(WGPUDevice device, std::string label, WGPUBufferUsage usage,
                            void* content, std::size_t content_size)
{
	if (0 == content_size) {
		return createBuffer(device, label, 0, usage, false);
	}

	WGPUBuffer buffer = createBuffer(device, label, content_size, usage, true);

	assert(nullptr != buffer);

	void* buf = wgpuBufferGetMappedRange(buffer, 0, content_size);
	std::memcpy(buf, content, content_size);
	wgpuBufferUnmap(buffer);

	return buffer;
}

WGPUShaderModule loadShaderModule(WGPUDevice device, std::filesystem::path const& path)
{
	std::ifstream file(path);
	if (!file.is_open()) {
		return nullptr;
	}

	file.seekg(0, std::ios::end);
	std::size_t size = file.tellg();
	std::string shader_source(size, ' ');
	file.seekg(0);
	file.read(shader_source.data(), size);

	WGPUShaderSourceWGSL shader_wgsl = WGPU_SHADER_SOURCE_WGSL_INIT;
	shader_wgsl.chain.sType          = WGPUSType_ShaderSourceWGSL;
	// FIXME: Should this be `shader_source.length()`?
	shader_wgsl.code = {shader_source.c_str(), WGPU_STRLEN};

	WGPUShaderModuleDescriptor shader_desc = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
	// TODO: shader_code_desc.label
	shader_desc.nextInChain = reinterpret_cast<WGPUChainedStruct const*>(&shader_wgsl);

	return wgpuDeviceCreateShaderModule(device, &shader_desc);
}
}  // namespace ufo::compute