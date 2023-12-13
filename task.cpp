#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
using namespace std;

class Entry {
public:
    string name;

    Entry(const string& n) : name(n) {}

    virtual ~Entry() = default;

    virtual void listEntries() const = 0;
    virtual Entry* getEntryByPath(const string& path) = 0;
    virtual void removeEntry(Entry* entry) = 0;
    virtual void addEntry(Entry* entry) = 0;
    virtual string getFullPath() const = 0;
    virtual void writeToFile(const string& content) {}
    virtual string readFromFile() const { return ""; }
    virtual void searchInFile(const string& pattern) const {}
    virtual Entry* copy() const = 0;
    virtual string getType() const = 0;

    virtual string to_string() const {
        return "Entry: " + name;
    }
};

class Directory : public Entry {
private:
    vector<Entry*> entries;
    Directory* parent;
public:
    Directory(const string& n, Directory* p = nullptr) : Entry(n), parent(p) {}

    ~Directory() override {
        for (Entry* entry : entries) {
            delete entry;
        }
    }

    void listEntries() const override {
        for (const Entry* entry : entries) {
            cout << entry->name << " ";
        }
        cout << endl;
    }

    Entry* getEntryByPath(const string& path) override {
        if (path == "..") {
            return parent;
        } else if (path == ".") {
            return this;
        } else {
            for (Entry* entry : entries) {
                if (entry->name == path) {
                    return entry;
                }
            }
            return nullptr;
        }
    }

    void removeEntry(Entry* entry) override {
        auto it = find(entries.begin(), entries.end(), entry);
        if (it != entries.end()) {
            entries.erase(it);
        }
    }

    void addEntry(Entry* entry) override {
        entry->parent = this;
        entries.push_back(entry);
    }

    string getFullPath() const override {
        if (parent) {
            return parent->getFullPath() + "/" + name;
        } else {
            return "/";
        }
    }

    Entry* copy() const override {
        Directory* newDirectory = new Directory(name);
        for (const Entry* entry : entries) {
            newDirectory->addEntry(entry->copy());
        }
        return newDirectory;
    }

    string getType() const override {
        return "Directory";
    }

    string to_string() const override {
        return "Directory: " + name;
    }
};

class File : public Entry {
private:
    string content;
public:
    File(const string& n, const string& c = "") : Entry(n), content(c) {}

    void writeToFile(const string& c) override {
        content = c;
    }

    string readFromFile() const override {
        return content;
    }

    void searchInFile(const string& pattern) const override {
        istringstream iss(content);
        string line;
        while (getline(iss, line)) {
            if (line.find(pattern) != string::npos) {
                cout << line << endl;
            }
        }
    }

    Entry* copy() const override {
        return new File(name, content);
    }

    string getType() const override {
        return "File";
    }

    string to_string() const override {
        return "File: " + name;
    }
};

class FileSystem {
private:
    Directory* root;
    Directory* currentDir;

public:
    FileSystem() : root(new Directory("/")), currentDir(root) {}

    ~FileSystem() {
        delete root;
    }

    void mkdir(const string& dirname) {
        Directory* newDir = new Directory(dirname);
        currentDir->addEntry(newDir);
    }

    void cd(const string& path) {
        if (path == "/") {
            currentDir = root;
        } else {
            Entry* entry = currentDir->getEntryByPath(path);
            if (entry && entry->getType() == "Directory") {
                currentDir = dynamic_cast<Directory*>(entry);
            }
        }
    }

    void ls(const string& path = ".") const {
        Entry* entry = currentDir->getEntryByPath(path);
        if (entry) {
            entry->listEntries();
        }
    }

    void grep(const string& pattern, const string& filename) const {
        Entry* entry = currentDir->getEntryByPath(filename);
        if (entry && entry->getType() == "File") {
            dynamic_cast<File*>(entry)->searchInFile(pattern);
        } else {
            cout << "File not found." << endl;
        }
    }

    void cat(const string& filename) const {
        Entry* entry = currentDir->getEntryByPath(filename);
        if (entry && entry->getType() == "File") {
            cout << dynamic_cast<File*>(entry)->readFromFile() << endl;
        } else {
            cout << "File not found." << endl;
        }
    }

    void touch(const string& filename) {
        File* newFile = new File(filename);
        currentDir->addEntry(newFile);
    }

    void echo(const string& content, const string& filename) {
        Entry* entry = currentDir->getEntryByPath(filename);
        if (entry && entry->getType() == "File") {
            dynamic_cast<File*>(entry)->writeToFile(content);
        } else {
            cout << "File not found." << endl;
        }
    }

    void mv(const string& source, const string& destination) {
        Entry* sourceEntry = currentDir->getEntryByPath(source);
        Directory* destinationDir = dynamic_cast<Directory*>(currentDir->getEntryByPath(destination));

        if (sourceEntry && destinationDir) {
            destinationDir->addEntry(sourceEntry->copy());
            currentDir->removeEntry(sourceEntry);
        }
    }

    void cp(const string& source, const string& destination) {
        Entry* sourceEntry = currentDir->getEntryByPath(source);
        Directory* destinationDir = dynamic_cast<Directory*>(currentDir->getEntryByPath(destination));

        if (sourceEntry && destinationDir) {
            destinationDir->addEntry(sourceEntry->copy());
        }
    }

    void rm(const string& path) {
        Entry* entry = currentDir->getEntryByPath(path);
        if (entry) {
            currentDir->removeEntry(entry);
            delete entry;
        }
    }

    void saveState(const string& path) const {
        ofstream outFile(path);
        if (outFile.is_open()) {
            outFile << root->to_string() << endl;
            outFile.close();
        } else {
            cout << "Unable to save state to file." << endl;
        }
    }

    void loadState(const string& path) {
        ifstream inFile(path);
        if (inFile.is_open()) {
            // Clear the current file system
            delete root;
            root = nullptr;
            currentDir = nullptr;

            // Load the state from the file
            unordered_map<string, Entry*> entriesMap;
            string line;
      while (getline(inFile, line)) {
                istringstream iss(line);
                string type;
                string name;
                iss >> type >> name;

                if (type == "Directory:") {
                    Directory* newDir = new Directory(name);
                    entriesMap[name] = newDir;

                    string parentName;
                    iss >> parentName;

                    if (parentName == "null") {
                        root = newDir;
                        currentDir = newDir;
                    } else {
                        newDir->parent = dynamic_cast<Directory*>(entriesMap[parentName]);
                    }
                } else if (type == "File:") {
                    File* newFile = new File(name);
                    entriesMap[name] = newFile;

                    string content;
                    getline(inFile, content); // Read content from the next line
                    newFile->writeToFile(content);
                }
            }
            inFile.close();
        } else {
            cout << "Unable to load state from file." << endl;
        }
    }
};

int main() {
    FileSystem fs;

    string savePath;
    string loadPath;

    cout << "Enter save path (or 'no' to skip saving): ";
    cin >> savePath;

    if (savePath != "no") {
        fs.saveState(savePath);
        cout << "State saved successfully." << endl;
    }

    cout << "Enter load path (or 'no' to skip loading): ";
    cin >> loadPath;

    if (loadPath != "no") {
        fs.loadState(loadPath);
        cout << "State loaded successfully." << endl;
    }

    cin.ignore(); // Clear the newline from the buffer

    while (true) {
        string command;
        cout << "Enter command (or 'exit' to quit): ";
        getline(cin, command);

        if (command == "exit") {
            if (!savePath.empty()) {
                fs.saveState(savePath);
                cout << "State saved successfully." << endl;
            }
            break;
        } else {
            // Split command into tokens
            istringstream iss(command);
            vector<string> tokens(istream_iterator<string>{iss}, istream_iterator<string>());

            // Execute the command
            if (!tokens.empty()) {
                if (tokens[0] == "ls") {
                    fs.ls(tokens.size() > 1 ? tokens[1] : ".");
                } else if (tokens[0] == "mkdir" && tokens.size() > 1) {
                    fs.mkdir(tokens[1]);
                } else if (tokens[0] == "cd" && tokens.size() > 1) {
                    fs.cd(tokens[1]);
                } else if (tokens[0] == "grep" && tokens.size() > 2) {
                    fs.grep(tokens[1], tokens[2]);
                } else if (tokens[0] == "cat" && tokens.size() > 1) {
                    fs.cat(tokens[1]);
                } else if (tokens[0] == "touch" && tokens.size() > 1) {
                    fs.touch(tokens[1]);
                } else if (tokens[0] == "echo" && tokens.size() > 2) {
                    ostringstream content;
                    for (size_t i = 2; i < tokens.size(); ++i) {
                        content << tokens[i] << " ";
                    }
                    fs.echo(content.str(), tokens[1]);
                } else if (tokens[0] == "mv" && tokens.size() > 2) {
                    fs.mv(tokens[1], tokens[2]);
                } else if (tokens[0] == "cp" && tokens.size() > 2) {
                    fs.cp(tokens[1], tokens[2]);
                } else if (tokens[0] == "rm" && tokens.size() > 1) {
                    fs.rm(tokens[1]);
                } else {
                    cout << "Invalid command. Try again." << endl;
                }
            }
        }
    }
    return 0;
}
