#include<iostream>
//#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include<tchar.h>
#include <boost/beast/core.hpp>
#include<boost/beast/websocket.hpp>
#include<string>
#include<thread>
#include<chrono>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

TCHAR phiscs_mem_name[] = TEXT("Local\\acpmf_physics");
TCHAR 


struct phisicsInfo {
	int packetId;
	float gas;
	float brake;
	float fuel;
	int gear;
	int rpms;
	float steerAngle;
	float speedKmh;
	float velocity[3];
	float accG[3];
	float wheelSlip[4];
	float wheelLoad[4];
	float wheelsPressure[4];
	float wheelAngularSpeed[4];
	float tyreWear[4];
	float tyreDirtLevel[4];
	float tyreCoreTemperature[4];
	float camberRad[4];
	float suspensionTravel[4];
	float drs;
	float tc;
	float heading;
	float pitch;
	float roll;
	float cgHeight;
	float carDamage[5];
	int numberOfTyresOut;
};

phisicsInfo* p;
LPVOID pBuf;

int main(int argc, char* argv[]) {
	HANDLE hMapFile;


	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		smName

	);
if (hMapFile == NULL)
   {
      _tprintf(TEXT("Could not open file mapping object (%d).\n"),
             GetLastError());
      return 1;
   }
		pBuf =MapViewOfFile(hMapFile,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			240);


if (pBuf == NULL)
   {
      std::cout<<"Could not map view of file (%d).\n"<<
             GetLastError();

       CloseHandle(hMapFile);

      return 1;
   }


memcpy(&p,&pBuf,sizeof(pBuf));
std::cout<<p->speedKmh<<std::endl;

auto const address = net::ip::make_address("127.0.0.1");
auto const port = static_cast<unsigned short>(std::atoi("8083"));

net::io_context ioc{1};
tcp::acceptor acceptor{ ioc,{address,port}};

for (;;) {
	tcp::socket socket{ ioc };
	acceptor.accept(socket);

	std::thread{
		std::bind(
			[q{std::move(socket)}]() {
				websocket::stream<tcp::socket> ws{std::move(const_cast<tcp::socket&>(q))};
				ws.set_option(websocket::stream_base::decorator(
					[](websocket::response_type& res)
					{res.set(http::field::server,
						std::string(BOOST_BEAST_VERSION_STRING) + "websocket-server-sync"); }));

				ws.accept();
				beast::flat_buffer out_buffer;
				
				while (true) {
					try {
						
						    memcpy(&p, &pBuf, sizeof(pBuf));

							std::string output;
							output = output.append("{\"throttle\":\"" + std::to_string(p->gas*100) + "\",");
							output = output.append("\"fuel\":\"" + std::to_string(p->fuel) + "\",");
							output = output.append("\"gear\":\"" + std::to_string(p->gear) + "\",");
							output = output.append("\"rpms\":\"" + std::to_string(p->rpms) + "\",");
							output = output.append("\"speedKph\":\"" + std::to_string(p->speedKmh ) + "\"}");
							

							boost::beast::ostream(out_buffer) << output ;
							
							ws.write(out_buffer.data());
							//std::this_thread::sleep_for(std::chrono::seconds(1));
							out_buffer.clear();
						
						

					}
					catch (beast::system_error const& se)
					{
						if (se.code() != websocket::error::closed)
						{
							std::cerr << "Error: " << se.code().message() << std::endl;
							break;
						}
					}

				}
}
)
	}.detach();
}






UnmapViewOfFile(pBuf);

CloseHandle(hMapFile);

return 0;
}