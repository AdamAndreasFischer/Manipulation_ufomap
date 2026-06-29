// UFO
#include <ufo/utility/io/read_buffer.hpp>

// STL
#include <algorithm>
#include <cstring>

namespace ufo
{
ReadBuffer::ReadBuffer(std::byte const* data, size_type count) : data_(data), size_(count)
{
}

ReadBuffer& ReadBuffer::read(void* dest, size_type count)
{
	if (size() < pos_ + count) {
		// TODO: Fill in exception message
		throw std::out_of_range("");
	}

	return readUnsafe(dest, count);
}

ReadBuffer& ReadBuffer::read(std::ostream& out, size_type count)
{
	if (size() < pos_ + count) {
		// TODO: Fill in exception message
		throw std::out_of_range("");
	}

	return readUnsafe(out, count);
}

bool ReadBuffer::readLine(std::string& line)
{
	// FIXME: Implement correct
	std::byte const* it = std::find_if(data_ + pos_, data_ + size_, [](std::byte b) {
		return '\n' == static_cast<char>(b);
	});
	line.assign(reinterpret_cast<char const*>(data_ + pos_),
	            reinterpret_cast<char const*>(it));
	pos_ = std::min(size_, pos_ + line.size() + 1);
	return pos_ != size_;
}

ReadBuffer& ReadBuffer::readUnsafe(void* dest, size_type count)
{
	std::memmove(dest, data_ + pos_, count);
	pos_ += count;
	return *this;
}

ReadBuffer& ReadBuffer::readUnsafe(std::ostream& out, size_type count)
{
	out.write(reinterpret_cast<char const*>(data_), static_cast<std::streamsize>(count));
	pos_ += count;
	return *this;
}

std::byte const* ReadBuffer::data() const { return data_; }

bool ReadBuffer::empty() const { return 0 == size(); }

ReadBuffer::size_type ReadBuffer::size() const { return size_; }

ReadBuffer::size_type ReadBuffer::readPos() const noexcept { return pos_; }

void ReadBuffer::readPos(size_type pos) noexcept { pos_ = pos; }

void ReadBuffer::readSkip(size_type count) noexcept { pos_ += count; }

ReadBuffer::size_type ReadBuffer::readLeft() const noexcept
{
	return size_ < pos_ ? 0 : pos_ - size_;
}
}  // namespace ufo