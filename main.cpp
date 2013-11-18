#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <time.h>

// adds wsock32.lib to the linker
#pragma comment(lib, "wsock32.lib")

using namespace std;


void main() {
	WSADATA wsa_data; // this variable will contain the details of the winsock connection
	if(WSAStartup(MAKEWORD(2,0), &wsa_data) != 0) {
		cout << "Error initializing winsock version 2.0" << endl;
		WSACleanup(); // terminates the use of the winsock 2 DLL
		return;
	}

	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET) {
		cout << "Error creating socket" << endl;
		WSACleanup(); // terminates the use of the winsock 2 DLL
		return;
	}

	char servaddr[100];
	int port;
	char filepath[1024];
	long size;

	//display to user
	//cout << "Please enter the server address:" << endl;
	//cin >> servaddr;
	char *hostname = "192.168.71.50";

	//cout << "Please enter the server port" << endl;
	//cin >> port;

	//cout << "Please enter the path to the file you would like to upload" << endl;
	//cin >> filepath;


	// resolves the hostname to an IP address, stored in the hostent structure
	struct hostent *server = gethostbyname(hostname);
	if(server == NULL) {
		cout << "Error resolving hostname to an IP address" << endl;
		WSACleanup(); // terminates the use of the winsock 2 DLL
		return;
	}

	SOCKADDR_IN server_info;
	server_info.sin_port = htons(27015);
	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = *((unsigned long *)server->h_addr);

	
	if(connect(sock, (SOCKADDR*)(&server_info), sizeof(server_info)) != 0) {
		cout << "Error connecting to server" << endl;
		WSACleanup(); // terminates the use of the winsock 2 DLL
		return;
	}

	/*++++++++++++++++++++
	First, upload to server
	++++++++++++++++++++++*/
	char* rbuffer;			//will save file content here
	
	
	ifstream file;
	file.open("winbox.cfg", ios::in | ios::binary | ios::ate);		//open file
	
	if(file.is_open()){
		file.seekg(0, ios::end);
		size = file.tellg();	//file size! 
		
		cout << "The file size is " << size << " Bytes" << endl;

		file.seekg(0, ios::beg);		//sets location back to beginning of file

		rbuffer = new char[size];
		file.read(rbuffer, size);		//write file to buffer

		clock_t ustart, uend;			//timer
		ustart=clock();
		int j = send(sock, rbuffer, size, NULL);	//send file to server
		
		if (j == -1){
			cout << "Error uploading file to server :(" << endl;
		}

	//	char* ackbuf;
	//	ackbuf = new char[500]; //will write the acknowledgement to this buffer

	//	int a = recv(sock, ackbuf, 500, NULL);

//		cout << ackbuf << endl;

		uend=clock()-ustart;
		cout << "The file took " << (double)uend / ((double)CLOCKS_PER_SEC) << " seconds to upload" << endl;

		float up_br = (float)(8*size)/((float)uend/(double)CLOCKS_PER_SEC);
		cout << "The upload bit rate is " << up_br << " bps" << endl;

		file.close();
	}

	/*+++++++++++++++++++++
	Next, download file back
	++++++++++++++++++++++*/

	char* wbuffer;
	wbuffer = new char[size];		//will write message to this buffer

	clock_t dstart, dend;			//timer
	dstart=clock();		
	int k = recv(sock, wbuffer, size, NULL);	//receiving message from server
	if (k == -1){
		cout << "Error downloading file from server :(" << endl;
	}
	cout << "receiving file" << endl;
	dend=clock()-dstart;
		
	cout << "The file took " << (double)dend / ((double)CLOCKS_PER_SEC) << " seconds to download" << endl;

	float down_br = (float)(8*size)/((float)dend/(double)CLOCKS_PER_SEC);
	cout << "The upload bit rate is " << down_br << " bps" << endl;

	cout << "Thank you for using my program!" << endl;

	shutdown(sock, SD_SEND);

	closesocket(sock);

	WSACleanup();
}
