#pragma once

#include <iostream>

/**
 * \brief A wrapper around a file stream that manages reading and writing of common data types.
 */
class BinaryStreamBuilder
{
public:
    explicit BinaryStreamBuilder(std::fstream* stream) : m_stream{stream}, m_managedStream{false}
    {
    }

    explicit BinaryStreamBuilder(std::string_view fileName) : m_stream{new std::fstream{}}, m_managedStream{true}
    {
        m_stream->open(fileName, std::ios::binary | std::ios::in | std::ios::out);

        if (!m_stream->is_open())
            std::cerr << "Failed to open file " << fileName << "!" << std::endl;
    }

    ~BinaryStreamBuilder()
    {
        if (m_managedStream)
        {
            m_stream->close();
            delete m_stream;
        }
    }

    // Writes an arbitrary amount of binary data into the stream.
    template <typename T>
    BinaryStreamBuilder& write(const T& data, size_t size)
    {
        m_stream->write(reinterpret_cast<const char*>(&data), static_cast<std::streamsize>(size));
        return *this;
    }

    // Reads an arbitrary amount of binary data from the stream.
    template <typename T>
    BinaryStreamBuilder& read(T* result, size_t size)
    {
        m_stream->read(reinterpret_cast<char*>(result), static_cast<std::streamsize>(size));
        return *this;
    }

    template <typename T>
    BinaryStreamBuilder& writeFixed(const T& data)
    {
        write(data, sizeof(T));
        return *this;
    }

    template <typename T>
    BinaryStreamBuilder& readFixed(T* result)
    {
        read(result, sizeof(T));
        return *this;
    }

    template <typename T>
    BinaryStreamBuilder& writeVector(const std::vector<T>& data)
    {
        size_t size{data.size() * sizeof(T)};
        writeFixed(data.size());
        write(data[0], size);
        return *this;
    }

    template <typename T>
    BinaryStreamBuilder& readVector(std::vector<T>* result)
    {
        size_t size{};
        readFixed(&size);
        result->resize(size);
        read(result->data(), size * sizeof(T));
        return *this;
    }

    BinaryStreamBuilder& writeString(std::string_view data)
    {
        size_t size{data.length() * sizeof(char)};
        writeFixed(size);
        write(data[0], size);
        return *this;
    }

    BinaryStreamBuilder& readString(std::string* result)
    {
        size_t size{};
        readFixed(&size);
        result->resize(size);
        read(result->data(), size);
        return *this;
    }

private:
    std::fstream* m_stream;
    bool m_managedStream;
};
