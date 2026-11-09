#include "storage.h"

string extractValue(const string& line, const string& key) {
    string searchKey = "\"" + key + "\"";
    size_t kpos = line.find(searchKey);
    if (kpos == string::npos) return "";
    
    size_t colon = line.find(":", kpos + searchKey.length());
    if (colon == string::npos) return "";
    
    size_t firstQuote = line.find("\"", colon);
    if (firstQuote == string::npos) return "";
    
    size_t secondQuote = line.find("\"", firstQuote + 1);
    if (secondQuote == string::npos) return "";
    
    return line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

vector<string> splitCSV(const string& csv, bool keepEmpty) {
    vector<string> result;
    string current = "";
    
    for (size_t i = 0; i < csv.length(); ++i) {
        char ch = csv[i];
        if (ch == ',') {
            if (!current.empty() || keepEmpty) {
                result.push_back(current);
            }
            current = "";
        } else {
            current += ch;
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    } else if (keepEmpty && !csv.empty() && csv.back() != ',') {
        result.push_back(current);
    }
    
    return result;
}

StorageMeta loadMeta(const string& filename) {
    StorageMeta meta;
    ifstream in(filename);
    if (!in.is_open()) return meta;

    string line;
    while (getline(in, line)) {
        if (line.find("|array") != string::npos) {
            size_t pipe = line.find("|");
            size_t quote = line.find("\"");
            if (quote != string::npos && pipe != string::npos && pipe > quote) {
                meta.arrayName = line.substr(quote + 1, pipe - quote - 1);
            }
        } else if (line.find("|flist") != string::npos) {
            size_t pipe = line.find("|");
            size_t quote = line.find("\"");
            if (quote != string::npos && pipe != string::npos && pipe > quote) {
                meta.singlyName = line.substr(quote + 1, pipe - quote - 1);
            }
        } else if (line.find("|dlist") != string::npos) {
            size_t pipe = line.find("|");
            size_t quote = line.find("\"");
            if (quote != string::npos && pipe != string::npos && pipe > quote) {
                meta.doublyName = line.substr(quote + 1, pipe - quote - 1);
            }
        } else if (line.find("|stack") != string::npos) {
            size_t pipe = line.find("|");
            size_t quote = line.find("\"");
            if (quote != string::npos && pipe != string::npos && pipe > quote) {
                meta.stackName = line.substr(quote + 1, pipe - quote - 1);
            }
        } else if (line.find("|queue") != string::npos) {
            size_t pipe = line.find("|");
            size_t quote = line.find("\"");
            if (quote != string::npos && pipe != string::npos && pipe > quote) {
                meta.queueName = line.substr(quote + 1, pipe - quote - 1);
            }
        } else if (line.find("|tree") != string::npos) {
            size_t pipe = line.find("|");
            size_t quote = line.find("\"");
            if (quote != string::npos && pipe != string::npos && pipe > quote) {
                meta.treeName = line.substr(quote + 1, pipe - quote - 1);
            }
        } else if (line.find("|set") != string::npos) {
            size_t pipe = line.find("|");
            size_t quote = line.find("\"");
            if (quote != string::npos && pipe != string::npos && pipe > quote) {
                meta.setName = line.substr(quote + 1, pipe - quote - 1);
            }
        } else if (line.find("|bst") != string::npos) {
            size_t pipe = line.find("|");
            size_t quote = line.find("\"");
            if (quote != string::npos && pipe != string::npos && pipe > quote) {
                meta.bstName = line.substr(quote + 1, pipe - quote - 1);
            }
        } else if (line.find("|ht") != string::npos) {
            size_t pipe = line.find("|");
            size_t quote = line.find("\"");
            if (quote != string::npos && pipe != string::npos && pipe > quote) {
                meta.htName = line.substr(quote + 1, pipe - quote - 1);
            }
        }
    }
    return meta;
}
