#include <iostream>
#include <map>
#include <vector>
#include "CORE.h"
#include <stdlib.h>
#include <string>
#include <chrono>

using namespace std;

// :)

/* 
 * CORE.h function list:
 *
 * kbhit() : bool
 * raw_mode_on() : void
 * raw_mode_off() : void 
 * getch() : char
 *
*/


class Game{
	public:
		//structs
		struct body;

		//Game functions
		void loop();
		void gameover();
		bool endgame();

		void input(char in);
		void frame_processor();

		void spawn_apple();
		void init();

		void info(string in);
		void draw_frame();
		void draw_ver_border();
		void draw_hor_border();
		string rgb(int r, int g, int b);
		string Color(string foreground, string background="");

		//constructor/destructor
		Game(int iw, int ih, int FPI){
			w = iw;
			h = ih;
			
			if (FPI > 0) fpi = FPI;

			init();	
		}
		~Game(){
			delete[] screen;
		}
	private:
		//colors
		string color_clear = "\033[0m";

		string color_light = rgb(208,208,88);  
		string color_mid = rgb(160,168,64);
		string color_dim = rgb(112,128,40);
		string color_dark = rgb(64,80,16);

		//sys
		vector<body> tail;
		int fpi=110000;
		int w,h;
		short** screen;

		//game
		bool game_loop=true;
		short dir=1;
		short last_dir;
		bool apple=false;
		int points=0;
		vector<int> bus;

		//graphics
		int surround = 2;
		int thicness = 2;
		
		//maps
		map<char,int> movement = {
			{'h',1},
			{'j',-2},
			{'k',2},
			{'l',-1},
			
			{'a',1},
			{'s',-2},
			{'w',2},
			{'d',-1},

			{'[',10},
			{']',11}
		};
		map<int,string> sprites = {
			{0,Color(color_light,color_light) + " "},
			{1,Color(color_dark,color_dark) + "#"},
			{2,Color(color_dark, color_dark) + "@"},
			{3,Color(color_dark, color_dark) + "&"}
		};

};
//Main game structs
struct Game::body{
	int x,y;
	bool head=false;
};

//Main loop events
void Game::init(){

	screen = new short*[h];
	for(int i=0;i<h;i++)
		screen[i] = new short[w];	
	
	body head;
	last_dir = -1*dir;

       	head.x = int(w/2);	
       	head.y = int(h/2);	

	head.head = true;
	
	tail.push_back(head);
	
	//start game
	spawn_apple();
	loop();
}

void Game::spawn_apple(){

	if (apple) return;

	srand(time(0));

	int rx,ry;
	bool rerol;

	while(true){
		rerol = false;

		rx = rand() % w;
		ry = rand() % h;
		
		for(auto it : tail){
			if(rx == it.x && ry == it.y){
				rerol = true;
				break;
			}
		}
		if(!rerol) break;

	}
	
	screen[ry][rx] = 3;	
	apple = true;
}

void Game::input(char in){
	if (movement[in] >= 10){
		bool ch = movement[in] - 10;

		if (ch) fpi += 100000;
		else fpi -= 100000;
		
		return;
	}	

	last_dir = dir;
	dir = movement[in];	
	
	//validate input
	if (dir == -1 * last_dir) dir = last_dir;
}

void Game::gameover(){
	//end game
	game_loop = false;
}

bool Game::endgame(){
	return (
	(tail[0].x < 0)		||
	(tail[0].y >= h) 	||
	(tail[0].y < 0) 	||
	(tail[0].x >= w) 	||
	(screen[tail[0].y][tail[0].x] == 1)
	);
	
}

void Game::frame_processor(){
	

	body temp2;
	temp2.head = false;
	temp2.x = tail[tail.size()-1].x;
	temp2.y = tail[tail.size()-1].y;
		
	//move head and back
	screen[temp2.y][temp2.x] = 0;
		
	for (int i=tail.size()-1; i > 0; i--){
		tail[i] = tail[i-1];
		tail[i].head = false;
	}

	if(dir == 1){ //left
		tail[0].x -= 1; 
	}
	else if(dir == -2){ //down
		tail[0].y += 1; 
	}
	else if(dir == 2){ // up
		tail[0].y -= 1; 

	}
	else if(dir == -1){ // right
		tail[0].x += 1;

	}
	

	if(endgame()) return;
	
	//process points
	body temp = tail[0];
	if(screen[temp.y][temp.x] == 3){
		points++;
		tail.push_back(temp2);
		apple = false;
		spawn_apple();
	}

	//load to map	
	for(auto t : tail){
		screen[t.y][t.x] = 1;
		if (t.head) screen[t.y][t.x]++;
	}

}

//Mixed up names TODO: fix

void Game::draw_ver_border(){
	
	int S = w*2 + (surround*thicness)-4;

	cout << Color(color_mid,color_dark);
	cout << Color(color_mid,color_dark) << "||";
	for(int i=0;i < int(S/2);i++){
		cout << "- ";
	}
	cout << Color(color_mid,color_dark) << "||";
	cout << color_clear; 
	cout << '\n';
}

void Game::draw_hor_border(){
	cout << color_clear << Color(color_mid,color_dark);
	for(int i=0;i<int(surround/2)*thicness;i++){
		cout << "|";
	}
	cout << color_clear;
}

void Game::info(string in){
	
	int S = w*2 + (surround*thicness)-3;
	int mid = int(S/2) - int(in.size()/2);

	cout << Color(color_mid,color_dark) << "||";
	cout << Color(color_light, color_light);
	for(int i=0; i < S ;i++){
		if (i == mid){
			cout << Color(color_dark, color_light) << in;
			i+=in.size();
			continue;
		}
		cout << " ";
	}
	
	cout << Color(color_mid,color_dark) << "||";
	cout << color_clear; 
	cout << '\n';

}

string Game::Color(string foreground, string background){
	/*
		\033[38;2;<r>;<g>;<b>m     #Select RGB foreground color
		\033[48;2;<r>;<g>;<b>m     #Select RGB background color

		\033[38;2;<r>;<g>;<b>;48;2;<r>;<g>;<b>m
	*/
	if(foreground == "") return "\033[48;2" + background + 'm';
	else if(background == "") return "\033[38;2" + foreground + 'm';
	return "\033[38;2" + foreground + ";48;2" + background + 'm';
}

string Game::rgb(int r, int g, int b){
	return ';' + to_string(r) + ';' + to_string(g) + ';' + to_string(b);	
}

void Game::draw_frame(){

	frame_processor();

	string endmessage = "game over";	

	system("clear");
	//cout <<points << " | " <<fpi << " | " << tail.size() << " | x: "<< tail[0].x << " y: " << tail[0].y<< " | w: " << w << " h: "<< h <<'\n';
	
	draw_ver_border();
	info("Snake");
	draw_ver_border();
	draw_ver_border();

	for(int y=0;y<h;y++){

		if (y == int(h/2) && endgame()){
			info(endmessage);
			continue;
		}

		draw_hor_border();

		for(int x=0;x<w;x++){
			if (!endgame()){
				cout << sprites[screen[y][x]];
				cout << ' ';
			}
			else{
				cout << Color("",color_light) << "  ";
			}
		}
		draw_hor_border();
		cout << '\n';
	}
	draw_ver_border();
	info(to_string(points));
	draw_ver_border();
}

//Game loop
void Game::loop(){
	int fc=0;

	raw_mode_on();
	while(game_loop){
		//check input
		if(kbhit()){
			input(getch());
			raw_mode_off();
			raw_mode_on();
		}	
		//genenerate and draw frame
		if(fc >= fpi){
			draw_frame();
			fc=0;
			if(endgame()) gameover();			
		}
		fc++;//incremate frame count
	
	}
	raw_mode_on();
	tcflush(0, TCIFLUSH);
	
}

int main(){
	int w,h;
	cout << "Snake in terminal\n";
	cout << "Recomended settings w:20 h:20\n\n!!!Important!!!\nTo change snake speed press `[` to decrease and `]` to increase speed\n\n";
	
	cout << "width : ";
	cin >> w;
	cout << "height : ";
	cin >> h;
	Game snake(w,h,1200000);
}
