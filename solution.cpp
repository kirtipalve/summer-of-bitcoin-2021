
/* The way I understand the problem is via 3 points of condition.
 * The order of transactions should be maintained i.e. a parent transaction should always appear before a child transaction.
 * The net weight of all transactions must not exceed 4000000.
 * Transactions should be selected such that the fees are maximized.
 * 
 * A naive solution would be to iterate through the transaction list and include it in the block if it is a valid transaction.
 * A transaction would be considered valid if its parent transactions are already included in the block.
 * These steps should continue as long as the total weight is less than max given weight OR till the list is empty.
 * 
 * An efficient solution will also take fees and weight into consideration. 
 * A valuable transation will be a transaction with fees as high as possible and weight as low as possible.
 */

#include<bits/stdc++.h>
using namespace std;
typedef long long ll;

// Global Variables
string input = "mempool.csv"; 
set<string> visited_trx_ids; // to store txids that are already included in the block
int maxBlockWeight = 4000000; //max block weight as given in hint


class TransactionDetails {
    public:
        string tx_id;
        int fee;
        int weight;
        vector<string> parents;
};

// function to initialize transaction information and return as pair
pair<string, TransactionDetails*> addTransactionInfo(vector<string>& trxInfo){
    
    auto trx = new TransactionDetails(); // new objext trx created
    trx->tx_id = trxInfo[0]; // 1st element is the id
    trx->fee = stoi(trxInfo[1]); // 2nd element is the fee which is converted to integer from string
    trx->weight = stoi(trxInfo[2]); //3rd element is weight
    vector<string> pid; //vector to store parent ids
    if(trxInfo.size()==4){
    stringstream ss(trxInfo[3]);
     while (ss.good()) {
        string substr;
        getline(ss, substr, ';');
        pid.push_back(substr);
    }
    }
  
    /*for (int i = 3; i < trxInfo.size(); i++){
        pid.push_back(trxInfo[i]);
    }
      if(pid.size()>1){
    cout<<"pid size is"<<pid.size()<<endl;
}
else{
    cout<<"nope"<<endl;
}*/
    trx->parents = pid;
    return {trxInfo[0], trx}; //returning in the form of id, transaction info

}

// function to read the csv file and using a map to store transaction details
void readCSV(string filename, unordered_map<string, TransactionDetails*>& trx){
    
    ifstream fin(input);
    vector<string> trxDetails;
    string temp,line,word;
    getline(fin,line);
    while(getline(fin,line)){
        trxDetails.clear();
        stringstream s(line);
        while(getline(s,word,',')){ //for comma separated values
            trxDetails.push_back(word);
        }
        pair<string,TransactionDetails*> p = addTransactionInfo(trxDetails);
        trx[p.first] = p.second; // hash map with thecout<<"Transaction Count :" << ump.size() << "\n"; txid as index and transaction info as value stored at the specific index
    }
    fin.close();
    cout<<"Transaction Count :"<< trx.size() << "\n";
    
}

//function for output file
void getOutput(vector<string>& output, string filename){
    ofstream myfile(filename);
    for(auto s:output){
        myfile << s << endl;
    }
    myfile.close();
}


//function to check if a transaction is valid. A transaction is considered valid if its parents transactions appear before it
bool isValidTransaction(TransactionDetails* trx,set<string>& included_trx_ids){
    //If all the parents of trx are present in included_trx_ids then it is valid
    
    for(auto parent : trx->parents){
        
        if(included_trx_ids.find(parent) == included_trx_ids.end())
            return false;
    }
    ;
    return true;
}

// Main Function
int main(){

    unordered_map<string, TransactionDetails *> trx; //map to store transaction details
    readCSV(input, trx);
    set<pair<float, TransactionDetails*>, greater<pair<float, TransactionDetails*>>> transactionSet; //  to ensure the first parameter is sorted in descending
    set<string> transaction_set_included; // this will include all trx which are included in block
    vector<string> output; // maintains order of output
    //we will prioritize transaction with high fee/weight values
    for(auto i:trx){
        transactionSet.insert({(float)i.second->fee / (float)i.second->weight, i.second});
    }
    int currentBlockWeight = 0;
    int totalFee = 0;
    while(!transactionSet.empty() && (currentBlockWeight < maxBlockWeight)){
        //cout<<"First while loop entered"<<endl;
        bool found = false;
        for(auto itr = transactionSet.begin(); itr != transactionSet.end(); itr++){
            //cout<<"for loop entered"<<endl;
            TransactionDetails* currentTransaction = (*itr).second;
            int currFee = currentTransaction->fee;
            //cout<<"current fee is"<<currFee<<endl;
            int currWeight = currentTransaction->weight;
            //cout<<visited_trx_ids.size()<<endl;
            if(isValidTransaction(currentTransaction, transaction_set_included) && currentBlockWeight + currWeight <= maxBlockWeight){
                //cout<<"check for valid"<<endl;
                currentBlockWeight += currWeight;
                transaction_set_included.insert(currentTransaction->tx_id);
                output.push_back(currentTransaction->tx_id);
                totalFee += currFee;
                transactionSet.erase(itr); //delete after including in block
                found = true;
                break;
            }
        }
        if(!found){
            break;
        }
    }
    cout << "Total fee in current block: " << totalFee << endl;
    cout << "Total weight in current block: " << currentBlockWeight << endl;
    cout<<"Total transactions ids "<<output.size()<<endl;
    getOutput(output, "block.txt");

    return 0;
}
