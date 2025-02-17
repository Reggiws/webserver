# C++ WebServer Project Information

This is a high-performance Web server project based on C++ implementation , using multi-threaded epoll concurrency simple model , the basic processing flow of the HTTP protocol . This project can be used as a practical project as learning network programming , but also as a basic framework for building more complex Web server . (This project refers to nowcoder's linux high concurrency server development course content and github part of the open source project)

/

这是一个基于C++实现的高性能Web简易服务器项目，采用多线程+epoll的并发模型，实现了HTTP协议的基本处理流程。本项目为学习网络编程的实践项目，也可作为构建更复杂Web服务器的基础框架。（本项目参考nowcoder的linux高并发服务器开发的课程内容以及github上部分开源项目）
## Core features


process_graph:
```mermaid
graph TD
    A[启动服务器] --> B[创建线程池]
    B --> C[创建监听socket]
    C --> D[绑定端口并监听]
    D --> E[创建epoll实例]
    E --> F[将监听socket添加到epoll]
    F --> G[进入事件循环]

    G --> H{有事件发生?}
    H -- 是 --> I[处理事件]
    H -- 否 --> G

    I --> J{事件类型?}
    J -- 新连接 --> K[接受新连接]
    J -- 数据可读 --> L[读取数据]
    J -- 数据可写 --> M[写入数据]
    J -- 连接关闭或错误 --> N[关闭连接]

    K --> O[初始化新连接]
    O --> P[将新连接添加到epoll]
    P --> G

    L --> Q[将请求添加到线程池]
    Q --> R[线程池处理请求]
    R --> S[解析HTTP请求]
    S --> T{解析结果?}
    T -- 成功 --> U[生成HTTP响应]
    T -- 失败 --> V[生成错误响应]
    U --> W[将响应写入缓冲区]
    V --> W
    W --> X[将连接状态设置为可写]
    X --> G

    M --> Y[发送响应数据]
    Y --> Z{发送完成?}
    Z -- 是 --> AA[重置连接状态]
    Z -- 否 --> M
    AA --> G

    N --> AB[关闭连接并释放资源]
    AB --> G

    subgraph 线程池
        R --> AC[线程1]
        R --> AD[线程2]
        R --> AE[线程3]
        AC --> AF[处理请求]
        AD --> AF
        AE --> AF
    end

    subgraph HTTP请求解析
        S --> AG[解析请求行]
        AG --> AH[解析请求头]
        AH --> AI[解析请求体]
        AI --> AJ[验证请求]
    end

    subgraph HTTP响应生成
        U --> AK[生成状态行]
        AK --> AL[生成响应头]
        AL --> AM[生成响应体]
        V --> AN[生成错误状态行]
        AN --> AO[生成错误响应头]
        AO --> AP[生成错误响应体]
    end

    subgraph 网络通信
        C --> AQ[创建socket]
        D --> AR[绑定端口]
        AR --> AS[监听端口]
        E --> AT[创建epoll]
        F --> AU[添加监听socket]
        K --> AV[接受连接]
        AV --> AW[初始化连接]
        P --> AX[添加新连接到epoll]
        L --> AY[读取数据]
        M --> AZ[写入数据]
        N --> BA[关闭连接]
    end

    subgraph 资源管理
        AB --> BB[释放连接资源]
        W --> BC[管理写缓冲区]
        Y --> BD[管理发送状态]
    end

    style A fill:#f9f,stroke:#333,stroke-width:4px
    style B fill:#bbf,stroke:#333,stroke-width:4px
    style C fill:#bbf,stroke:#333,stroke-width:4px
    style D fill:#bbf,stroke:#333,stroke-width:4px
    style E fill:#bbf,stroke:#333,stroke-width:4px
    style F fill:#bbf,stroke:#333,stroke-width:4px
    style G fill:#f96,stroke:#333,stroke-width:4px
    style H fill:#f96,stroke:#333,stroke-width:4px
    style I fill:#f96,stroke:#333,stroke-width:4px
    style J fill:#f96,stroke:#333,stroke-width:4px
    style K fill:#f96,stroke:#333,stroke-width:4px
    style L fill:#f96,stroke:#333,stroke-width:4px
    style M fill:#f96,stroke:#333,stroke-width:4px
    style N fill:#f96,stroke:#333,stroke-width:4px
    style O fill:#f96,stroke:#333,stroke-width:4px
    style P fill:#f96,stroke:#333,stroke-width:4px
    style Q fill:#f96,stroke:#333,stroke-width:4px
    style R fill:#f96,stroke:#333,stroke-width:4px
    style S fill:#f96,stroke:#333,stroke-width:4px
    style T fill:#f96,stroke:#333,stroke-width:4px
    style U fill:#f96,stroke:#333,stroke-width:4px
    style V fill:#f96,stroke:#333,stroke-width:4px
    style W fill:#f96,stroke:#333,stroke-width:4px
    style X fill:#f96,stroke:#333,stroke-width:4px
    style Y fill:#f96,stroke:#333,stroke-width:4px
    style Z fill:#f96,stroke:#333,stroke-width:4px
    style AA fill:#f96,stroke:#333,stroke-width:4px
    style AB fill:#f96,stroke:#333,stroke-width:4px
    style AC fill:#f96,stroke:#333,stroke-width:4px
    style AD fill:#f96,stroke:#333,stroke-width:4px
    style AE fill:#f96,stroke:#333,stroke-width:4px
    style AF fill:#f96,stroke:#333,stroke-width:4px
    style AG fill:#f96,stroke:#333,stroke-width:4px
    style AH fill:#f96,stroke:#333,stroke-width:4px
    style AI fill:#f96,stroke:#333,stroke-width:4px
    style AJ fill:#f96,stroke:#333,stroke-width:4px
    style AK fill:#f96,stroke:#333,stroke-width:4px
    style AL fill:#f96,stroke:#333,stroke-width:4px
    style AM fill:#f96,stroke:#333,stroke-width:4px
    style AN fill:#f96,stroke:#333,stroke-width:4px
    style AO fill:#f96,stroke:#333,stroke-width:4px
    style AP fill:#f96,stroke:#333,stroke-width:4px
    style AQ fill:#f96,stroke:#333,stroke-width:4px
    style AR fill:#f96,stroke:#333,stroke-width:4px
    style AS fill:#f96,stroke:#333,stroke-width:4px
    style AT fill:#f96,stroke:#333,stroke-width:4px
    style AU fill:#f96,stroke:#333,stroke-width:4px
    style AV fill:#f96,stroke:#333,stroke-width:4px
    style AW fill:#f96,stroke:#333,stroke-width:4px
    style AX fill:#f96,stroke:#333,stroke-width:4px
    style AY fill:#f96,stroke:#333,stroke-width:4px
    style AZ fill:#f96,stroke:#333,stroke-width:4px
    style BA fill:#f96,stroke:#333,stroke-width:4px
    style BB fill:#f96,stroke:#333,stroke-width:4px
    style BC fill:#f96,stroke:#333,stroke-width:4px
    style BD fill:#f96,stroke:#333,stroke-width:4px
```


### Concurrent processing
- Using the concurrency model of multithreading + epoll.
- Implement a thread pool to handle concurrent requests, avoiding frequent thread creation and destruction.
- Use epoll ET edge trigger mode to realize efficient event processing.
- Use EPOLLONESHOT to ensure that a socket connection is processed by only one thread at any given time.

### HTTP support
- HTTP GET method support
- Support HTTP GET method for parsing and responding to HTTP requests.
- Supports HTTP long connection (Keep-Alive).
- Support for static resource access
- Implemented complete HTTP error handling (400, 403, 404, 500 and other status codes)

### Performance optimizations
- Optimize file transfers using memory mapping (mmap).
- Improve data sending efficiency by using writev to distribute writes.
- Implement port multiplexing
- Using non-blocking I/O to improve concurrency performance

### Synchronization
- Encapsulate mutex, condition variable and semaphore.
- Thread synchronization and thread safety
- RAII mechanism for resource management

## Project deficiencies

### Functional limitations
- Only GET method is supported. Other HTTP methods such as POST are not supported.
- Only supports GET method, no other HTTP methods such as POST.
- HTTPS protocol is not supported.
- Lack of access control and security mechanisms.

### Project integrity
- Lack of logging system, not conducive to troubleshooting
- Lack of logging system, which is not conducive to troubleshooting.
- Code comments and documentation are not perfect.
- Lack of unit test and performance test data

### Performance improvement
- Lack of timer mechanism to handle timeout connections
- Lack of efficient memory pooling
- No load balancing.
- Load balancing is not implemented. Concurrency performance can be further optimized.

### Future development direction

1. Functionality Extension
   - Add support for HTTP methods such as POST.
   - Implement dynamic content processing
   - Add HTTPS support
   - Implement access control and security mechanism

2. Project optimization
   - Implement complete logging system
   - Add configuration file support
   - Add unit tests
   - Improve the project documentation

3. Performance Improvement
   - Implement timer to handle timeout connections
   - Add memory pool
   - Load balancing
   - Optimize concurrency handling mechanism

4. Architecture Improvement
   - Optimize code structure and improve scalability
   - Implement modularized design
   - Reduce code coupling
   - Add plug-in mechanism

This project, although there is still a lot of room for improvement, has already realized the core functionality of the Web server and is a structurally complete, easy to learn and extend the basic framework. For developers who want to learn network programming , multi-threaded development and server architecture , is a good practice project .
