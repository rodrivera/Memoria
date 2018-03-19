#ifndef IS_IIS_H
#define IS_IIS_H

#define Dc(x)
//#define Dc(x) cout<<x<<endl

#define Dq(x)
//#define Dq(x) cout<<x<<endl

#include "sdsl/bit_vectors.hpp"
#include "sdsl/rank_support.hpp"
#include "sdsl/select_support.hpp"

#include <vector>

#include <climits>

using namespace sdsl;
using namespace std;

struct interval {
		double l,r;
		long value;
};

bool IntervalCompare(const interval& x, const interval& y) {
	if (x.l < y.l) return true;
	if (x.l > y.l) return false;
	if (x.r < y.r) return true; // same starting point: ascending order
	if (x.r > y.r) return false;
	return false;
}

class Stabbing{
private:
	long discr_multiplier;
  long domain_size;
  long independent_sets_number;

	vector<sd_vector<>*> sdv_start;
	vector<sd_vector<>*> sdv_stop;
	vector<rank_support_sd<>*> rs_start;
	vector<rank_support_sd<>*> rs_stop;
	vector<select_support_sd<>*> ss_start;
	vector<select_support_sd<>*> ss_stop;

  //int_vector<0> ids;
  vector<vector<vector<long> > > v_ids;

public:
	Stabbing(vector<interval>& intervals, long domainSize,long discrMultiplier = 100000){

		Dc("Construct BEGIN:");

		long num_intervals = intervals.size();
		domain_size = domainSize;
		discr_multiplier = discrMultiplier;

		Dc("    > structure vars set");

		sort(intervals.begin(), intervals.end(), IntervalCompare);
		//ids.width(ceil(log2(num_intervals)+1)); // ?!
		//ids.resize(num_intervals); 

		vector<vector<interval> > independent_sets;
		for (int i = 0; i < num_intervals; ++i) {
			long max_is_val = -1;
			long max_is_i = -1;
			for (int s = 0; s < independent_sets.size(); ++s) {

				long I_l = round(intervals[i].l * discr_multiplier);
				long I_r = round(intervals[i].r * discr_multiplier);
				long last_l = round(independent_sets[s].back().l * discr_multiplier);
				long last_r = round(independent_sets[s].back().r * discr_multiplier);

				if (I_r  > last_r && I_l != last_l || 
						I_r == last_r && I_l == last_l ) {
					if (independent_sets[s].back().r > max_is_val) {
						max_is_val = independent_sets[s].back().r;
						max_is_i = s;
					}
				}
			}
			if (max_is_i == -1){
				independent_sets.push_back( vector<interval>() );
				independent_sets.back().push_back(intervals[i]);
			} else {
				independent_sets[max_is_i].push_back(intervals[i]);
			}
		}

					    /*
							for (int i = 0; i < independent_sets.size(); ++i) {
								cout << endl << "#" << i << endl;
								for (int j = 0; j < independent_sets[i].size(); ++j) {
									cout << i << " - "<< independent_sets[i][j].value<<" ("<<independent_sets[i][j].l<< ","<<independent_sets[i][j].r<<") "<< endl;
								}
								cout << endl <<"~~~~~~~~~~" << endl;
							}
							*/
							
							

    independent_sets_number = independent_sets.size();

    v_ids.resize(independent_sets_number);
    sdv_start.resize(independent_sets_number);
    sdv_stop.resize(independent_sets_number);
    rs_start.resize(independent_sets_number);
		rs_stop.resize(independent_sets_number);
		ss_start.resize(independent_sets_number);
		ss_stop.resize(independent_sets_number);

		for (int iset = 0; iset < independent_sets.size(); ++iset) { // FOR EACH INDEPENDENT SET

			intervals = independent_sets[iset];
			num_intervals = independent_sets[iset].size();

			Dc("    > intervals sorted");

			v_ids[iset].resize(num_intervals);

			v_ids[iset][0].push_back(intervals[0].value);
			int cont = 0;
			for (int i = 1; i < intervals.size(); ++i) {
				if ( (intervals[i].l != intervals[i-1].l) || (intervals[i].r != intervals[i-1].r) ) {
					cont++;
				}
				v_ids[iset][cont].push_back(intervals[i].value);
			}
			Dc("  > ids created");

			bit_vector start = bit_vector(domain_size*discr_multiplier + 1, false);
		  bit_vector stop  = bit_vector(domain_size*discr_multiplier + 1, false);
		  Dc("  > bvs created");

		  for (int i=0; i < intervals.size(); ++i) {
				start[ round(intervals[i].l * discr_multiplier) ] = true;
				stop[ round(intervals[i].r * discr_multiplier) ] = true;
			}
			Dc("  > start/stop created");

			sdv_start[iset] = new sd_vector<>(start); 
			sdv_stop[iset] = new sd_vector<>(stop);
	    Dc("  > sdv created");

			rs_start[iset] = new rank_support_sd<>(sdv_start[iset]); 
			rs_stop[iset] = new rank_support_sd<>(sdv_stop[iset]);
		  Dc("  > rs created");

		  ss_start[iset] = new select_support_sd<>(sdv_start[iset]); 
		  ss_stop[iset] = new select_support_sd<>(sdv_stop[iset]);
		  Dc("  > ss created");

		  util::clear(start); 
		  util::clear(stop);
		  Dc("  > clear");
	  
	  	if ( (*rs_start[iset])(domain_size*discr_multiplier) != (*rs_stop[iset])(domain_size*discr_multiplier) ){
	  		cout << "WARNING: set #" << iset << "start-stop unbalanced!" << endl;
	  	}


		} // [FIN] FOR EACH INDEPENDENT SET

	  Dc("Construct DONE");
	}

	void query(const double& ql, const double& qr, std::vector<long>& output){
		output.clear();

		Dq("Query "<<ql<<" , "<<qr);

		if(! (0 <= ql && ql <= qr && qr <= domain_size) ){
			cout << "> q = [ " << ql << " , " << qr << " ]" << endl;
			cout << "> D = [ " << 1 << " , " << domain_size << " ]" << endl;
			cout << "Out of bounds query." << endl;
			return;
		}

	  long q_l = round(ql * discr_multiplier);
	  long q_r = round(qr * discr_multiplier);

	  Dq("discretized: "<<q_l<<" , "<<q_r);

	  for (int iset = 0; iset < independent_sets_number; ++iset) { // FOREACH INDEPENDENT SET
	  	Dq("  >iset "<<iset<<":");

		  long i_n_last = (*rs_start[iset])( q_r + 1 );
		  long i_n_first = (*rs_stop[iset])( q_l + 1 );

		  Dq("    > start with:");
		  Dq("      > i_n_first "<<i_n_first);
		  Dq("      > i_n_last "<<i_n_last);

		  if (i_n_first < 1) {
		  	i_n_first++;
		  }

		  long i_r = (*ss_stop[iset])( i_n_first ); if (i_r < q_l) i_n_first++;

		  Dq("      > i_l "<<(*ss_start[iset])( i_n_first ));
		  Dq("      > i_r "<<i_r);

		  for (long i_i = i_n_first; i_i <= i_n_last; i_i++) {
		  	Dq("    >"<<i_i<<" = ("<<(double)(*ss_start[iset])(i_i)/discr_multiplier<<", "<<(double)(*ss_stop[iset])(i_i)/discr_multiplier<<") : "<<v_ids[iset][i_i-1]);
		  	for (int i = 0; i < v_ids[iset][i_i-1].size(); ++i) {
	      	output.push_back( v_ids[iset][i_i-1][i] );
	      }
		  }
		
		} // [FIN] FOREACH INDEPENDENT SET
	  Dq("~~~" << endl);

	}
	size_t size() {

		size_t totalSize = sizeof(*this);
		Dc(cout << endl << "this = " << sizeof(*this));

		for (int is = 0; is < independent_sets_number; ++is){
			Dc(" > set #" << is);
			totalSize += size_in_bytes(*sdv_start[is]);
			Dc("    > sdv_start :" << size_in_bytes(*sdv_start[is]));
			totalSize += size_in_bytes(*sdv_stop[is]);
			Dc("    > sdv_stop :" << size_in_bytes(*sdv_stop[is]));
			totalSize += size_in_bytes(*rs_start[is]);
			Dc("    > ra_start :" << size_in_bytes(*rs_start[is]));
			totalSize += size_in_bytes(*rs_stop[is]);
			Dc("    > rs_stop :" << size_in_bytes(*rs_stop[is]));
			totalSize += size_in_bytes(*ss_start[is]);
			Dc("    > ss_start :" << size_in_bytes(*ss_start[is]));
			totalSize += size_in_bytes(*ss_stop[is]);
			Dc("    > ss_stop :" << size_in_bytes(*ss_stop[is]));

			//totalSize += v_ids[is].size()*sizeof(v_ids[0][0]);
			Dc("    > IDS :" << v_ids[is].size()*sizeof(v_ids[is][0]));
			Dc("        > num :" << v_ids[is].size());
			Dc("        > size :" << sizeof(v_ids[is][0][0]));
		}

		return totalSize;
	}
	long num_independent_sets() {
		return independent_sets_number;
	}
};

#endif