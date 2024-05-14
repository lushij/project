all:server consumer
server:main.o
	g++ $^ -o $@ -lSimpleAmqpClient -lwfrest -lworkflow -lssl -lcrypt -lcrypto -lalibabacloud-oss-cpp-sdk -lcurl -lpthread
main.o:main.cc
	g++ -c $^ -g -o  $@  -std=c++11 -fno-rtti 
consumer:consumer.o
	g++ $^ -o $@ -std=c++11 -lSimpleAmqpClient -fno-rtti -lalibabacloud-oss-cpp-sdk -lcurl -lcrypto -lpthread
clean:
	$(RM) server main.o consumer consumer.o
rebuild: clean all
