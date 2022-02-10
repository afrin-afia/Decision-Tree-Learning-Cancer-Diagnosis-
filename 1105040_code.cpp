#include <iostream>
#include <algorithm>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<fstream>

using namespace std;

int rows= 535;      //80% of dataset
int cols = 9;
int totalRows=669;
int d[535][9];
int dtotal[669][9];
int dtest[134][9];
float e;


class node{

 public:
    int attr,val,label;       //attr= kon attr er value eita.
    node* parent;
    node* child[10];

    node(int a, int v, int l, node* p){
            attr=a;
            val=v;
            parent= p;
            label=l;
            for(int i=-0; i<10; i++){
                    child[i]= NULL;
            }
    }

};

//node* dummyroot;
void readFromFile(){
        char str1[20], str2[20], str3[20], str4[20], str5[20], str6[20], str7[20], str8[20], str9[20];
        FILE *fp;
        fp = fopen("assignment1_data_set.csv", "r");
        if(NULL == fp)
        {
            printf("\nCan't open file.");
            return ;
        }

        fscanf(fp, "%[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %s, %s, %s, %s, %s, %s, %s, %s, %s ", str1, str2, str3, str4, str5, str6, str7, str8, str9);

        for(int i=0; i<totalRows ;i++){
                if( EOF != fscanf(fp, "%[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %[^,], %s, %s, %s, %s, %s, %s, %s, %s, %s ", str1, str2, str3, str4, str5, str6, str7, str8, str9) ) {
                        dtotal[i][0]= atoi(str1);
                        dtotal[i][1]= atoi(str2);
                        dtotal[i][2]= atoi(str3);
                        dtotal[i][3]= atoi(str4);
                        dtotal[i][4]= atoi(str5);
                        dtotal[i][5]= atoi(str6);
                        dtotal[i][6]= atoi(str7);
                        dtotal[i][7]= atoi(str8);
                        dtotal[i][8]= atoi(str9);
                }
        }

        fclose(fp);

        int randomArray[669];
        for(int x=0; x<totalRows; x++){
                randomArray[x]= x;
        }
        srand (time(NULL));
        int v1 = rand() % 10;//cout<<"v1= "<<v1<<endl;
        for(int t=0; t<v1;t++){
            random_shuffle(&randomArray[0],&randomArray[669]);
        }
        //cout<<"71 aray--- "<<randomArray[0]<<"   "<< randomArray[668]<<endl;
        for(int i=0; i<rows ;i++){
                for(int j=0; j<cols; j++){
                        d[i][j]= dtotal[randomArray[i]][j];
                }
        }
        for(int i=0; i< totalRows-rows ;i++){
                for(int j=0; j<cols; j++){
                        dtest[i][j]= dtotal[randomArray[i+rows]][j];
                }
        }

}


float findEntropy(){

        float countP=0.0 ,countN=0.0;
        for(int i=0; i<rows; i++){
                if(d[i][cols-1]==1) countP++;
                else countN++;
        }

        float e= (-countP/rows)* (log2(countP/rows))-(countN/rows)* (log2(countN/rows));
        //cout<<e;
        //cout<<countN;
        return e;

}


int findMostCommonValue(int colNo, int attVal){

        int count0=0, count1=0;
        if(colNo== -1 && attVal==-1 ){      //test all examples
                for(int i=0;i<rows; i++){
                            if(d[i][cols-1]==0) count0++;
                            else count1++;
                }
                if(count0> count1) return 0;
                else return 1;          //*****************else if, else cant -- eirokom?
        }

        else{           //only selected values test

                for(int i=0;i<rows; i++){
                            if(d[i][colNo]== attVal) {

                                    if(d[i][cols-1]==0) count0++;
                                    else count1++;
                            }
                }
                if(count0> count1) return 0;
                else return 1;

        }

}

float findEntropyAttr(float p, float n){

        float pn= p+n;
        float e;
          //cout<<"p= "<<p<<"  n="<<n<<endl;
        if(n==0)    e= (-p/pn)* (log2(p/pn));
        else if(p==0)    e= (-n/pn)* (log2(n/pn));

        else e= (-p/pn)* (log2(p/pn))-(n/pn)* (log2(n/pn));
        //cout<<e;
        //cout<<countN;
        return e;

}


float calculateGain(int c){         //'c' namok att er jonno gain dibe. c=col dhore.

        float countPA[11], countNA[11];           //eder 0 index baad
        for(int i=0;i<11; i++){
                countNA[i]=0.0;  countPA[i]=0.0;
        }

        for(int i=0; i<rows; i++){
                    if(d[i][cols-1]==1)    countPA[d[i][c]]++;
                    else countNA[d[i][c]]++;
        }

        float sum=0.0;
        for(int i=1; i<11; i++){

                float x= (countNA[i]+countPA[i])/rows;//cout<<"x="<<x<<endl;
                float y= findEntropyAttr(countPA[i],countNA[i]);//cout<<"y="<<y<<endl;
                sum+= (x*y);
        }//cout<<"sum="<<sum<<"  e="<<e<<endl;

        return e-sum;
}


int selectBestAtt(int* remainAtt){

        float maxgain=-1; int selectedAtt=-1;


        for(int i=1;i<9; i++){

                if(remainAtt[i]==1){        //this att named 'i' is remaining

                        float gain= calculateGain(i-1);     //'i' namok att er gain, se ache i=1 col e

                        if(gain > maxgain){
                                maxgain=gain;
                                selectedAtt=i;
                        }
                }
        }
        return selectedAtt;
}

int checkExampleSet(int colNo,int val){

        int f=0;
        for(int i=0; i<rows;i++){

                if(d[i][colNo]== val) f=1;
        }
        return f;
}

node* makeTree(int colNo, int attVal, int* remainAtt){

        int count1=0, count0=0;
        node* root= new node (-1,-1,-1,NULL);  //dummy root
        //cout<<"colNo, attval= "<<colNo<<"  "<<attVal<<endl;
        for(int i=0;i<rows; i++){

                if( (colNo==-1 && d[i][cols-1]==1) || (colNo !=-1 && d[i][colNo]==attVal && d[i][cols-1]==1) ) count1++;
                else if((colNo==-1 && d[i][cols-1]==0) || (colNo !=-1 && d[i][colNo]==attVal && d[i][cols-1]==0) ) count0++;
        }//cout<<count0<<"   ..   "<<count1<<endl;
        if(count0==0){      //all are 1
                root->label=1;//cout<<"changing label-211"<<endl;
                root->attr= colNo+1;
                root->val=attVal;
                return root;
        }
        else if (count1==0) {
                root->label=0;
                root->attr= colNo+1;
                root->val=attVal;
                //cout<<"changing label-216...0"<<endl;
                return root;
        }
        int flag=0;
        for(int i=0; i<9; i++){
                if(remainAtt[i]== 1){
                    flag=1;break;
                }
        }
        if(flag==0){

                root->label= findMostCommonValue(colNo, attVal);
                root->attr= colNo+1;
                root->val=attVal;
                //cout<<"changing label-227....."<< root->label<<endl;
                return root;
        }


        //otherwise begin:
        int selectedAtt= selectBestAtt(remainAtt); //cout<<selectedAtt;
        //cout << selectedAtt<<endl;

        root->attr= selectedAtt;
        for(int i=1; i<=10;i++){        //for each value of selectedAttr

                node* child = new node (selectedAtt,i,-1,root);         //att, val, label
                root->child[i-1] = child;
                //child->parent=root;

                if(checkExampleSet(selectedAtt-1,i)==0){    //empty example set
                       int mCV= findMostCommonValue(selectedAtt-1, i) ;
                       //child->label = mCV;      //label= answer. child ke leaf banay dilam
                       //cout<<"changing label-245"<<endl;
                       node* leaf=  new node (selectedAtt,child->val,mCV,child);
                       child->child[i-1]= leaf;
                       //leaf->parent= child;
                }
                else{
                        remainAtt[selectedAtt]= 0;      //mark as taken
                        node* c= makeTree(selectedAtt-1, i, remainAtt);
                        child->child[i-1]= c;

                }
        }
        return root;

}

void printArray(){

        for (int i=0; i<rows;i++){

            for(int j=0;j<cols;j++){
                     cout<< d[i][j];
            }
            cout<<endl;
        }
}


int test(int i, node* root){

        int ans=5;//[134];

            //cout<<"testing--- ";
            //for(int j=0;j<9;j++) cout<< dtest[i][j];
            //cout<<endl;

            node* cur= root;
            //cout<<"ok1"<<endl;
            for(int j=0; j<cols; j++){
                for(int k=0; k<10; k++){//cout<<k<<endl;

                    //if(cur->child[k]!=NULL) cout<<cur->child[k]->val <<"  ?? "<<dtest[i][cur->attr-1]<<endl;
                    if(cur->child[k]!=NULL && cur->child[k]->val == dtest[i][cur->attr-1]){
                            //cout<<"going to path= "<< cur->child[k]->attr<<"  "<<cur->child[k]->val<<endl;
                            //cout<<"label= "<<cur->child[k]->label<<endl;
                            if(cur->child[k]->label != -1){     //means, this is leaf
                                    ans= cur->child[k]->label;
                                  //cout<<"changing ans  .."<<ans<<endl;
                                    return ans;
                            }
                            else{
                                    cur= cur->child[k];
                                    k=-1;
                            }
                    }
                }
            }
        //}
}

int main(){


    int remainAttr[9];         //remaining attr. 1 to 8 numbering
    float accu=0.0, prec=0.0, recall=0.0;
    for(int run=0; run<100;run++){
        remainAttr[0]=-1;
        for(int i=1; i<9;i++){
                remainAttr[i]=1;            //set all as remaining
        }
        readFromFile();

        e= findEntropy();     //entropy of the system
        node* dummyroot= makeTree(-1,-1,remainAttr);     //-1,-1 means all examples dekhte hbe
        //TESTING
        //cout<<"TESTING..............."<<endl<<endl;

        //int resultArray[134];
        float tp=0.0, tn=0.0, fp=0.0, fn=0.0;
        int result;
        for(int i=0;i<totalRows-rows;i++){
            //cout<< test(i,dummyroot)<< endl;
            result = test(i,dummyroot);
            if(result== dtest[i][cols-1]){
                    if(result==1)  tp++;
                    else tn++;
            }
            else{
                    if(result==1) fp++;     //my result=1, actual=0
                    else fn++;              //res=0, act=1
            }
        }

        //calculating accuracy, precision, recall:
        accu += (tp+tn)/(tp+tn+fp+fn);
        prec += tp/(tp+fp);
        recall += tp/(tp+fn);
    }
        cout<< "accuracy= "<<accu/100 <<endl<<"precision= "<< prec/100 <<endl<<"recall= "<< recall/100 <<endl;

        return 0;
}
