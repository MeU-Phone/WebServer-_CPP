#include "buffer.h"

Buffer::Buffer(int initBuffSize)
    : buffer_(initBuffSize), readPos_(0), writePos_(0) {}

size_t Buffer::ReadableBytes() const { return writePos_ - readPos_; }

size_t Buffer::WritableBytes() const { return buffer_.size() - writePos_; }

size_t Buffer::PrependableBytes() const { return readPos_; }

const char *Buffer::Peek() const { return BeginPtr_() + readPos_; }

void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    readPos_ += len;
}

void Buffer::RetrieveUntil(const char *end) {
    assert(Peek() <= end);
    Retrieve(end - Peek());
}

void Buffer::RetrieveAll() {
    bzero(&buffer_[0], buffer_.size());
    readPos_ = writePos_ = 0;
}

std::string Buffer::RetrieveAllToStr() {
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}

const char *Buffer::BeginWriteConst() const { return BeginPtr_() + writePos_; }

char *Buffer::BeginWrite() { return BeginPtr_() + writePos_; }

void Buffer::HasWritten(size_t len) { writePos_ += len; }

void Buffer::Append(const std::string& str) { Append(str.data(), str.length()); }

void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

void Buffer::Append(const char* str, size_t len){
    assert(str);
    EnsureWritable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const Buffer& buff) { Append(buff.Peek(), buff.ReadableBytes()); }

void Buffer::EnsureWritable(size_t len) {
    if(WritableBytes() < len){
        MakeSpace_(len);
    }
    assert(WritableBytes() >= len);
}

ssize_t Buffer::ReadFd(int fd, int* saveErrno) {
    char buff[65535];
    struct iovec iov[2];    /* 离散/聚集 I/O */
    const size_t writable = WritableBytes();
    //分散读，保证数据全部读完
    iov[0].iov_base = BeginPtr_() + writePos_;
<<<<<<< HEAD
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

=======
    iov[0].iov_len = writable;  
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    //从文件描述符fd所对应的的文件中读去count个数据段到多个buffers中,返回读写的字节数
>>>>>>> f9bd2f2 (1)
    const ssize_t len = readv(fd, iov, 2);
    if(len < 0){
        *saveErrno = errno;
    }
    else if(static_cast<size_t>(len) <= writable){
        writePos_ += len;
    }
<<<<<<< HEAD
    else{
=======
    else{   //如果没读完，申请内存后将剩余buff内未读取的数据继续读到buffer_中
>>>>>>> f9bd2f2 (1)
        writePos_ = buffer_.size();
        Append(buff, len - writable);
    }
    return len;
}

ssize_t Buffer::WriteFd(int fd, int* saveErrno) {
    size_t readSize = ReadableBytes();
    ssize_t len = write(fd, Peek(), readSize);
    if(len < 0){
        *saveErrno = errno;
        return len;
    }
    readPos_ += len;
    return len;
}

char* Buffer::BeginPtr_(){ return &*buffer_.begin(); }

const char* Buffer::BeginPtr_() const { return &*buffer_.begin(); }

void Buffer::MakeSpace_(size_t len) {
    if(WritableBytes() + PrependableBytes() < len){
        buffer_.resize(writePos_ + len + 1);
    } else {
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
        readPos_ = 0;
        writePos_ = readPos_ + readable;
        assert(readable == ReadableBytes());
    }
}