# synchronous daytime clock via tcp in c++

## requirements
- boost & asio
- clang
- cmake


## setting up

```
git clone https://github.com/abschill/tcp-daytime-clock
cd tcp-daytime-clock
bash setup.sh
./server/server
./client/client <private_ip_address>
```

calling the client will return the relative time of the day to standard output