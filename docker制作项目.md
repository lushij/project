docker制作项目

安装g++

```c++
apt install g++
```



安装openssl

```c++
apt-get install libcurl4-openssl-dev
```



安装curl

```c++
apt-get install libcurl4-openssl-dev
```

```c++
apt-get install zlib1g-dev
```

```c++
apt-get install libboost-all-dev
```

项目启动

```c++
docker run -it --network my_network -p 1236:1236 clouddisk:ubuntu /bin/bash
    
docker run -it -v /home/lu/Project:/root/project --network my_network -p 1236:1236 clouddisk:ubuntu /bin/bash
    
docker run -d  --network my_network --hostname rabbitsvr --name rabbit -p 5672:5672 -p 15672:15672 -p 25672:25672 -v /data/rabbitmq:/var/lib/rabbitmq rabbitmq:management

```

```c++
tar czf cloudDisk1.0.tar cloudDisk1.0/

```

```c++
保存旧镜像为新的就镜像id 新镜像名称:版本
docker commit 1bbbc33a2897 my_new_image:tag

```

