#include <stdio.h>
#include <stdlib.h>
#include "sim.h"

int main(int argc, char **argv){
	sim::Message msg;
	msg.set_type("ok");
	msg.set(34, 1);
	msg.set(35, "A");
	msg.set(49, "CLIENT 06\n你好");
	msg.set(56, "SERVER");
	msg.set(52, "20150424-03:06:59.114");
	msg.set(96, "H:12345678:12345678:");
	msg.set(98, 0);
	msg.set(141, "Y");
	msg.set(108, 20);

	std::string s = msg.encode();
	printf("%s \n", s.c_str());
	// 将 s 写入 socket, 相当于向服务器发送了请求
	
	sim::Decoder decoder;
	for(int i=0; i<2000; i++){
		// 模拟从网络读取数据, 一次只读取一个字节
		decoder.push(s.data() + (i%s.size()), 1);
		
		while(1){
			sim::Message tmp;
			int ret = decoder.parse(&tmp);
			if(ret == -1){
				printf("error! ret = %d\n", ret);
				exit(0);
			}else if(ret == 0){
				// 报文未就绪, 继续读网络
				break;
			}
			
			printf("%s\n", tmp.get(49)->c_str());
			printf("%s", tmp.encode().c_str());
			// 继续解析, 因为可能一次读取到多个报文
		}
	}

	return 0;
}
