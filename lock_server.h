// this is the lock server
// the lock client has a similar interface

#ifndef lock_server_h
#define lock_server_h

#include <string>
#include "lock_protocol.h"
#include "lock_client.h"
#include "rpc/rpc.h"
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>

class locks_lid {
	enum stat{
		LOCKED,
		FREE
	};
	std::map<lock_protocol::lockid_t , stat> table;
	std::mutex mutex_;
	std::condition_variable condvar;
public:
	void locks_(lock_protocol::lockid_t lid);
	void unlocks_(lock_protocol::lockid_t lid);
	bool findlid_(lock_protocol::lockid_t lid);
};

class lock_server {

 protected:
  int nacquire;
  locks_lid lk_;
 public:
  lock_server();
  ~lock_server() {};
  lock_protocol::status stat(int clt, lock_protocol::lockid_t lid, int &);
  lock_protocol::status acquire(int clt, lock_protocol::lockid_t lid, int &);
  lock_protocol::status release(int clt, lock_protocol::lockid_t lid, int &);

};


#endif 







