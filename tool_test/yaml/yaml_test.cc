#include <iostream>
#include <assert.h>
#include <yaml-cpp/yaml.h>
#include <string>

using namespace std;

void print(std::pair<YAML::Node, YAML::Node>& p) {
    if(p.first.IsMap()) {
        cout << p.first << ": " << endl;
    }
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    // 读取yaml文件
    YAML::Node yaml_node = YAML::LoadFile(argv[1]);
    for (auto it : yaml_node) {
        if(it.second.IsMap()) {
            cout << it.first << ": " << endl;
        } else {
            cout << it.first << endl;
        }
        if(it.second.IsMap()) {
            for(auto itt: it.second) {
                cout << "  " << itt.first << ":" << itt.second << endl;
            }
        } else if(it.second.IsDefined()) {
            cout  << it.second << endl;
        }
	}

    return 0;
}