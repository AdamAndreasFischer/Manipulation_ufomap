// UFO

// Catch2
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

// WebGPU
#include <webgpu/webgpu.h>

// WGPU-native
#include <webgpu/wgpu.h>

// STL
#include <array>

static void handleRequestAdapter(WGPURequestAdapterStatus /* status */,
                                 WGPUAdapter adapter, char const* /* message */,
                                 void*       userdata)
{
	*static_cast<WGPUAdapter*>(userdata) = adapter;
}

static void handleRequestDevice(WGPURequestDeviceStatus /* status */, WGPUDevice device,
                                char const* /* message */, void*                 userdata)
{
	*static_cast<WGPUDevice*>(userdata) = device;
}

static void handleBufferMap(WGPUBufferMapAsyncStatus status, void* /* userdata */)
{
	printf("[UFOCompute] buffer_map status=%#.8x\n", status);
}

std::string shader_code = R"(
@group(0)
@binding(0)
var<storage, read_write> v_indices: array<u32>; // this is used as both input and output for convenience

// The Collatz Conjecture states that for any integer n:
// If n is even, n = n/2
// If n is odd, n = 3n+1
// And repeat this process for each new n, you will always eventually reach 1.
// Though the conjecture has not been proven, no counterexample has ever been found.
// This function returns how many times this recurrence needs to be applied to reach 1.
fn collatz_iterations(n_base: u32) -> u32{
    var n: u32 = n_base;
    var i: u32 = 0u;
    loop {
        if (n <= 1u) {
            break;
        }
        if (n % 2u == 0u) {
            n = n / 2u;
        }
        else {
            // Overflow? (i.e. 3*n + 1 > 0xffffffffu?)
            if (n >= 1431655765u) {   // 0x55555555u
                return 4294967295u;   // 0xffffffffu
            }

            n = 3u * n + 1u;
        }
        i = i + 1u;
    }
    return i;
}

@compute
@workgroup_size(1)
fn main(@builtin(global_invocation_id) global_id: vec3<u32>) {
    v_indices[global_id.x] = collatz_iterations(v_indices[global_id.x]);
}
)";

TEST_CASE("Compute")
{
	uint32_t numbers[]      = {1, 2, 3, 4};
	uint32_t numbers_size   = sizeof(numbers);
	uint32_t numbers_length = numbers_size / sizeof(uint32_t);

	WGPUInstance instance = wgpuCreateInstance(nullptr);
	assert(instance);

	WGPUAdapter adapter = nullptr;
	wgpuInstanceRequestAdapter(instance, nullptr, handleRequestAdapter,
	                           static_cast<void*>(&adapter));
	assert(adapter);

	WGPUDevice device = nullptr;
	wgpuAdapterRequestDevice(adapter, nullptr, handleRequestDevice,
	                         static_cast<void*>(&device));
	assert(device);

	WGPUQueue queue = wgpuDeviceGetQueue(device);
	assert(queue);

	WGPUShaderModuleDescriptor shader_desc;
	shader_desc.label     = "shader";
	shader_desc.hintCount = 0;
	shader_desc.hints     = nullptr;

	WGPUShaderModuleWGSLDescriptor shader_wgsl_desc;
	shader_wgsl_desc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
	shader_wgsl_desc.chain.next  = nullptr;
	shader_wgsl_desc.code        = shader_code.c_str();

	shader_desc.nextInChain = reinterpret_cast<WGPUChainedStruct const*>(&shader_wgsl_desc);

	WGPUShaderModule shader_module = wgpuDeviceCreateShaderModule(device, &shader_desc);
	assert(shader_module);

	WGPUBufferDescriptor staging_buffer_desc;
	staging_buffer_desc.label = "staging_buffer";
	staging_buffer_desc.usage = WGPUBufferUsage_MapRead | WGPUBufferUsage_CopyDst;
	staging_buffer_desc.size  = numbers_size;
	staging_buffer_desc.mappedAtCreation = false;
	staging_buffer_desc.nextInChain      = nullptr;

	WGPUBuffer staging_buffer = wgpuDeviceCreateBuffer(device, &staging_buffer_desc);
	assert(staging_buffer);

	WGPUBufferDescriptor storage_buffer_desc;
	storage_buffer_desc.label = "storage_buffer";
	storage_buffer_desc.usage =
	    WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst | WGPUBufferUsage_CopySrc;
	storage_buffer_desc.size             = numbers_size;
	storage_buffer_desc.mappedAtCreation = false;
	storage_buffer_desc.nextInChain      = nullptr;

	WGPUBuffer storage_buffer = wgpuDeviceCreateBuffer(device, &storage_buffer_desc);
	assert(storage_buffer);

	WGPUComputePipelineDescriptor comp_pipeline_desc;
	comp_pipeline_desc.label       = "compute_pipeline";
	comp_pipeline_desc.nextInChain = nullptr;
	comp_pipeline_desc.layout      = nullptr;

	WGPUProgrammableStageDescriptor prog_stage_desc;
	prog_stage_desc.constantCount = 0;
	prog_stage_desc.constants     = nullptr;
	prog_stage_desc.entryPoint    = "main";
	prog_stage_desc.module        = shader_module;
	prog_stage_desc.nextInChain   = nullptr;

	comp_pipeline_desc.compute = prog_stage_desc;

	WGPUComputePipeline compute_pipeline =
	    wgpuDeviceCreateComputePipeline(device, &comp_pipeline_desc);
	assert(compute_pipeline);

	WGPUBindGroupLayout bind_group_layout =
	    wgpuComputePipelineGetBindGroupLayout(compute_pipeline, 0);
	assert(bind_group_layout);

	WGPUBindGroupDescriptor bind_group_desc;
	bind_group_desc.label       = "bind_group";
	bind_group_desc.layout      = bind_group_layout;
	bind_group_desc.entryCount  = 1;
	bind_group_desc.nextInChain = nullptr;

	std::array<WGPUBindGroupEntry, 1> bind_group_entries;
	bind_group_entries[0].binding     = 0;
	bind_group_entries[0].buffer      = storage_buffer;
	bind_group_entries[0].offset      = 0;
	bind_group_entries[0].size        = numbers_size;
	bind_group_entries[0].nextInChain = nullptr;
	bind_group_entries[0].sampler     = nullptr;
	bind_group_entries[0].textureView = nullptr;

	bind_group_desc.entries = bind_group_entries.data();

	WGPUBindGroup bind_group = wgpuDeviceCreateBindGroup(device, &bind_group_desc);
	assert(bind_group);

	WGPUCommandEncoderDescriptor command_enc_desc;
	command_enc_desc.label       = "command_encoder";
	command_enc_desc.nextInChain = nullptr;

	WGPUCommandEncoder command_encoder =
	    wgpuDeviceCreateCommandEncoder(device, &command_enc_desc);
	assert(command_encoder);

	WGPUComputePassDescriptor comp_pass_desc;
	comp_pass_desc.label           = "compute_pass";
	comp_pass_desc.nextInChain     = nullptr;
	comp_pass_desc.timestampWrites = nullptr;

	WGPUComputePassEncoder compute_pass_encoder =
	    wgpuCommandEncoderBeginComputePass(command_encoder, &comp_pass_desc);
	assert(compute_pass_encoder);

	wgpuComputePassEncoderSetPipeline(compute_pass_encoder, compute_pipeline);
	wgpuComputePassEncoderSetBindGroup(compute_pass_encoder, 0, bind_group, 0, nullptr);
	wgpuComputePassEncoderDispatchWorkgroups(compute_pass_encoder, numbers_length, 1, 1);
	wgpuComputePassEncoderEnd(compute_pass_encoder);
	wgpuComputePassEncoderRelease(compute_pass_encoder);

	wgpuCommandEncoderCopyBufferToBuffer(command_encoder, storage_buffer, 0, staging_buffer,
	                                     0, numbers_size);

	WGPUCommandBufferDescriptor command_buf_desc;
	command_buf_desc.label       = "command_buffer";
	command_buf_desc.nextInChain = nullptr;

	WGPUCommandBuffer command_buffer =
	    wgpuCommandEncoderFinish(command_encoder, &command_buf_desc);
	assert(command_buffer);

	wgpuQueueWriteBuffer(queue, storage_buffer, 0, &numbers, numbers_size);
	wgpuQueueSubmit(queue, 1, &command_buffer);

	wgpuBufferMapAsync(staging_buffer, WGPUMapMode_Read, 0, numbers_size, handleBufferMap,
	                   nullptr);
	wgpuDevicePoll(device, true, nullptr);

	uint32_t* buf =
	    static_cast<uint32_t*>(wgpuBufferGetMappedRange(staging_buffer, 0, numbers_size));
	assert(buf);

	printf("times: [%d, %d, %d, %d]\n", buf[0], buf[1], buf[2], buf[3]);

	REQUIRE(0 == buf[0]);
	REQUIRE(1 == buf[1]);
	REQUIRE(7 == buf[2]);
	REQUIRE(2 == buf[3]);

	wgpuBufferUnmap(staging_buffer);
	wgpuCommandBufferRelease(command_buffer);
	wgpuCommandEncoderRelease(command_encoder);
	wgpuBindGroupRelease(bind_group);
	wgpuBindGroupLayoutRelease(bind_group_layout);
	wgpuComputePipelineRelease(compute_pipeline);
	wgpuBufferRelease(storage_buffer);
	wgpuBufferRelease(staging_buffer);
	wgpuShaderModuleRelease(shader_module);
	wgpuQueueRelease(queue);
	wgpuDeviceRelease(device);
	wgpuAdapterRelease(adapter);
	wgpuInstanceRelease(instance);
}