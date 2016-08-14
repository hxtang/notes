#include <iostream>
#include <chrono>
#include <queue>
#include <mutex>
#include <thread>
#include <string>

class leaky_bucket {
public:
    // creates a string leaky bucket of size bucket_size_, and sends at most rate_ characters per time interval of interval_
    leaky_bucket(int bucket_size_, int rate_, int interval_) 
    : bucket_size(bucket_size_), rate(rate_), interval(interval_), capacity(bucket_size_), thread_refill_n_send(&leaky_bucket::run, this) 
    {};
    
    // deconstructor
    ~leaky_bucket() {
        active = false;
        thread_refill_n_send.join();
    }

    // receive a packet of string str, 
    // return false abd drop packet if its size is larger than bucket hole or bucket overflows
    // put packet to waiting queue otherwise
    bool receive(std::string str) {
        std::lock_guard<std::mutex> guard(m);

        if (active == false) return false;
        int packet_size = str.size();
        if (packet_size > rate || packet_size > capacity) return false; 
        
        packets.push(str);
        capacity -= packet_size;
        return true;
    }
    
private:
    const int bucket_size;  //volume of packets in queue
    const int rate;         //volume of packets sent per refill interval
    const int interval;     //length of refill interval

    int capacity;           //available space in bucket
    std::queue<std::string> packets;  //incoming packets not sent yet

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
                npacket = rate;
                t_last = t_current;
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
                    capacity += packet_size;
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

//example usage of the leaky_bucket class
int main() {
    leaky_bucket bucket(50, 5, 1000);
    for (int k = 1; k < 100; ++k) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        bucket.receive(std::to_string(k));
    }
}
