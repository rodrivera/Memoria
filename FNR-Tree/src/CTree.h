#ifndef CTREE_H
#define CTREE_H

//#define DEBUG 1

#ifdef DEBUG
#define D(x) x
#else 
#define D(x)
#endif

#include <iostream>
#include <vector>
#include <set>
#include "includes/rtree/RTree.h"
#include "includes/isiis/IS-IIS.h"

#include <map>

using namespace std;

    	int cont_OK;
    	int cont_MISS;

class CTree
{
public:
	class SpatialLeaf;


private:
	RTree<SpatialLeaf*, int, 2, float> *SpatialLevel;

public:
	~CTree() {};
	
	CTree()
	{
		SpatialLevel = new RTree<SpatialLeaf*, int, 2, float>;
	}

	class Line
	{
	public:
		int min[2];
		int max[2];

		Line();
		~Line() {};
		Line(int a_minX, int a_minY, int a_maxX, int a_maxY)
		{
			min[0] = a_minX; min[1] = a_minY;
			max[0] = a_maxX; max[1] = a_maxY;
		}
		bool equals(const Line& other)
		{
			return min[0]==other.min[0] && min[1]==other.min[1] && max[0]==other.max[0] && max[0]==other.max[0];
		}
	};

	class SpatialLeaf
	{
	private:
		bool orientation;
		bool built;
		Stabbing *temporalTree;
		vector<interval> intervals;
		Line* line;
		size_t num_intervals;

	public:
		string nnn;

		SpatialLeaf();
		~SpatialLeaf() {};
		SpatialLeaf(Line* l, bool ori, string nn)
		{
			built = false;
			line = l;
			orientation = ori;
			nnn = nn;
		}
		Line* getLine()
		{
			return line;
		}
		bool getOrientation()
		{
			return orientation;
		}
		Stabbing* getTemporalTree()
		{
			return temporalTree;
		}
		void insertInterval(interval *i)
		{
			intervals.push_back(*i);
			built = false;
		}
		void build()
		{
					//cout << " >* BEGIN BUILD!! with " << intervals.size() << " ... ";
			if(!built)
			{
				temporalTree = new Stabbing(intervals,100,10000000); // 10^7
			}
			built = true;
			num_intervals = intervals.size();
			intervals.clear();

					//cout << " >* END BUILD!!" << endl;
		}
		size_t size()
		{
			size_t totalSize = sizeof(SpatialLeaf) + sizeof(Line);

			if (temporalTree->size() >= 5) 
			{
				totalSize += temporalTree->memory_usage();
			}

			return totalSize;
		}
		size_t S_size()
		{
			return sizeof(SpatialLeaf) + sizeof(Line);
		}
		size_t T_size()
		{
			return sizeof(Stabbing) + temporalTree->memory_usage();
		}
		size_t leaf_stats()
		{
			return num_intervals;
		}
	};

	int networkSize()
	{
		return SpatialLevel->Count();
	}

	void InsertLine(int x1, int y1, int x2, int y2, string name)
	{
		D(cout << "> BEGIN InsertLine:" << endl);

		bool ori = !( (x2 - x1)*(y2 - y1) >= 0 ); // 0 -> / , 1 -> \ .
		Line* tmpLine = new Line(min(x1,x2), min(y1,y2), max(x1,x2), max(y1,y2));

		D(cout << "    > Inserting.. (" << tmpLine->min[0] << "," << tmpLine->min[1] << ")->(" << tmpLine->max[0] << "," << tmpLine->max[1] << ")" << endl);

		SpatialLeaf* tmpLeaf = new SpatialLeaf(tmpLine,ori,name);
		SpatialLevel->Insert(tmpLine->min, tmpLine->max, tmpLeaf);

		D(cout << "> END   InsertLine." << endl);
	}

	class InsertIntervalArgs
	{
	public:
		Line* line;
		interval* timeInterval;
		bool orientation;

		InsertIntervalArgs();
		~InsertIntervalArgs() {};
		InsertIntervalArgs(Line* l, interval* i, bool o){
			line = l;
			timeInterval = i;
			orientation = o;
		};

		long objectId()
		{
			return timeInterval->value;
		}

	};

	static bool InsertTimeInterval(SpatialLeaf* id, void* arg)
	{
		D(cout << "    > TRYING InsertTimeInterval..." << endl);	

		InsertIntervalArgs* args = (InsertIntervalArgs*)arg;
		int objectId = args->objectId();
		Line* targetLine = args->line;
		Line* line = id->getLine();
		bool orientation = args->orientation;

		if(!targetLine->equals(*line)) return true;

		D(cout << "      > BEGIN InsertTimeInterval." << endl);	

		interval* tmpInterval = args->timeInterval;

		D(string arrow = orientation? "<--" : "-->";)
		D(cout << "        > Inserting.. id: "<<objectId<<" interval["<<tmpInterval->l<<","<<tmpInterval->r<<"] "<<arrow<<" into "<<id->nnn<< endl;)

		//id->getTemporalTree()->Insert(tmpInterval->start,tmpInterval->stop,make_pair(objectId,orientation));
		id->insertInterval(tmpInterval);
		
		D(cout << "      > END   InsertTimeInterval." << endl);
		return false;
	}

	void InsertTripSegment(long _objectId, int x1, int y1, int x2, int y2, double entranceTime, double exitTime)
	{
		D(cout << "> BEGIN InsertTripSegment." << endl;)

		Line* tmpLine = new Line(min(x1,x2), min(y1,y2), max(x1,x2), max(y1,y2));
		bool orientation = !(x1<x2);
		interval *tmpInterval = new interval(entranceTime,exitTime,_objectId);
		InsertIntervalArgs* args = new InsertIntervalArgs(tmpLine, tmpInterval, orientation);

		SpatialLevel->Search(tmpLine->min, tmpLine->max, this->InsertTimeInterval, (void*)args);

		D(cout << "> END   InsertTripSegment." << endl;)
	}

	class searchArgs
	{
	public:
		Line* sWindow;
		interval* tWindow;
		set<long>* resultArray;

		searchArgs();
		~searchArgs() {};
		searchArgs(Line* l, interval* i, set<long>* r){
			sWindow = l;
			tWindow = i;
			resultArray = r;
		};

	};

	static bool SegmentIntersectRectangle(
        int rectangleMinX,int rectangleMinY,
        int rectangleMaxX,int rectangleMaxY,
        int p1X,int p1Y,int p2X,int p2Y)
    {
        int minX = p1X, maxX = p2X;
        if (p1X > p2X)
        {
            minX = p2X;
            maxX = p1X;
        }
        if (maxX > rectangleMaxX)
        {
            maxX = rectangleMaxX;
        }
        if (minX < rectangleMinX)
        {
            minX = rectangleMinX;
        }
        if (minX > maxX)
        {
            return false;
        }
        int minY = p1Y, maxY = p2Y;
        double dx = p2X - p1X;
        if (dx != 0)
        {
            double a = (p2Y - p1Y)/dx;
            double b = p1Y - a*p1X;
            minY = a*minX + b;
            maxY = a*maxX + b;
        }
        if (minY > maxY)
        {
            int tmp = maxY;
            maxY = minY;
            minY = tmp;
        }

        if (maxY > rectangleMaxY)
        {
            maxY = rectangleMaxY;
        }

        if (minY < rectangleMinY)
        {
            minY = rectangleMinY;
        }

        if (minY > maxY)
        {
            return false;
        }
        return true;
    }

    static bool auxSpatialSearch(SpatialLeaf* id, void* arg)
    {

    	D(cout << "  > BEGIN auxSpatialSearch." << endl;)
    	D(cout << "    " << id->nnn;)
    	searchArgs* args = (searchArgs*)arg;
    	interval* temporalWindow = args->tWindow;
    	set<long>* resultArray = args->resultArray;

    	D(cout << " -> interval = [" << temporalWindow->l << ", " << temporalWindow->r << "]" << endl;)

    	vector<long> auxRes;
		//id->build();

    	id->getTemporalTree()->query(temporalWindow->l,temporalWindow->r,auxRes);
    	if (auxRes.size() > 0)
    	{
    		Line* sBox = args->sWindow;
	    	Line* lSeg = id->getLine();
	    	bool ori = id->getOrientation();
    		int p1X,p1Y, p2X,p2Y;
    		if(ori)
    		{
    			p1X = lSeg->min[0]; p1Y = lSeg->max[1];
    			p2X = lSeg->max[0]; p2Y = lSeg->min[1];
    		}
    		else
    		{
    			p1X = lSeg->min[0]; p1Y = lSeg->min[1];
    			p2X = lSeg->max[0]; p2Y = lSeg->max[1];	
    		}

    		if(SegmentIntersectRectangle(sBox->min[0],sBox->min[1],sBox->max[0],sBox->max[1],p1X, p1Y, p2X, p2Y))
    		{
    			for(int i=0;i<auxRes.size();i++)
    			{
    				//resultArray->insert(auxRes[i].value.first);

    				if (resultArray->insert(auxRes[i]).second){
    					cont_OK++;
    				} else {
    					cont_MISS++;
    				}

    			}
    		}
    	}

    	D(cout << "  > END   auxSpatialSearch." << endl;)
    	return true;
    }

	int Search(int x1, int y1, int x2, int y2, double entranceTime, double exitTime, set<long>* resultArray)
	{
		D(cout << "> BEGIN Search." << endl;)

					  	cont_OK = 0;
					  	cont_MISS = 0;

		resultArray->clear();
		Line* spatialWindow = new Line(min(x1,x2), min(y1,y2), max(x1,x2), max(y1,y2));
		interval* temporalWindow = new interval(entranceTime,exitTime,-1);
		searchArgs* args = new searchArgs(spatialWindow, temporalWindow, resultArray);

		SpatialLevel->Search(spatialWindow->min, spatialWindow->max, auxSpatialSearch, (void*)args);


							//cout << "OK = " << cont_OK << " , MISS = " << cont_MISS << endl;

		D(cout << "> END   Search." << endl;)
		return resultArray->size();
	}

	void Build()
	{
		RTree<SpatialLeaf*, int, 2, float>::Iterator it;
		SpatialLevel->GetFirst(it);

		while(! (SpatialLevel->IsNull(it)) )
		{
			(*it)->build();
			SpatialLevel->GetNext(it);
		}
	}

	size_t size()
	{
		size_t totalSize = sizeof(SpatialLevel);

		RTree<SpatialLeaf*, int, 2, float>::Iterator it;
		SpatialLevel->GetFirst(it);

		while(! (SpatialLevel->IsNull(it)) )
		{
			totalSize += (*it)->size();
			SpatialLevel->GetNext(it);
		}

		return totalSize;
	}

	pair<size_t,size_t> dSize()
	{
		size_t spatialSize = sizeof(SpatialLevel);
		size_t temporalSize = 0;

		RTree<SpatialLeaf*, int, 2, float>::Iterator it;
		SpatialLevel->GetFirst(it);

		while(! (SpatialLevel->IsNull(it)) )
		{
			spatialSize += (*it)->S_size();
			temporalSize += (*it)->T_size();
			SpatialLevel->GetNext(it);
		}

		return pair<size_t,size_t>(spatialSize,temporalSize);		
	}

	map<long,long> stats()
	{
		map<long,long> counters;

		RTree<SpatialLeaf*, int, 2, float>::Iterator it;
		SpatialLevel->GetFirst(it);

		while(! (SpatialLevel->IsNull(it)) )
		{
			counters[(*it)->leaf_stats()]++;
			SpatialLevel->GetNext(it);
		}

		return counters;
	}

}; 

#endif //CTREE_H
