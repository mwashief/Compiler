#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>

using namespace std;
vector<string> split(string s)
{
    vector<string> result;
    istringstream iss(s);
    for(string t; iss >> t; )result.push_back(t);
    return result;
}

int main()
{
    ifstream ifs("code.asm");
    vector<string> preLines;
    string curLine = "";
    //int i = 0;
    while(getline(ifs, curLine))
    {
        //i++;
        if(curLine.size() <= 1) continue;
        if(curLine[0] == ';') continue;
        if(preLines.size()<1 )
        {
            preLines.push_back(curLine);
            continue;
        }
        vector<string> preTokens = split(preLines[preLines.size()-1]);
        vector<string> curTokens = split(curLine);
        if((preTokens[0] == "push" && curTokens[0] == "pop") || (preTokens[0] == "pop" && curTokens[0] == "push"))
        {
            if(preTokens[1] == curTokens[1])
            {
                //cout << "Stack: " << i << endl;
                preLines.pop_back();
                continue;
            }
        }
        else if(preTokens[0] == "mov" && curTokens[0] == "mov")
        {
            //cout << "found mov pair" << i << endl;
            curTokens[1].erase(curTokens[1].end()-1, curTokens[1].end());
            preTokens[1].erase(preTokens[1].end()-1, preTokens[1].end());
            //cout << ": " << preTokens[1] << " " << preTokens[2] << " | " << curTokens[1] << " " << curTokens[2] <<endl;
            if((curTokens[1] == preTokens[2]) && (curTokens[2] == preTokens[1]))
            {
                //cout << "MOV: " << i << endl;
                continue;
            }
        }
        preLines.push_back(curLine);
    }
    ifs.close();
    ofstream ofs("code.asm");
    for(int i=0; i<preLines.size(); i++)
    {
        //cout << preLines[i] << endl;
        ofs << preLines[i] << endl;
    }
}
