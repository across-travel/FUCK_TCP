# tcp_read_packet
唯一正确的一种从TCP流中读取报文的方法，其它的方法都是错的！

```
char recv_buf[];
Buffer buffer;
// 网络循环：必须在一个循环中读取网络，因为网络数据是源源不断的。
while(1){
    // 从TCP流中读取不定长度的一段流数据，不能保证读到的数据是你期望的长度
    tcp.read(recv_buf);
    // 将这段流数据和之前收到的流数据拼接到一起
    buffer.append(recv_buf);
    // 解析循环：必须在一个循环中解析报文，避免所谓的粘包
    while(1){
        // 尝试解析报文
        msg = parse(buffer);
        if(!msg){
            // 报文还没有准备好，糟糕，我们遇到拆包了！跳出解析循环，继续读网络。
            break;
        }
        // 将解析过的报文对应的流数据清除
        buffer.remove(msg.length);
        // 业务处理
        process(msg);
    }
}
```

真的，相信我，这是一条定理：__所有从TCP中读取报文的代码如果不长上面这个样子，一定是错误的。__

这条定理不仅有理论和逻辑的支持，还经过了无数人愚蠢的错误做法的反证。

http://www.ideawu.net/blog/archives/993.html
