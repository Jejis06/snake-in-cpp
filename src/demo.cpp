#include <sys/ioctl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>

using namespace std;


char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}



void enable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode()
{
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

bool kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
//    cout << byteswaiting << '\n';
    return byteswaiting > 0;
}

void frame(int** arr, int w, int h, int fc, int framerate){
	system("clear");
	cout << "| fpi | " << fc << " / " << framerate <<" |\n";
	for(int y=0;y<h;y++){
		for(int x=0;x<w;x++){
			if(x==0) cout << '|';
			if(arr[y][x] == 0) cout << ' ';
			else cout << '@';
		}
		cout <<"|\n";
	}

	cout <<'\n';
}



int main(){
	char temp;
	int it=0;
	
	int h=20,w=20;
	int dir=0;
	

	int fc=0,framerate=110000;

	int sx=int(w/2),sy=int(h/2);

	int** arr = new int*[h];
	for(int i=0;i<h;i++)
		arr[i] = new int[w];


	//simple volume control with controlled with `a` and `d`
	

	enable_raw_mode();

	arr[sy][sx]=1;

	while(1){
		if(kbhit()){
			temp = getch();
			
			
			if(temp == 'a'){
				dir = 0;
			}
			if(temp == 'd'){
				dir = 1;
			}
			if(temp == 'w'){
				dir = 2;

			}
			if(temp == 's'){
				dir = 3;

			}
			
			if(temp == ',') {
				framerate -= 1000;
			}
			if(temp == '.') {
				framerate += 1000;
			}

			disable_raw_mode();
			enable_raw_mode();
		}	
		if(fc >= framerate){
			arr[sy][sx]=0;

			if(dir == 0) sx = (sx-1 < 0)?w-1:sx-1;	
			else if(dir == 1) sx = (sx+1 >= w)?0:sx+1;	
			else if(dir == 2) sy = (sy-1 < 0)?h-1:sy-1;	
			else if(dir == 3) sy = (sy+1 >= h)?0:sy+1;	

			arr[sy][sx]=1;
			frame(arr,w,h,fc,framerate);
			fc=0;
		}
		else fc++;
	}
	disable_raw_mode();
	tcflush(0, TCIFLUSH);

	delete[] arr;

}
