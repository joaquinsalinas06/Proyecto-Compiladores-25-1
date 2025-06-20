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
        return -1; // Not found
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

    // Método para añadir una variable sin valor inicial (se inicializa por defecto)
    void add_var(string var, string type) {
        if (type == "Int") {
            int_levels.back()[var] = 0;
        } else if (type == "Float") {
            float_levels.back()[var] = 0.0f;
        } else if (type == "Boolean") { // ¡NUEVO! Inicialización por defecto para booleanos
            bool_levels.back()[var] = false;
        }
        type_levels.back()[var] = type;
    }


    // Métodos lookup (ya los tienes para int y float, aquí completo para bool)
    int lookup(string var) {
        int n = int_levels.size() - 1;
        while (n >= 0) {
            if (int_levels[n].find(var) != int_levels[n].end()) {
                return int_levels[n][var];
            }
            n--;
        }
        // Considera lanzar una excepción o un valor por defecto si no se encuentra
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
        return 0.0f;
    }

    // ¡NUEVO! Método para buscar el valor de una variable booleana
    bool lookup_bool(string var) {
        int n = bool_levels.size() - 1;
        while (n >= 0) {
            if (bool_levels[n].find(var) != bool_levels[n].end()) {
                return bool_levels[n][var];
            }
            n--;
        }
        // Si no se encuentra, devolvemos false por defecto (o lanzar excepción)
        return false;
    }

    // Métodos update (ya los tienes para int y float, aquí completo para bool)
    bool update(string var, int val) {
        int n = int_levels.size() - 1;
        while (n >= 0) {
            if (int_levels[n].find(var) != int_levels[n].end()) {
                int_levels[n][var] = val;
                return true;
            }
            n--;
        }
        return false; // Variable no encontrada
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
        return false;
    }

    // ¡NUEVO! Método para actualizar el valor de una variable booleana
    bool update(string var, bool val) {
        int n = bool_levels.size() - 1;
        while (n >= 0) {
            if (bool_levels[n].find(var) != bool_levels[n].end()) {
                bool_levels[n][var] = val;
                return true;
            }
            n--;
        }
        return false; // Variable no encontrada
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
        bool_levels.push_back(unordered_map<string, bool>()); // ¡Asegúrate de que esto esté aquí!
        type_levels.push_back(unordered_map<string, string>());
    }
    void remove_level() {
        int_levels.pop_back();
        float_levels.pop_back();
        bool_levels.pop_back(); // ¡Asegúrate de que esto esté aquí!
        type_levels.pop_back();
    }

    // Verifica el tipo de una variable antes de asignar valor
    bool typecheck(const string& var, const string& expected_type) {
        string actual_type = lookup_type(var);
        return actual_type == expected_type;
    }
};

#endif