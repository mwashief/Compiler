#include<iostream>
#include<cmath>
#include<fstream>
#include<vector>
using namespace std;


class SymbolInfo
{
public:
    string name = "";
    string type = "";
    string tag = "";

    vector<SymbolInfo*> alist;
    int size = 0;
    int start =0;
    int finish = 0;
    string description= "";


    SymbolInfo* next;

    SymbolInfo()
    {
        name = type = "";
        next = 0;
    }

    SymbolInfo(string f, string s)
    {
        name = f;
        type = s;
        next = 0;
    }

    bool operator==(SymbolInfo& rhs)
    {
        return name == rhs.name;
    }

    SymbolInfo operator=(SymbolInfo& rhs)
    {
        name = rhs.name;
        type = rhs.type;
        return *this;
    }
    string getName() {return name;}
    string getType() {return type;}
    friend ostream& operator<<(ostream& os, SymbolInfo rhs)
    {
    os << "<" << rhs.name << ", " << rhs.tag << ">";
    return os;
    }
};


class ScopeTable
{
public:
    SymbolInfo **scope;
    ScopeTable* parentScope;
    int id;
    int buckets;

    ScopeTable(int id, int n, ScopeTable* parent)
    {
        this->id = id;
        buckets = n;
        scope = new SymbolInfo*[n];
        for(int i=0; i<n; i++) scope[i] = 0;
        parentScope = parent;
    }

    int hashFunc(string a)
    {
        unsigned int h = 0;
        for(int i=0; i<a.length(); i++)
        {
            h += ((int)(a[i]-'a'))*pow(26,i);
        }
        return h%buckets;
    }

    SymbolInfo* lookUp(string a)
    {
        int bucket = hashFunc(a), i = 0;
        SymbolInfo* temp = scope[bucket];
        while(temp!=0)
        {
            if(temp->getName() == a)
            {
                //cout << "Found in ScopeTable# " << id << " at position " << bucket << ", " << i << endl;
                return temp;
            }
            temp = temp->next;
            i++;
        }
        //cout << "Not found" << endl;
        return 0;
    }

    bool insert(string a, string b)
    {
        if(!lookUp(a))
        {
            SymbolInfo* temp = new SymbolInfo(a,b);
            int bucket = hashFunc(a), i = 0;
            SymbolInfo* now = scope[bucket];
            if(scope[bucket] == 0)
            {
                scope[bucket] = temp;
            }
            else
            {
                i++;
                while(now->next != 0)
                {
                    now = now->next;
                    i++;
                }
                now->next = temp;
            }
            //cout << "Inserted in Scopetable# " << id << " at position " << hashFunc(a) << ", " << i << endl;
            return true;
        }
        //cout << "<" << a << "," << b << ">" << " already exists in current scopeTable" << endl;
        return false;
    }

    bool insert(SymbolInfo* a)
    {
        if(!lookUp(a->name))
        {
            SymbolInfo* temp = a;
            int bucket = hashFunc(a->name), i = 0;
            SymbolInfo* now = scope[bucket];
            if(scope[bucket] == 0)
            {
                scope[bucket] = temp;
            }
            else
            {
                i++;
                while(now->next != 0)
                {
                    now = now->next;
                    i++;
                }
                now->next = temp;
            }
            //cout << "Inserted in Scopetable# " << id << " " << *a << endl;
            return true;
        }
        //cout << "<" << a << "," << b << ">" << " already exists in current scopeTable" << endl;
        return false;
    }

    bool deleteSymbol(string a)
    {
        int bucket = hashFunc(a), i = 0;
        SymbolInfo* temp = scope[bucket];
        if(temp->getName() == a)
        {
            scope[bucket] = temp->next;
            delete temp;
            //cout << "Deleted entry at "<< bucket << ", " << i << " from current ScopeTable" << endl;
            return true;
        }
        else
        {
            while(temp->next!=0)
            {
                i++;
                if(temp->next->getName() == a)
                {
                    SymbolInfo* store = temp->next->next;
                    delete temp->next;
                    temp->next = store;
                    //cout << "Deleted entry at "<< bucket << ", " << i << " from current ScopeTable" << endl;
                    return true;
                }
                temp = temp->next;
            }
        }
        //cout << "Not found" << endl;
        return false;
    }

    void print()
    {
        cout<<  " ScopeTable # " << id << endl;
        for(int i=0; i<buckets; i++)
        {
            SymbolInfo *temp;
            temp = scope[i];
            if(temp == 0) continue;
            cout << " " << i << " -->  ";
            while(temp!=0)
            {
                cout << *temp << " ";
                temp = temp->next;
            }
            cout << endl;
        }
        cout << endl;
    }

    ~ScopeTable()
    {
        for(int i=0; i<buckets; i++)
        {
            SymbolInfo *temp;
            temp = scope[i];
            while(temp!=0)
            {
                SymbolInfo *now = temp;
                temp = temp->next;
                delete now;
            }
        }
        delete scope;
    }
};

class SymbolTable
{
public:
    ScopeTable* current;
    int bks;
    SymbolTable(int n)
    {
        bks = n;
        current = new ScopeTable(1, n, 0);
    }

    void enterScope()
    {
        ScopeTable* a = new ScopeTable(current->id+1, bks, current);
        current = a;
    }

    void exitScope()
    {
        ScopeTable* temp = current;
        current = current->parentScope;
        delete temp;
    }

    bool insert(string a, string b)
    {
        return current->insert(a,b);
    }

    bool insert(SymbolInfo* a)
    {
        return current->insert(a);
    }

    bool remove(string a)
    {
        return current->deleteSymbol(a);
    }

    SymbolInfo* lookUp(string a)
    {
        SymbolInfo* result = 0;
        ScopeTable *now = current;
        while(now!=0)
        {
            result = now->lookUp(a);
            if(result) return result;
            now = now->parentScope;
        }
        return result;
    }

    void PCST()
    {
        current->print();
    }

    void PAST()
    {
        ScopeTable *now = current;
        while(now!=0)
        {
            now->print();
            now = now->parentScope;
        }
    }
};
