#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <string>
using namespace std;

int main(){
	string filename;
	cout<<"Input filename: ";
	cin>>filename;
	
	
	pid_t pid = fork();
	if(pid){
	//parent process
		//Open the file
		int fd = open(filename.c_str(),O_RDWR);
		if (fd == -1) {
			cerr<<"Failed to open file\n";
			return -1;
		}
		// Get the size of the file
		off_t filesize = lseek(fd, 0, SEEK_END);

		// Map the file into memory
		void* mapped_file = mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
		if (mapped_file == MAP_FAILED) {
			cerr << "Failed to map file into memory\n";
			return -1;
		}

		// Change the contents of the file
		string new_content = "Hello world!";
		memcpy(mapped_file, new_content.c_str(), new_content.size());
		
		//Wait to see if the read can detect the change immediately before we munmap
		waitpid(pid, NULL, 0);

		//Unmap the file from memory
		munmap(mapped_file, filesize);

		//Close the file
		close(fd);
    	}
	else{
	//child process
		//Open the file
		int fd = open(filename.c_str(), O_RDWR);
		if (fd == -1) {
			cerr << "Failed to open file\n";
			return -1;
		}

		// Get the size of the file
		off_t filesize = lseek(fd, 0, SEEK_END);

		// Map the file into memory
		void* mapped_file = mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
		if (mapped_file == MAP_FAILED) {
			cerr << "Failed to map file into memory\n";
			return -1;
		}
		
		//Print the contents of the file
		cout<<"File contents:\n";
		cout<<static_cast<const char*>(mapped_file)<<endl;

		// Unmap the file from memory
		munmap(mapped_file, filesize);
		
		//Close file
		close(fd);
    	}
	return 0;
}
