#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;


int main(int argc, char *argv[]) {
    // {    
    //     ifstream fin("../datasets/interactInfo.json");
    //     json data = json::parse(fin);

    //     cout << data.size() << endl;
    //     cout << "is_obj:" << data.is_object() << endl;
    //     cout << "is_array:" << data.is_array() << endl;

    //     int i = 0;
    //     for (json::iterator it = data.begin(); it != data.end(); ++it) {
    //         // for (json::iterator iit = it->begin(); iit != it->end(); ++iit) {
    //         //     cout << iit.key() << " : " << iit.value() << "\n";
    //         // }
    //         // cout << "\n";
    //         json& interacts = *it;
    //         json& comments = interacts["comment"];
    //         for (auto iit = comments.begin(); iit != comments.end(); ++iit) {
    //             cout << iit->at("user_id") << endl;
    //         }

    //         if (++i >= 10) {
    //             break;
    //         }
    //     }
    // }

    {
        json data;
        cout << data.is_null() << endl;
    }

    {
        json data = json::parse(R"({"name": "Gilbert", "wins": [["straight", "7♣"], ["one pair", "10♥"]]})");
        auto it = data.find("name");
        cout << *it << endl;
    }
    return 0;
}