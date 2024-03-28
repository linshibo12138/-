#include <TCPKernel.h>



int main(int argc,char *argv[])
{
    int port = 8000;
    if( argc >= 2 )
    {
        port = atoi(argv[1]);
    }
    TcpKernel * pKernel =  TcpKernel::GetInstance();

    //开启服务 给定端口, 可以使用输入的port
    pKernel->Open( port);
    cout<<"-----------"<<endl;
    // 事件循环 : 循环监听事件
    pKernel->EventLoop();

    pKernel->Close();

    return 0;
}


//#include <chrono>
//#include <iostream>
//#include <typeinfo>
//using namespace std;

//time_t GetCurrentTimeMsec(){
//    auto time = chrono::time_point_cast<chrono::milliseconds>(chrono::system_clock::now());
//    time_t timestamp = time.time_since_epoch().count();
//    return timestamp;
//}

//int main() {
//    cout<<typeid(GetCurrentTimeMsec()).name()<<endl;
//}
