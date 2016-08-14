//toy implementation of token bucket
#include <iostream>
#include <chrono>
#include <queue>
#include <mutex>
#include <thread>
#include <string>

class token_bucket {
public:
    // creates a string token bucket of size bucket_size_,
    // the bucket sends on average at most rate_ characters per time interval of interval_
    token_bucket(int bucket_size_, int rate_, int interval_) 
    : bucket_size(bucket_size_), rate(rate_), interval(interval_), 
      thread_refill_n_send(&token_bucket::run, this) 
    {};
    
    // deconstructor
    ~token_bucket() {
        active = false;
        thread_refill_n_send.join();
    }
    
    // receive a packet of string str, 
    // put packet to waiting queue unless the bucket is to be deconstructed
    bool receive(std::string str) {
        std::lock_guard<std::mutex> guard(m);

        if (active == false || str.size() > bucket_size) return false;
        
        packets.push(str);
        return true;
    }
    
private:
    const int bucket_size;  //max capacity for tokens
    const int rate;         //tokens generated per refill interval
    const int interval;     //length of refill interval

    std::queue<std::string> packets;   //packets waiting

    std::mutex m;           //concurrency lock
    bool active = true;     //the state of the bucket, is true before deconstruction

    std::thread thread_refill_n_send;
 
    void run() {
        int npacket = rate;
        auto t_init = std::chrono::steady_clock::now();
        auto t_last = t_init;

        while (active) {
            auto t_current = std::chrono::steady_clock::now();
            if (t_current > t_last + std::chrono::milliseconds(interval)) {
                npacket += rate;
                if (npacket > bucket_size) npacket = bucket_size;
            }

            while (!packets.empty()) {
                std::lock_guard<std::mutex> guard(m);
                int packet_size = packets.front().size();
                if (packet_size <= npacket) {
                    using namespace std::chrono;
                    send(duration_cast<duration<double>>(t_current-t_init).count(), 
                        packets.front());
                    packets.pop();
                    npacket -= packet_size;
                }
                else break;
            }
            std::this_thread::yield();
        } 
    }
  
    void send(int t, std::string str) {
        std::cout << t << ":" << "packet " << str << " sent." << std::endl;
    }
};

//example usage of the token_bucket class
int main() {
    token_bucket bucket(50, 5, 1000);
    for (int k = 1; k < 100; ++k) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        bucket.receive(std::to_string(k));
    }
}
