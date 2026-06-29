// UFO
#include <ufo/utility/io/buffer.hpp>

namespace ufo
{
Buffer& Buffer::write(void const* src, size_type count)
{
	WriteBuffer::write(src, count);
	ReadBuffer::size_ = WriteBuffer::size_;
	return *this;
}

Buffer& Buffer::write(std::istream& in, size_type count)
{
	WriteBuffer::write(in, count);
	ReadBuffer::size_ = WriteBuffer::size_;
	return *this;
}

void Buffer::clear()
{
	WriteBuffer::clear();
	ReadBuffer::size_ = 0;
	ReadBuffer::pos_  = 0;
}

std::byte* Buffer::data() { return WriteBuffer::data(); }

std::byte const* Buffer::data() const { return WriteBuffer::data(); }

bool Buffer::empty() const { return WriteBuffer::empty(); }

Buffer::size_type Buffer::size() const { return WriteBuffer::size(); }

void Buffer::reserve(size_type new_cap)
{
	WriteBuffer::reserve(new_cap);
	ReadBuffer::data_ = WriteBuffer::data();
}

void Buffer::resize(size_type new_size)
{
	WriteBuffer::resize(new_size);
	ReadBuffer::data_ = WriteBuffer::data();
}
}  // namespace ufo