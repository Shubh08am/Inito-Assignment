```
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
using namespace std;

/*This defines an abstract base class Entry representing either a directory or a file.
It contains pure virtual functions and some common functions that need to be implemented by its derived classes.
*/
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

/*This defines a class Directory that inherits from Entry.
It represents a directory and contains a vector of Entry pointers for its contents (entries).
It also has a pointer to its parent directory (parent).
*/
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
/*This function prints the names of entries (files and subdirectories) within the current directory.*/
    void listEntries() const override {
        for (const Entry* entry : entries) {
            cout << entry->name << " ";
        }
        cout << endl;
    }

/*This function returns an Entry pointer given a path (file or directory name).
It handles special cases like ".." (parent directory) and "." (current directory).
*/
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

/*This function removes an entry from the current directory.*/
    void removeEntry(Entry* entry) override {
        auto it = find(entries.begin(), entries.end(), entry);
        if (it != entries.end()) {
            entries.erase(it);
        }
    }
/*This function adds an entry to the current directory.*/
    void addEntry(Entry* entry) override {
        entry->parent = this;
        entries.push_back(entry);
    }
/*This function returns the full path of the current directory, including its parent directories.*/
    string getFullPath() const override {
        if (parent) {
            return parent->getFullPath() + "/" + name;
        } else {
            return "/";
        }
    }
/*This function creates a deep copy of the current directory and its contents.*/
    Entry* copy() const override {
        Directory* newDirectory = new Directory(name);
        for (const Entry* entry : entries) {
            newDirectory->addEntry(entry->copy());
        }
        return newDirectory;
    }
/*This function returns the type of the entry, indicating that it's a directory.*/
    string getType() const override {
        return "Directory";
    }
/*This function provides a string representation of the directory.*/
    string to_string() const override {
        return "Directory: " + name;
    }
};

/*This defines a class File that inherits from Entry. It represents a file and contains a string for its content.*/
class File : public Entry {
private:
    string content;
public:
    File(const string& n, const string& c = "") : Entry(n), content(c) {}

    void writeToFile(const string& c) override {
        content = c;
    }
/*This function returns the content of the file.*/
    string readFromFile() const override {
        return content;
    }
/*This function searches for a pattern in the content of the file and prints matching lines.*/
    void searchInFile(const string& pattern) const override {
        istringstream iss(content);
        string line;
        while (getline(iss, line)) {
            if (line.find(pattern) != string::npos) {
                cout << line << endl;
            }
        }
    }
/*This function creates a deep copy of the file.*/
    Entry* copy() const override {
        return new File(name, content);
    }
/*This function returns the type of the entry, indicating that it's a file.*/
    string getType() const override {
        return "File";
    }
/*This function provides a string representation of the file.*/
    string to_string() const override {
        return "File: " + name;
    }
};

/*This defines a class FileSystem representing the entire file system.
It contains pointers to the root directory (root) and the current working directory (currentDir).
*/
class FileSystem {
private:
    Directory* root;
    Directory* currentDir;

public:
/*The constructor initializes the file system with a root directory ("/") and sets the current directory to the root.*/
    FileSystem() : root(new Directory("/")), currentDir(root) {}

    ~FileSystem() {
        delete root;
    }
/*This function creates a new directory within the current directory.*/
    void mkdir(const string& dirname) {
        Directory* newDir = new Directory(dirname);
        currentDir->addEntry(newDir);
    }
/*This function changes the current directory based on the provided path.
It handles absolute paths ("/"), parent directory (".."), and subdirectories.
*/
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
/*This function lists the entries (files and subdirectories) within the specified directory (default is the current directory).*/
    void ls(const string& path = ".") const {
        Entry* entry = currentDir->getEntryByPath(path);
        if (entry) {
            entry->listEntries();
        }
    }
/*This function searches for a pattern within the content of a file and prints matching lines.*/
    void grep(const string& pattern, const string& filename) const {
        Entry* entry = currentDir->getEntryByPath(filename);
        if (entry && entry->getType() == "File") {
            dynamic_cast<File*>(entry)->searchInFile(pattern);
        } else {
            cout << "File not found." << endl;
        }
    }
/*This function displays the content of a file.*/
    void cat(const string& filename) const {
        Entry* entry = currentDir->getEntryByPath(filename);
        if (entry && entry->getType() == "File") {
            cout << dynamic_cast<File*>(entry)->readFromFile() << endl;
        } else {
            cout << "File not found." << endl;
        }
    }
/*This function creates a new empty file within the current directory.*/
    void touch(const string& filename) {
        File* newFile = new File(filename);
        currentDir->addEntry(newFile);
    }
/*This function writes text content to a file or creates a new file with the specified content.*/
    void echo(const string& content, const string& filename) {
        Entry* entry = currentDir->getEntryByPath(filename);
        if (entry && entry->getType() == "File") {
            dynamic_cast<File*>(entry)->writeToFile(content);
        } else {
            cout << "File not found." << endl;
        }
    }
/*This function moves a file or directory from the source path to the destination path.*/
    void mv(const string& source, const string& destination) {
        Entry* sourceEntry = currentDir->getEntryByPath(source);
        Directory* destinationDir = dynamic_cast<Directory*>(currentDir->getEntryByPath(destination));

        if (sourceEntry && destinationDir) {
            destinationDir->addEntry(sourceEntry->copy());
            currentDir->removeEntry(sourceEntry);
        }
    }
/*This function copies a file or directory from the source path to the destination path.*/
    void cp(const string& source, const string& destination) {
        Entry* sourceEntry = currentDir->getEntryByPath(source);
        Directory* destinationDir = dynamic_cast<Directory*>(currentDir->getEntryByPath(destination));

        if (sourceEntry && destinationDir) {
            destinationDir->addEntry(sourceEntry->copy());
        }
    }
/*This function removes a file or directory from the file system.*/
    void rm(const string& path) {
        Entry* entry = currentDir->getEntryByPath(path);
        if (entry) {
            currentDir->removeEntry(entry);
            delete entry;
        }
    }
/*This function saves the current state of the file system to a file.*/
    void saveState(const string& path) const {
        ofstream outFile(path);
        if (outFile.is_open()) {
            outFile << root->to_string() << endl;
            outFile.close();
        } else {
            cout << "Unable to save state to file." << endl;
        }
    }
/*This function loads the file system state from a file, creating directories and files based on the information stored in the file.*/
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
```
