#include<iostream>
#include<filesystem>
#include<fstream>
#include <stdexcept>
#include<openssl/sha.h>
#include<cstdio>
#include<map>
using namespace std;
namespace fs = std::filesystem;

string readFile(const string& filename) {

    ifstream file(filename, ios::binary);

    if(!file) {
        throw runtime_error("Cannot open file");
    }

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    return content;
}

string sha256(const string& data) {
    //calculation of hash but in byte form and not in hexadecimal form
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,data.c_str(),data.size());
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash,&ctx);

    //conversion of hash to hexadecimal form

    string result="";

    for(int i=0;i<SHA256_DIGEST_LENGTH;i++) {
        unsigned char byte = hash[i];
        //high nibble
        unsigned char high = byte/16; 

        if(high<10) {
            result+= ('0' + high);
        }
        else {
            result += ('a' + (high - 10));
        }

        //low nibble
        unsigned char low = byte%16;
        if(low<10) {
            result += ('0' + low);
        }
        else {
            result+= ('a' + (low - 10));
        }
    }

    return result;
}

string createTree() {
    string treeContent = "";

    ifstream indexFile(".mygit/index");

    if(!indexFile) {
        cerr << "No files staged.\n";
        return "";
    }

    string filename;
    string hash;

    while(indexFile >> filename >> hash) {
        treeContent += filename + " " + hash + "\n";
    }
    indexFile.close();

    string treeHash = sha256(treeContent);

    string objectPath = ".mygit/objects/" + treeHash;

    if(!fs::exists(objectPath)) {

        ofstream treeFile(objectPath);

        if(!treeFile) {
            cerr << "Failed to create tree object\n";
            return "";
        }

        treeFile << treeContent;
        treeFile.close();

        cout << "Created tree: " << treeHash << '\n';
    }
    else {
        cout << "Tree already exists. Reusing it.\n";
    }

    return treeHash;
}

string createCommit(string message) {
    string treeHash = createTree();

    if(treeHash.empty()) {
        return "";
    }

    ifstream headFile(".mygit/HEAD");

    if(!headFile) {
        cerr << "Failed to read HEAD\n";
        return "";
    }

    string headContent;
    getline(headFile, headContent);
    headFile.close();

    cout<< "HEAD: " << headContent <<'\n';

    string branchRef = headContent.substr(5);  //contains ref/heads/main

    cout << "Current branch: " << branchRef << '\n';

    fs::path branchPath = fs::path(".mygit") / branchRef;

    cout<< "Branch path: " << branchPath << '\n';

    //checking whether previous commit exists or not
    
    string parentHash = "";

    if(fs::exists(branchPath)) {

        ifstream branchFile(branchPath);

        if(!branchFile) {
            cerr<< "Failed to read branch reference.\n";
            return "";
        }

        getline(branchFile, parentHash);
        branchFile.close();

        cout << "Previous commit: " << parentHash << '\n';
    }
    else {
        cout << "No previous commit. This is the first commit.\n";
    }

    string commitContent;

    commitContent += "tree " + treeHash + '\n';

    if(!parentHash.empty()) {
        commitContent += "Parent " + parentHash + '\n';
    }

    commitContent += "author Pavan\n";
    commitContent += "message " + message + "\n";

    string commitHash = sha256(commitContent);

    //storing commit actually

    fs::path commitPath = fs::path(".mygit") / "objects" / commitHash;

    if(!fs::exists(commitPath)) {
        ofstream commitFile(commitPath);

        if(!commitFile) {
            cerr << "Failed to create commit object\n";
            return "";
        }

        commitFile << commitContent;
        commitFile.close();

        cout << "Created commit: " << commitHash << '\n';
    }
    else {
        cout << "Commit already exists. Reusing it.\n";
    }

    ofstream branchFile(branchPath);

    if(!branchFile) {
        cerr << "Failed to update branch.\n";
        return "";
    }

    branchFile << commitHash << '\n';
    branchFile.close();

    cout << "Updated branch: " << branchRef << '\n';

    cout << "Commit hash: " << commitHash << '\n';



    return commitHash;
}

map<string, string> readIndex() {
    map<string, string> index;

    ifstream indexFile(".mygit/index");

    if(!indexFile) {
        return index;
    }

    string filename;
    string hash;

    while(indexFile >> filename >> hash) {
        index[filename] = hash;
    }

    indexFile.close();

    return index;
}

void writeIndex(const map<string, string>& index) {

    ofstream indexFile(".mygit/index");

    if(!indexFile) {
        cerr << "Failed to write index.\n";
        return;
    }

    for(const auto& entry : index) {
        indexFile << entry.first << " " << entry.second << '\n';
    }

    indexFile.close();
}

void createBranch(string branchName) {
    fs::path branchPath = fs::path(".mygit") / "refs" / "heads" / branchName;

    if(fs::exists(branchPath)) {
        cerr << "Branch already exists.\n";
        return;
    }

    ifstream headFile(".mygit/HEAD");

    if(!headFile) {
        cerr << "Failed to read HEAD.\n";
        return;
    }

    string headContent;
    getline(headFile, headContent);
    headFile.close();

    string branchRef = headContent.substr(5);

    fs::path currentBranchPath = fs::path(".mygit") / branchRef;

    ifstream branchFile(currentBranchPath);

    if(!branchFile) {
        cerr << "No commits yet.\n";
        return;
    }

    string currentCommit;
    getline(branchFile, currentCommit);
    branchFile.close();

    ofstream newBranch(branchPath);

    if(!newBranch) {
        cerr << "Failed to create branch.\n";
        return;
    }

    newBranch << currentCommit << '\n';
    newBranch.close();

    cout << "Created branch " << branchName << " at " << currentCommit << '\n';
}

map<string, string> readTree(string treeHash) {

    map<string, string> tree;

    fs::path treePath = fs::path(".mygit") / "objects" /treeHash;

    ifstream treeFile(treePath);

    if(!treeFile) {
        cerr << "Failed to read tree.\n";
        return tree;
    }

    string filename;
    string hash;

    while(treeFile >> filename >> hash) {
        tree[filename] = hash;
    }

    treeFile.close();
    return tree;
}

void restoreTree(string treeHash) {

    map<string, string> tree = readTree(treeHash);

    for(const auto& entry : tree) {

        string filename = entry.first;
        string blobHash = entry.second;

        fs::path blobPath = fs::path(".mygit") / "objects" / blobHash;

        ifstream blobFile(blobPath, ios::binary);

        if(!blobFile) {
            cerr << "Failed to read blob: " << blobHash << '\n';
            continue;
        }

        string content((istreambuf_iterator<char>(blobFile)),istreambuf_iterator<char>());

        blobFile.close();

        ofstream outputFile(filename, ios::binary);

        if(!outputFile) {
            cerr << "Failed to restore " << filename << '\n';
            continue;
        }

        outputFile.write(content.data(), content.size());
        outputFile.close();
    }
}

string getTreeFromCommit(string commitHash) {

    fs::path commitPath = fs::path(".mygit") / "objects" / commitHash;

    ifstream commitFile(commitPath);

    if(!commitFile) {
        cerr << "Failed to read commit.\n";
        return "";
    }

    string line;

    while(getline(commitFile, line)) {

        if(line.rfind("tree ",0) ==0) {
            commitFile.close();
            return line.substr(5);
        }
    }
    commitFile.close();

    cerr << "Tree not found in commit.\n";
    return "";
}

void removeFilesNotInTree(const map<string, string>& tree) {

    map<string, string> index = readIndex();

    for(const auto& entry : index) {

        string filename = entry.first;

        if(tree.find(filename) == tree.end()) {

            if(fs::exists(filename)) {
                fs::remove(filename);

                cout << "Removed: " << filename << '\n';
            }
        }
    }
}

bool hasUncommittedChanges() {

    map<string, string> index = readIndex();

    for(const auto& entry : index) {

        string filename = entry.first;
        string stagedHash = entry.second;

        if(!fs::exists(filename)) {
            return true;
        }

        ifstream file(filename, ios::binary);

        string content(
            (istreambuf_iterator<char>(file)),
            istreambuf_iterator<char>()
        );

        file.close();

        string currentHash = sha256(content);

        if(currentHash != stagedHash) {
            return true;
        }
    }

    return false;
}

void checkoutBranch(string branchName) {
    fs::path branchPath = fs::path(".mygit") / "refs" / "heads" / branchName;

    if(!fs::exists(branchPath)) {
        cerr << "Branch does not exist.\n";
        return;
    }

    if(hasUncommittedChanges()) {
    cerr << "Your local changes would be overwritten by checkout.\n";
    return;
    }

    ifstream branchFile(branchPath);

    if(!branchFile) {
        cerr << "Failed to read branch.\n";
        return;
    }

    string commitHash;
    getline(branchFile, commitHash);
    branchFile.close();

    string treeHash = getTreeFromCommit(commitHash);

    if(treeHash.empty()) {
        return;
    }

    map<string , string> tree = readTree(treeHash);
    removeFilesNotInTree(tree);
    restoreTree(treeHash);
    writeIndex(tree);

    ofstream headFile(".mygit/HEAD");

    if(!headFile) {
        cerr << "Failed to update HEAD.\n";
        return;
    }

    headFile << "ref: refs/heads/" << branchName << '\n';
    headFile.close();

    cout << "Switched to branch " << branchName << '\n';
}




int main(int argc,char* argv[]) {
    if(argc<2) {
        cout<<"Usage: mygit <command>\n";
        return 1;
    }

    string command = argv[1];

    if(command == "init") {
        fs::path gitDir = ".mygit";

        if(fs::exists(gitDir)) {
            cout<<"Repository is already initialized\n";
            return 0;
        }
        fs::create_directory(gitDir);
        fs::create_directory(gitDir / "objects");
        fs::create_directories(gitDir / "refs" / "heads");

        ofstream head(gitDir / "HEAD");

        if(!head) {
            cerr << "Failed to create HEAD\n";
            return 1;
        }

        head << "ref: refs/heads/main\n";
        head.close();

        cout<<"Initialized an empty Mygit repository\n";
        return 0;
    }
    else if(command == "add") {
        if(argc < 3) {
            cerr<< "Usage: mygit add <file>\n";
            return 1;
        }

        if(!fs::exists(".mygit")) {
            cerr<< "Not a Mygit repository\n";
            return 1;
        }

        string filename = argv[2];

        string content = readFile(filename);

        string hash = sha256(content);

        fs::path objectPath = fs::path(".mygit") / "objects" / hash;

        if(!fs::exists(objectPath)) {

            ofstream blobFile(objectPath, ios::binary);

            if(!blobFile) {
                cerr << "Failed to create object\n";
                return 1;
            }

            blobFile.write(content.data(), content.size());

            blobFile.close();

            cout<< "Created object: " << hash << '\n';
        }
        else {
            cout<< "Object already exists. Reusing it.\n";
        }

        fs::path indexPath = fs::path(".mygit") / "index";

        map<string,string> index;

        ifstream indexFile(indexPath);

        string storedFilename;
        string storedHash;

        while(indexFile >> storedFilename >> storedHash) {
            index[storedFilename] = storedHash;
        }
        indexFile.close();

        index[filename] = hash;

        ofstream indexOut(indexPath);

        if(!indexOut) {
            cerr<< "Failed to write index\n";
            return 1;
        }

        for(const auto& entry : index) {
            indexOut << entry.first <<" " << entry.second <<'\n';
        }

        indexOut.close();

        cout<<"Added " << filename << " to staging area.\n";
        return 0;
    }
    else if (command == "commit") {

        if(argc < 4 || string(argv[2]) != "-m") {
            cerr << "Usage: mygit commit -m \"message\"\n";
            return 1;
        }

        string message = argv[3]; //commit message

        string commitHash = createCommit(message);

        if(commitHash.empty()) {
            return 1;
        }

        cout << "Commit hash: " << commitHash << '\n';

        return 0;
    }
    else if(command == "log") {
        fs::path headPath = fs::path(".mygit") / "HEAD";

        ifstream headFile(headPath);

        if(!headFile) {
            cerr << "Failed to read HEAD\n";
            return 1;
        }

        string headContent;
        getline(headFile, headContent);
        headFile.close();

        string branchRef = headContent.substr(5);

        fs::path branchPath = fs::path(".mygit") / branchRef;

        if(!fs::exists(branchPath)) {
            cout << "No commits yet.\n";
            return 0;
        }

        ifstream branchFile(branchPath);

        string currCommit;
        getline(branchFile, currCommit);
        branchFile.close();

        while(!currCommit.empty()) {
            fs::path commitPath = fs::path(".mygit") / "objects" / currCommit;

            ifstream commitFile(commitPath);

            if(!commitFile) {
                cerr << "Failed to read commit object\n";
                return 1;
            }

            string line;
            string parentHash;
            bool foundParent = false;

            while(getline(commitFile, line)) {
                cout << line << '\n';

                if(line.rfind("Parent ", 0)==0) {
                    parentHash = line.substr(7);
                    foundParent = true;
                }
            }
            commitFile.close();
            cout<< '\n';

            if(!foundParent) {
                break;
            }

            currCommit = parentHash;
        }
        return 0;
    }
    else if(command == "status") {
        map<string,string> index = readIndex();

        for(const auto& entry : fs::directory_iterator(".")) {

            if(entry.is_directory()) {
                continue; //if it is directory skip it. for this prototype version i am only scanning files directly in root directory level.
            }

            string filename = entry.path().filename().string();

            string extension = entry.path().extension().string();

            if(filename == "mygit.exe" || extension == ".o" || extension == ".a" || extension == ".dll" || extension == ".lib" || extension == ".s") {
                continue;
            }

            if(index.find(filename) == index.end()) {
                cout << "untracked: " << filename <<'\n';
                continue;  //case1: untracked
            }

            ifstream file(entry.path(), ios::binary);

            string content((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());

            string currentHash = sha256(content);

            if(currentHash != index[filename]) {
                cout<< "modified: " << filename << '\n';
            }

        }

        for(const auto& entry : index) {
            string filename = entry.first;

            if(!fs::exists(filename)) {
                cout << "deleted: " <<filename << '\n';
            }
        }
        return 0;
    }
    else if(command == "branch") {

        if(argc < 3) {
            cerr << "Usage: mygit branch <branch-name>\n";
            return 1;
        }

        createBranch(argv[2]);
        return 0;
    }
    else if(command == "checkout") {
        if(argc < 3) {
            cerr << "Usage: mygit checkout <branch-name>\n";
            return 1;
        }

        checkoutBranch(argv[2]);

        return 0;
    }
    

    cout<<"Unknown command: "<< command << '\n';
        return 1;

}