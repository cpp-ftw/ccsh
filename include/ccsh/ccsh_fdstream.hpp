#ifndef CCSH_CCSH_FDSTREAM_HPP
#define CCSH_CCSH_FDSTREAM_HPP

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>

namespace ccsh {

namespace internal {

class ofdstreambuf : public std::streambuf
{
    int fd;
    std::array<char, BUFSIZ> buffer_;
public:
    explicit ofdstreambuf(int fd)
        : fd(fd)
    {
        char* base = &buffer_.front();
        setp(base, base + buffer_.size() - 1); // -1 to make overflow() easier
    }

    ofdstreambuf(ofdstreambuf&&) = default;
    ofdstreambuf(ofdstreambuf const&) = delete;
    ofdstreambuf& operator=(ofdstreambuf&&) = default;
    ofdstreambuf& operator=(ofdstreambuf const&) = delete;

    bool do_flush();

    int_type overflow(int_type c) override;
    int sync() override
    {
        return do_flush() ? 0 : -1;
    }
};

class ifdstreambuf : public std::streambuf
{
private:
    static constexpr std::size_t put_back_size = 8;

    int fd;
    std::array<char, BUFSIZ> buffer_;

public:
    explicit ifdstreambuf(int fd)
        : fd(fd)
    {
        char* end = &buffer_.front() + buffer_.size();
        setg(end, end, end);
    }

    ifdstreambuf(ifdstreambuf&&) = default;
    ifdstreambuf(ifdstreambuf const&) = delete;
    ifdstreambuf& operator=(ifdstreambuf&&) = default;
    ifdstreambuf& operator=(ifdstreambuf const&) = delete;

private:
    int_type underflow() override;
};

} // namespace internal

class ofdstream : public std::ostream
{
    std::unique_ptr<std::streambuf> buf;
public:
    explicit ofdstream(int fd)
        : std::ostream{new internal::ofdstreambuf{fd}}
        , buf(rdbuf())
    { }
};

class ifdstream : public std::istream
{
    std::unique_ptr<std::streambuf> buf;
public:
    explicit ifdstream(int fd)
        : std::istream{new internal::ifdstreambuf{fd}}
        , buf(rdbuf())
    { }
};

} // namespace ccsh

#endif // CCSH_CCSH_FDSTREAM_HPP
