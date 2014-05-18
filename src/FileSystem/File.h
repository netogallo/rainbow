// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILE_H_
#define FILESYSTEM_FILE_H_

#include <cstdio>

#include "Common/NonCopyable.h"

struct AAsset;

class File : private NonCopyable<File>
{
public:
	static File open(const char *const path);
	static File open_asset(const char *const path);
	static File open_document(const char *const path);
	static File open_write(const char *const path);

	inline File();
	File(File &&);
	~File();

	/// Returns the file size.
	size_t size() const;

	/// Reads \p size bytes from file into buffer \p dst.
	/// \param[out] dst   Destination buffer.
	/// \param      size  Number of bytes to read.
	/// \return Number of bytes read.
	size_t read(void *dst, const size_t size) const;

	/// Sets the file position indicator for the file stream to the value
	/// pointed to by \p offset. See \c fseek().
	/// \param offset  Number of bytes to shift the position relative to origin.
	/// \param origin  Position to which offset is added.
	/// \return 0 upon success, nonzero value otherwise.
	int seek(const long offset, const int origin) const;

	/// Writes buffer at \p buffer to file.
	/// \param buffer  Source buffer.
	/// \param size    Number of bytes to write.
	/// \return Number of bytes written.
	size_t write(const void *buffer, const size_t size) const;

	inline explicit operator bool() const;
	inline operator AAsset*() const;
	inline operator FILE*() const;

private:
	bool is_asset_;  ///< Whether this file is an asset.

	union
	{
		AAsset *asset_;
		FILE *stream_;
	};

	explicit File(const char *const path);
	File(const char *const path, const char *const mode);
};

File::File() : is_asset_(false), stream_(nullptr) { }

File::operator bool() const
{
	return stream_;
}

File::operator AAsset*() const
{
	return asset_;
}

File::operator FILE*() const
{
	return stream_;
}

#endif
