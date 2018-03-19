#include <iostream>

#include <set>
#include <map>
#include <fstream>
#include <sstream>

#include <cmath>

using namespace std;

void readNodes(const char *filename, map<long, pair<int,int> > *m){
	ifstream infile(filename);
	string line;
	while(getline(infile,line)){
		istringstream iss(line);
		long id; int x, y;
		if(!(iss >> id >> x >> y)) break;
		m->insert(make_pair(id,make_pair(x,y)));
	}
}

void readEdges(const char *filename, map<long, pair<int,int> > *nodes){
	ifstream infile(filename);
	string line;
	while(getline(infile,line)){
		istringstream iss(line);
		long id, A, B; string name;
		if(!(iss >> id >> A >> B)) break;

		pair<int, int> Acoord, Bcoord;
		Acoord = nodes->find(A)->second;
		Bcoord = nodes->find(B)->second;
		//tree->InsertLine(Acoord.first,Acoord.second,Bcoord.first,Bcoord.second, name);
	}
}

void readBrinkhoff(const char *filename){
	map<long, pair<double, pair<int,int> > >Objects; // id -> (time, (x,y) )

	ifstream infile(filename);
	string line;
	while(getline(infile,line)){
		istringstream iss(line);
		char cls; long id; double time, speed;
		int cID, currX, currY, nextX, nextY;
		if(!(iss >> cls >> id >> cID >> 
			time  >> currX >> currY >> 
			speed >> nextX >> nextY)) break;

		if(cls == '0'){
			Objects[id] = make_pair(time, make_pair(currX, currY));
		}
		else{
			pair<double, pair<int,int> > lastPos = Objects[id];
			double t0 = lastPos.first;
			int x0 = lastPos.second.first;
			int y0 = lastPos.second.second;
			//tree->InsertTripSegment(id,x0,y0,currX,currY,t0,time);
			//cout << x0 << " " << y0 << " ";
			//cout << currX << " " << currY << " ";
			double delX = (currX > x0)? currX-x0 : x0-currX; double delY = (currY > y0)? currY-y0 : y0-currY; double dist = sqrt(delX*delX + delY*delY);
			if ( round(round(dist)/100) == 6)
				cout << t0 << " " << time << endl;
			Objects[id] = make_pair(time,make_pair(currX,currY));
		}
	}

}

int main(int argc, char const *argv[])
{

	if(argc != 4)
	{
		cout << "Usage: ./xtest [nodesFile] [edgesFile] [trajectoriesFile]" << endl;
		return -1;
	}

	const char* nodesFile 			= argv[1];
	const char* edgesFile 			= argv[2];
	const char* trajectoriesFile 	= argv[3];

	map<long, pair<int,int> > *Nodes = new map<long, pair<int,int> >();
	readNodes(nodesFile, Nodes);
	readEdges(edgesFile, Nodes);
	readBrinkhoff(trajectoriesFile);

	cout << endl;

	return 0;
}
