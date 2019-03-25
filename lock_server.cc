// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server():
  nacquire (0)
{
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
 /* if ( locks_for_lid.findlid(lid) ) {
	  ret = lock_protocol::OK;
  }
  else {
	  ret = lock_protocol::RETRY;
  }*/
  return ret;
}

lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r) {
	printf("acquire request from clt %d\n",clt);
	r = nacquire;
	lk_.locks_(lid);
	return lock_protocol::OK;
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r) {
	printf("release request from clt %d\n",clt);
	r = nacquire;
	lk_.unlocks_(lid);
	return lock_protocol::OK;
}

bool locks_lid::findlid_(lock_protocol::lockid_t lid) {
	if ( table.find(lid) == table.end() ) {
		table[lid] = FREE;
	   return true;
	}
	else if( table[lid] == LOCKED ) {
		return false;
	}
	else {
		return true;
	}
}	

void locks_lid::locks_(lock_protocol::lockid_t lid) {
	//std::unique_lock<std::mutex> lk(mutex_);
	pthread_mutex_lock( &mutex_ );
	if (findlid_(lid) ) {
		table[lid] = LOCKED;
	}
	else {
		//condvar.wait(lk,[&](){return table[lid] == FREE;} );
		auto flamda = [&](){return table[lid] == FREE;}; 
		while(!flamda() ) pthread_cond_wait( &condvar, &mutex_ );
		table[lid] = LOCKED;
	}
	pthread_mutex_unlock( &mutex_ );
}

void locks_lid::unlocks_(lock_protocol::lockid_t lid) {
	//std::unique_lock<std::mutex> lk(mutex_);
	pthread_mutex_lock( &mutex_ );
	if(!findlid_(lid)) {
		table[lid] = FREE;
	}
	//condvar.notify_all();
	pthread_cond_broadcast( &condvar );
	pthread_mutex_unlock( &mutex_ );
}	
