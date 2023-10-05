// 9504853406CBAC39EE89AA3AD238AA12CA198043
#include <getopt.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <stack>
#include <queue>
#include "P2random.h"
using namespace std;

struct zombie {
    const string name;
    uint32_t distance;
    uint32_t speed;
    uint32_t health;
    uint32_t num_active_rounds;
    
    zombie(const string &n, uint32_t d, uint32_t s, uint32_t h)
    : name(n), distance(d), speed(s), health(h), num_active_rounds(1) {}

    
};

struct comparezombie {
    bool operator() (const zombie* z1, const zombie* z2) const {
        uint32_t z1_ETA = z1->distance / (z1->speed);
        uint32_t z2_ETA = z2->distance / (z2->speed);
        if (z1_ETA == z2_ETA) {
            if (z1->health == z2->health) {
                return z1->name > z2->name;
            }
            return z1->health > z2->health;
        }
        return z1_ETA > z2_ETA;
    }

};
struct Greater {
    bool operator()(const zombie* z1, const zombie* z2) const {
        if (z1->num_active_rounds == z2->num_active_rounds) {
            return z1->name > z2->name; 
        } else {
            return z1->num_active_rounds > z2->num_active_rounds; 
        }
    }
};


struct Less {
    bool operator()(const zombie* z1, const zombie* z2) const {
        if (z1->num_active_rounds == z2->num_active_rounds) {
            return z1->name > z2->name; 
        } else {
            return z1->num_active_rounds < z2->num_active_rounds; 
        }
    }
};


// struct minheap {
//     bool operator()(const uint32_t z1, const uint32_t z2) const {
//         return z1 > z2;
//     }
// };
// struct maxheap{ 
//     bool operator()(const uint32_t z1, const uint32_t z2) const {
//         return z1 < z2;
//     }
// };





class Game {

private: 
    uint32_t quiver_capacity;
    deque <zombie*> masterlist;
    vector<zombie*> deadzombies;
    priority_queue<zombie*, vector<zombie*>, comparezombie> pq;
    bool verbose = false;
    bool median = false;

    bool player_alive = true;

    uint32_t random_seed;
    uint32_t max_rand_distance;
    uint32_t max_rand_speed;
    uint32_t max_rand_health;
    uint32_t round = 0;
    const zombie* killer = nullptr;
    const zombie* last_died = nullptr;
    uint32_t N = 999999;
    uint32_t record_round = 0;
    uint32_t med = 0;

    priority_queue<uint32_t> lefthalf;
    priority_queue<uint32_t, vector<uint32_t>, greater<uint32_t>> righthalf;
    








public:

    void getMode(int argc, char * argv[]) {
            int opt;
            int option_index = 0;
        static struct option longOpts[] = {{ "verbose", no_argument, nullptr, 'v' },
                                        { "statistics", required_argument, nullptr, 's' },
                                        { "median", no_argument, nullptr, 'm' },
                                        { "help", no_argument, nullptr, 'h' },
                                        { nullptr, 0, nullptr, '\0' }};
            while ((opt = getopt_long(argc, argv, "vs:mh", longOpts, &option_index)) != -1) {

                switch (opt) {
                    case 'v':
                            verbose = true;
                            break;
                    case 's':
                            N = static_cast<uint32_t>(stoi(optarg));
                            break;
                    case 'm':
                            median = true;
                            break;
                    case 'h':
                            std::cout << "Usage: " << argv[0] << " [OPTIONS]...\n"
                            << "  -v, --verbose     :Display verbose messages.\n"
                            << "  -s, --statistics <num>   :Display <num> statistics entries\n"
                            << "  -m, --median      :Display median time zombies were active.\n"
                            << "  -h, --help        :Display this help and exit\n";
                            exit(0);
                }
            }
    }
    

    void read_input() {
        string read;
        getline (cin, read);
        while (cin >> read) {
            if(read =="quiver-capacity:" || read == "quiver" ){
                cin >> quiver_capacity;
            }
            else if (read =="random-seed:" || read == "seed"){
                cin >> random_seed;
            }
            else if (read == "max-rand-distance:" || read == "rand-dist" ){
                cin >> max_rand_distance;
            }
            else if (read == "max-rand-speed:" || read == "rand-speed"){
                cin >> max_rand_speed;
            }
            else if (read == "max-rand-health:" || read == "rand-health" ){
                cin >> max_rand_health;
            }
            else if (read =="---"){ 
                break;
            }
                    
        }
    }

    void create_zombie() {
        uint32_t number = 0;
        
        string temp;
        // cout << round << " " << record_round << endl;
        if (record_round == 0 ){
            cin >> temp >> record_round;
        }
        
    
        if (record_round == round && !cin.fail()) {
            cin >> temp;
            if (temp == "random-zombies:") {
                cin >> number;
            }
            for (uint32_t i = 0; i < number; i++) {
                zombie *newzombie = new zombie{
                P2random::getNextZombieName(),
                P2random::getNextZombieDistance(),
                P2random::getNextZombieSpeed(),
                P2random::getNextZombieHealth(),
                };
                masterlist.push_back(newzombie);
                pq.push(newzombie);
                if (verbose) {
                    cout << "Created: " << newzombie->name <<" (distance: "<<
                            newzombie->distance << ", speed: " << newzombie->speed << ", health: "<< newzombie->health<<")\n";
                }
                
            }

            cin >> temp;
            if (temp == "named-zombies:"){
                cin >> number;
            }
            for (uint32_t i = 0; i < number; i++) {
                string name;
                uint32_t distance, speed, health;
                cin >> name >> temp >> distance >> temp >> speed >> temp >> health;
                zombie* newzombie = new zombie{name, distance, speed, health, };
                masterlist.push_back(newzombie);
                pq.push(newzombie);
                if(verbose){
                    cout << "Created: " << newzombie->name <<" (distance: "<<
                            newzombie->distance << ", speed: " << newzombie->speed << ", health: "<< newzombie->health<<")"<<'\n';
                }
                
            
            }
            cin >> temp;
            if (!cin.fail() && temp == "---"){
                cin >> temp; 
                if (temp == "round:") {
                    cin >> record_round;   // PROBLEM: I need to clean the old data in the record_round?
                }
            }
            
        }


       
    //    while(!pq.empty()){
    //     cout << pq.top() << endl;
    //     pq.pop();
    //    }

        // for (uint32_t i = 0; i < pq.size(); i++){
        //     cout << pq.top()->name << '\n';
        //     pq.pop();
        // }

            
    }

// Each round, you should generate random zombies 
// before the player starts shooting zombies but after existing zombies have been updated.

    void roundplay() {
        bool first = 0;
        round++;
        if (verbose) {   // step 1
            cout << "Round: " << round << '\n';
        }
        
        
        // if (!pq.empty()) {
            for (auto z : masterlist) {    // 每一个zombie 开始移动
            if(z->health > 0){
                z->num_active_rounds++;
                // if (z->num_active_rounds > 1){    //进入第二轮之后都要开始move了
                
                z->distance -= min(z->distance, z->speed);
                
                // }
                if (z->distance == 0) {
                    if (!first) {
                        killer = z;
                        first = 1;
                    }
                    player_alive = false;
                    
                }
                if(verbose){
                        cout << "Moved: "  << z->name << " (distance: "
                        <<z->distance << ", speed: "<<z->speed<<
                        ", health: " << z->health <<")\n";
                }
            }
            }
        // }
        if (player_alive) create_zombie();

        // for (uint32_t i = 0; i < pq.size(); i++){
        //     cout << pq.top()->name << '\n';
        //     pq.pop();
        // }

        uint32_t arrows = quiver_capacity; // step2

        while (arrows > 0 && !pq.empty() && player_alive) {    // 射zombie
        
        
            zombie* target = pq.top();

            if (target->health > 0){ 

                if (target->health <= arrows) {
                    arrows -= target->health;
                    target->health = 0;
                }
                else {
                    // cout << target->health << " " << arrows << endl;
                    target->health -= arrows;
                    arrows = 0;
                    // cout << "updated" << target->health << " " << arrows << endl;
                }
            }
             
            if (target->health == 0) {  // 死掉了

                
                    if(N>0) deadzombies.push_back(target); // 放进我的 dead 里面，等下用来 output 第一个死的。。。
                    if(median) addNum(target->num_active_rounds);
                

                if (cin.fail() && pq.size() == 1) {
                    last_died = target;
                    player_alive = false;
                }
                
                
                if (verbose) {
                    cout << "Destroyed: " << target->name << " (distance: " << target->distance
                    << ", speed: " << target->speed << ", health: " << target->health << ")\n";
                }
                pq.pop();
            


            }
            
        }

        // median  
        if (median && !deadzombies.empty()) {
            // priority_queue<uint32_t> lefthalf;
            // priority_queue<uint32_t, vector<uint32_t>, greater<uint32_t>> righthalf;
            
            // for (uint32_t i = 0; i < deadzombies.size(); i++){
            //     righthalf.push(deadzombies[i]->num_active_rounds);
            //     if (righthalf.size() > lefthalf.size() + 1) {
            //         lefthalf.push(righthalf.top());
            //         righthalf.pop();
            //     }
                
            // }
            // check the last one
            // if(righthalf.top() < lefthalf.top()){
            //     lefthalf.push(righthalf.top());
            //     righthalf.pop();
            // }
            
            // if (lefthalf.size() == righthalf.size()){
            //     med = static_cast<uint32_t>((lefthalf.top() + righthalf.top()) / 2);
            // }
            // else if (lefthalf.size() > righthalf.size()){
            //     med = lefthalf.top();
            // }
            // else {
            //     med = righthalf.top();
            // }
            // if (killer == nullptr) {
            //     cout << "At the end of round "<< round <<", the median zombie lifetime is " << med << '\n';
            // }

            if( lefthalf.size() > righthalf.size()){
                med = lefthalf.top();
            } 
            else if(righthalf.size() > lefthalf.size()) {
                med = righthalf.top();
            }
            else {med = (righthalf.top() + lefthalf.top())/2;}

            if (killer == nullptr) {
                 cout << "At the end of round "<< round <<", the median zombie lifetime is " << med << '\n';
             }
        }
    }

    void addNum(uint32_t num) {
        lefthalf.push(num);
        if(!lefthalf.empty() && !righthalf.empty() && lefthalf.top() > righthalf.top()){
            righthalf.push(lefthalf.top());
            lefthalf.pop();
      
        }
       if( lefthalf.size() > righthalf.size() + 1){
            righthalf.push(lefthalf.top());
            lefthalf.pop();
            
        }

        if( righthalf.size() > lefthalf.size()+1){
            lefthalf.push(righthalf.top());
            righthalf.pop();
        }
    } 


    void statistic() {
        priority_queue<zombie*, std::vector<zombie*>, Greater> activeless(masterlist.begin(), masterlist.end());
        priority_queue<zombie*, std::vector<zombie*>, Less> activegreat(masterlist.begin(), masterlist.end());
        
        cout << "Zombies still active: " << masterlist.size() - deadzombies.size()  <<'\n';
       
        cout<< "First zombies killed:\n";
        uint32_t temp = (static_cast<uint32_t>(deadzombies.size()) < N) ? static_cast<uint32_t>(deadzombies.size()) : N;
        for(uint32_t i = 1; i <= temp; ++i){
            cout << deadzombies[i - 1]->name << " " << i <<'\n';
        }

        cout<<"Last zombies killed:\n";

        for(uint32_t i = 1; i <= temp; ++i){
            cout << deadzombies[deadzombies.size() - i]->name << " " << temp - i + 1 <<'\n';
        }
// print activeness 
        temp = (static_cast<uint32_t>(activegreat.size()) < N) ? static_cast<uint32_t>(activegreat.size()) : N;
        cout << "Most active zombies:\n";
        for(uint32_t i = 1; i <= temp; ++i){
            cout << activegreat.top()->name << " " << activegreat.top()->num_active_rounds <<'\n';
            activegreat.pop();
        }

        temp = (static_cast<uint32_t>(activeless.size()) < N) ? static_cast<uint32_t>(activeless.size()) : N;
        cout << "Least active zombies:\n";
        for(uint32_t i = 1; i <= temp; ++i){
            cout << activeless.top()->name << " " << activeless.top()->num_active_rounds <<'\n';
            activeless.pop();
        }
    }


    void game(){
            P2random::initialize(random_seed, max_rand_distance, max_rand_speed, max_rand_health);
            while (player_alive == true){
                roundplay();
            }
            
            if(killer){
                cout << "DEFEAT IN ROUND "<<round<<"! " << killer->name <<" ate your brains!" <<'\n';
            } else {
                cout << "VICTORY IN ROUND " <<round<<"! " << last_died->name<<" was the last zombie."<<'\n';
            }
                
            if(N != 999999){ statistic();}
            
            // clear dynamics;
            // while(!pq.empty()){
            //     pq.pop();
            // }
            // masterlist.clear();

            for(auto* z : masterlist) {
            delete z;  
        }
            masterlist.clear();
    }





};

int main(int argc, char * argv[]) {
    std::ios_base::sync_with_stdio(false);
    Game g;
    g.getMode(argc,argv);
    g.read_input();
    g.game();
    
    return 0;
}
 // the median output is for each round or all the deadzombies so far?
 // 
