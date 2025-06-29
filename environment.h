#ifndef ENV
#define ENV

#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Environment {
private:
    vector<unordered_map<string, int> > int_levels;
    vector<unordered_map<string, float> > float_levels;
    vector<unordered_map<string, bool>> bool_levels; 

    vector<unordered_map<string, string> > type_levels; // tipo de la variable

    vector<unordered_map<string, vector<int>>> array_int_levels;
    vector<unordered_map<string, vector<float>>> array_float_levels;
    vector<unordered_map<string, vector<bool>>> array_bool_levels;

    int search_rib(string var, string type) {
        if (type == "Int") {
            int idx = int_levels.size() - 1;
            while (idx >= 0) {
                if (int_levels[idx].find(var) != int_levels[idx].end()) {
                    return idx;
                }
                idx--;
            }
        } else if (type == "Float") {
            int idx = float_levels.size() - 1;
            while (idx >= 0) {
                if (float_levels[idx].find(var) != float_levels[idx].end()) {
                    return idx;
                }
                idx--;
            }
        } else if (type == "Boolean") { 
            int idx = bool_levels.size() - 1;
            while (idx >= 0) {
                if (bool_levels[idx].find(var) != bool_levels[idx].end()) {
                    return idx;
                }
                idx--;
            }
        }
        return -1; // F no encontrado
    }

public:

    Environment() { }

    void add_var(string var, int val, string type) {
        if (type == "Int") {
            int_levels.back()[var] = val;
        } else {
            cout << "Error: tipo de variable no coincide al añadir " << var << endl;
        }
        type_levels.back()[var] = type;
    }

    void add_var(string var, float val, string type) {
        if (type == "Float") {
            float_levels.back()[var] = val;
        } else {
            cout << "Error: tipo de variable no coincide al añadir " << var << endl;
        }
        type_levels.back()[var] = type;
    }

    void add_var(string var, bool val, string type) {
        if (type == "Boolean") {
            bool_levels.back()[var] = val;
        } else {
            cout << "Error: tipo de variable no coincide al añadir " << var << endl;
        }
        type_levels.back()[var] = type;
    }

    // método que añade una variable (se inicializa por defecto)
    void add_var(string var, string type) {
        if (type == "Int") {
            int_levels.back()[var] = 0;
        } else if (type == "Float") {
            float_levels.back()[var] = 0.0f;
        } else if (type == "Boolean") {
            bool_levels.back()[var] = false;
        }
        type_levels.back()[var] = type;
    }


    // métodos lookup para int, float y bool
    int lookup(string var) {
        int n = int_levels.size() - 1;
        while (n >= 0) {
            if (int_levels[n].find(var) != int_levels[n].end()) {
                return int_levels[n][var];
            }
            n--;
        }

        cout << "Error: variable no encontrada: " << var << endl;
        return 0;
    }

    float lookup_float(string var) {
        int n = float_levels.size() - 1;
        while (n >= 0) {
            if (float_levels[n].find(var) != float_levels[n].end()) {
                return float_levels[n][var];
            }
            n--;
        }
        cout << "Error: variable no encontrada: " << var << endl;
        return 0.0f;
    }

    bool lookup_bool(string var) {
        int n = bool_levels.size() - 1;
        while (n >= 0) {
            if (bool_levels[n].find(var) != bool_levels[n].end()) {
                return bool_levels[n][var];
            }
            n--;
        }
        cout << "Error: variable no encontrada: " << var << endl;
        return false;
    }

    // métodos update para int, float y bool
    bool update(string var, int val) {
        int n = int_levels.size() - 1;
        while (n >= 0) {
            if (int_levels[n].find(var) != int_levels[n].end()) {
                int_levels[n][var] = val;
                return true;
            }
            n--;
        }
        cout << "Error: variable no encontrada: " << var << endl;
        return false;
    }

    bool update(string var, float val) {
        int n = float_levels.size() - 1;
        while (n >= 0) {
            if (float_levels[n].find(var) != float_levels[n].end()) {
                float_levels[n][var] = val;
                return true;
            }
            n--;
        }
        cout << "Error: variable no encontrada: " << var << endl;
        return false;
    }

    bool update(string var, bool val) {
        int n = bool_levels.size() - 1;
        while (n >= 0) {
            if (bool_levels[n].find(var) != bool_levels[n].end()) {
                bool_levels[n][var] = val;
                return true;
            }
            n--;
        }
        cout << "Error: variable no encontrada: " << var << endl;
        return false; 
    }

    // ARRAYS
    void add_array(string var, const vector<int>& val) {
        array_int_levels.back()[var] = val;
        type_levels.back()[var] = "arrayOf<Int>";
    }
    void add_array(string var, const vector<float>& val) {
        array_float_levels.back()[var] = val;
        type_levels.back()[var] = "arrayOf<Float>";
    }
    void add_array(string var, const vector<bool>& val) {
        array_bool_levels.back()[var] = val;
        type_levels.back()[var] = "arrayOf<Boolean>";
    }
    void add_array(string var, string type) {
        if (type == "arrayOf<Int>") {
            array_int_levels.back()[var] = vector<int>();
        } else if (type == "arrayOf<Float>") {
            array_float_levels.back()[var] = vector<float>();
        } else if (type == "arrayOf<Boolean>") {
            array_bool_levels.back()[var] = vector<bool>();
        }
        type_levels.back()[var] = type;
    }
    vector<int>& lookup_array_int(string var) {
        int n = array_int_levels.size() - 1;
        while (n >= 0) {
            if (array_int_levels[n].find(var) != array_int_levels[n].end()) {
                return array_int_levels[n][var];
            }
            n--;
        }
        throw runtime_error("arrayOf<Int> no encontrado: " + var);
    }
    vector<float>& lookup_array_float(string var) {
        int n = array_float_levels.size() - 1;
        while (n >= 0) {
            if (array_float_levels[n].find(var) != array_float_levels[n].end()) {
                return array_float_levels[n][var];
            }
            n--;
        }
        throw runtime_error("arrayOf<Float> no encontrado: " + var);
    }
    vector<bool>& lookup_array_bool(string var) {
        int n = array_bool_levels.size() - 1;
        while (n >= 0) {
            if (array_bool_levels[n].find(var) != array_bool_levels[n].end()) {
                return array_bool_levels[n][var];
            }
            n--;
        }
        throw runtime_error("arrayOf<Boolean> no encontrado: " + var);
    }
    bool update_array(string var, int idx, int val) {
        int n = array_int_levels.size() - 1;
        while (n >= 0) {
            if (array_int_levels[n].find(var) != array_int_levels[n].end()) {
                if (idx >= 0 && idx < array_int_levels[n][var].size()) {
                    array_int_levels[n][var][idx] = val;
                    return true;
                } else {
                    return false; 
                }
            }
            n--;
        }
        return false;
    }
    bool update_array(string var, int idx, float val) {
        int n = array_float_levels.size() - 1;
        while (n >= 0) {
            if (array_float_levels[n].find(var) != array_float_levels[n].end()) {
                if (idx >= 0 && idx < array_float_levels[n][var].size()) {
                    array_float_levels[n][var][idx] = val;
                    return true;
                } else {
                    return false;
                }
            }
            n--;
        }
        return false;
    }
    bool update_array(string var, int idx, bool val) {
        int n = array_bool_levels.size() - 1;
        while (n >= 0) {
            if (array_bool_levels[n].find(var) != array_bool_levels[n].end()) {
                if (idx >= 0 && idx < array_bool_levels[n][var].size()) {
                    array_bool_levels[n][var][idx] = val;
                    return true;
                } else {
                    return false; // Found variable but index out of bounds
                }
            }
            n--;
        }
        return false;
    }

    string lookup_type(string var) {
        int n = type_levels.size() - 1;
        while (n >= 0) {
            if (type_levels[n].find(var) != type_levels[n].end()) {
                return type_levels[n][var];
            }
            n--;
        }
        return "";
    }
    bool check(string var) {
        int n = type_levels.size() - 1;
        while (n >= 0) {
            if (type_levels[n].find(var) != type_levels[n].end()) {
                return true;
            }
            n--;
        }
        return false;
    }

    void add_level() {
        int_levels.push_back(unordered_map<string, int>());
        float_levels.push_back(unordered_map<string, float>());
        bool_levels.push_back(unordered_map<string, bool>());
        array_int_levels.push_back(unordered_map<string, vector<int>>());
        array_float_levels.push_back(unordered_map<string, vector<float>>());
        array_bool_levels.push_back(unordered_map<string, vector<bool>>());
        type_levels.push_back(unordered_map<string, string>());
    }
    void remove_level() {
        int_levels.pop_back();
        float_levels.pop_back();
        bool_levels.pop_back();
        array_int_levels.pop_back();
        array_float_levels.pop_back();
        array_bool_levels.pop_back();
        type_levels.pop_back();
    }

    bool typecheck(const string& var, const string& expected_type) {
        string actual_type = lookup_type(var);
        return actual_type == expected_type;
    }
};

#endif