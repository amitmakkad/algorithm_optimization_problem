#include <bits/stdc++.h>
using namespace std;
#include "minknap.c"


//DATASET GENERATION AS GIVEN IN RESEARCH PAPER
void dataset(double &r, int &n,int &p,int &s,vector<double>&x_location_supplier,vector<double>&y_location_supplier,vector<double>&x_location_plant,
vector<double>&y_location_plant,vector<vector<int>>&c,vector<int>&sv,int &cr,vector<int>&fp){


    // assigning random locations in unit square
    for(int i=0;i<s;i++){
        x_location_supplier[i]=((double) rand()/(RAND_MAX));
        y_location_supplier[i]=((double) rand()/(RAND_MAX));
    }
    for(int i=0;i<n;i++){
        x_location_plant[i]=((double) rand()/(RAND_MAX));
        y_location_plant[i]=((double) rand()/(RAND_MAX));
    }


    // The cost of assigning each supplier to each plant is then computed by multiplying their Euclidean distance by 10.
    for(int i=0;i<s;i++){
        for(int j=0;j<n;j++){
            c[i][j]=10*(sqrt((x_location_supplier[i]-x_location_plant[j])*(x_location_supplier[i]-x_location_plant[j])+(y_location_supplier[i]-y_location_plant[j])*(y_location_supplier[i]-y_location_plant[j])));
        }
    }


    // supply volume randomly and uniformly generated from the interval
    int total_supply_vol=0;
    for(int i=0;i<s;i++){
        sv[i]=rand()%100+1;
        total_supply_vol+=sv[i];
    }


    // CR of each plant is generated by the formula CR=(r/p)*sum(sv)
    cr=(r*total_supply_vol)/p;


    // fp cost of opening plant= [0 90]+[100 110]sqrt(cr)
    for(int i=0;i<n;i++){
        fp[i]=rand()%90+(100+rand()%10)*sqrt(cr);
    }

}


// we are using pisinger's MINKNAP for solving knapsack problem
int knapsack(vector<vector<int>>&c,vector<vector<int>>&x,vector<int>&fp,vector<int>&lamda,int j,vector<int>&sv,int cr){
    
    int s=c.size();
    
    // pre routine
    int mk_c=0, ans=0;
    int mk_p[s], mk_w[s], mk_x[s];   // profit, weight, x(take or not_take)
    for(int i=0;i<s;i++){
        mk_p[i]=c[i][j]+lamda[i];
        mk_w[i]=sv[i];
        mk_c+=sv[i];
        ans+=mk_p[i];
    }
    ans+=fp[j];
    mk_c-=cr;

    int val = minknap(s, mk_p, mk_w, mk_x, mk_c);

    // post routine
    ans-=val;
    for(int i=0;i<s;i++){           //since our problem is to minimize knapsack, so xi=1-yi substitution
        x[i][j]=1-mk_x[i];
    }
    return ans;
}


void standard_subgradient_method(int &z_ub, int &n,int &p,int &s,vector<vector<int>>&c,vector<vector<int>>&x,vector<int>&y,vector<int>&fp,vector<int>&lamda,vector<int>&sv,int &cr){

    int K=30;           // max_iterations=30 (just taken small number, but can take large also)
    
    double g[K][s];     // deflection, g[k][i] denotes in iteration k, i th component of subgradient of z_lb
    
    double theta=1;     // learning rate
    
    double t[K]={0};    // t[k] step size at iteration k
    

    for(int k=0;k<K;k++){   
        
        
        // solving n independent knacksack solutions
        int z_kp[n];
        int temp_array[n];
        
        
        for(int j=0;j<n;j++){
            z_kp[j]=knapsack(c,x,fp,lamda,j,sv,cr);
            temp_array[j]=z_kp[j];
        }
        
        // choosing smallest p values in feasible solution
        sort(temp_array,temp_array+n);
        int pth_smallest_value_of_knacpsack=temp_array[p-1];
        
        
        int z_lb=0;  // value of relaxed solution
        
        
        for(int j=0;j<n;j++){
            if(z_kp[j]<=pth_smallest_value_of_knacpsack){
                
                // counts in feasible solution
                y[j]=1;
                
                // x[i][j] remains same
                z_lb=z_lb+z_kp[j];
            }
            else{
                // not in feasible solution
                y[j]=0;
                
                // all x[i][j] should be 0
                for(int i=0;i<s;i++){
                    x[i][j]=0;
                }

            }
            
        }
        
        
        // as given in function of relaxed solution
        for(int i=0;i<s;i++){
            z_lb=z_lb-lamda[i];
        }
        
        
        // calculating subgradient 
        for(int i=0;i<s;i++){
            for(int j=0;j<n;j++){
                g[k][i]=g[k][i]+x[i][j];
            }
            g[k][i]=g[k][i]-1;
        }
        
        
        // calculating subgradient norm
        double g_norm2=0;
        for(int i=0;i<s;i++){
            g_norm2=g_norm2+g[k][i]*g[k][i];
        }
        
        
        // calculating step size
        if(g_norm2==0)
            g_norm2=1;
        t[k]=(double)(z_ub-z_lb)/g_norm2;
        t[k]=t[k]*theta;
        
        
        // calculating new lamda
        for(int i=0;i<s;i++){
            lamda[i]=max(0.0,lamda[i]+t[k]*g[k][i]);
        }
        
        
        if(t[k]<0.000001){   // lamda value not improving
            break;
        }
        else{               //continue iterations
            z_ub=z_lb;
        }
        
        
    }
}

void subgradient_deflection_method(int &z_ub, int &n,int &p,int &s,vector<vector<int>>&c,vector<vector<int>>&x,vector<int>&y,vector<int>&fp,vector<int>&lamda,vector<int>&sv,int &cr){

    int K=30;          // max iterations
    
    double h[K+2][s];   // h[k+2] is of current iteration
    
    for(int i=0;i<s;i++){   // h[0] means h[-2] and h[1] means h[-1]
        h[0][i]=0;
        h[1][i]=0;
    }

    double g[K][s];     // deflection, g[k][i] denotes in iteration k, i th component of subgradient of z_lb
    
    double theta=1;    //learning rate

    double t[K]={0};    // t[k] step size at iteration k
    
    
    for(int k=0;k<K;k++){   
        
        // solving n independent knacksack solutions
        int z_kp[n];
        int temp_array[n];
        
        for(int j=0;j<n;j++){
            z_kp[j]=knapsack(c,x,fp,lamda,j,sv,cr);
            temp_array[j]=z_kp[j];
        }
        
        
        // choosing smallest p values in feasible solution
        sort(temp_array,temp_array+n);
        int pth_smallest_value_of_knacpsack=temp_array[p-1];
        
        
        int z_lb=0;  // value of relaxed solution
        
        
        for(int j=0;j<n;j++){
            if(z_kp[j]<=pth_smallest_value_of_knacpsack){
                // counts in feasible solution
                y[j]=1;
                
                // x[i][j] remains same  
                z_lb=z_lb+z_kp[j];
            }
            else{
                // not in feasible solution
                y[j]=0;
                
                // all x[i][j] should be 0
                for(int i=0;i<s;i++){
                    x[i][j]=0;
                }
            }
            
        }
        

        // as given in function of relaxed solution
        for(int i=0;i<s;i++){
            z_lb=z_lb-lamda[i];
        }
        
        // calculating subgradient 
        for(int i=0;i<s;i++){
            for(int j=0;j<n;j++){
                g[k][i]=g[k][i]+x[i][j];
            }
            g[k][i]=g[k][i]-1;
        }
        
        
        // new deflection = combination of previous direction and current sub gradient
        // h[k+2][i] is of current iterataion
        for(int i=0;i<s;i++){
            h[k+2][i]=(g[k][i]+0.3*h[k+1][i]+0.1*h[k][i])/1.4;
        }
            
        
        // calculating  norm
        double h_norm2=0;
        for(int i=0;i<s;i++){
            h_norm2=h_norm2+h[k+2][i]*h[k+2][i];
        }
        
        
        // calculating step size
        if(h_norm2==0)
            h_norm2=1;
        t[k]=(double)(z_ub-z_lb)/h_norm2;
        t[k]=t[k]*theta;
        
        
        // calculating new lamda
        for(int i=0;i<s;i++){
            lamda[i]=max(0.0,lamda[i]+t[k]*h[k+2][i]);
        }
        

        if(t[k]<0.000001){   // lamda value not improving
            break;
        }
        else{               //continue iterations
            z_ub=z_lb;
        }
        
    }
}

void greedy_method(int &n,int &p,int &s,vector<vector<int>>&c,vector<vector<int>>&x,vector<int>&y,vector<int>&fp,vector<int>&lamda,vector<int>&sv,int &cr){

    // we have to select enough suppliers from unassigned ones to satisfy residual capacity of all open plants

    for(int i=0;i<s;i++){
        int temp=0;
        int index_of_plant=-1;
        
        for(int j=0;j<n;j++){
            if(x[i][j]==1){
                temp+=x[i][j];
                index_of_plant=j;
            }
            
        }
        
        for(int j=0;j<n;j++){
            x[i][j]=0;
        }
        
        if(temp==1&&index_of_plant>=0){
            x[i][index_of_plant]=1;
        }
    }

    
    int weight_function[s][n];  // weight function of supplier i supplies to plant j
    
    for(int i=0;i<s;i++){
        for(int j=0;j<n;j++){
            weight_function[i][j]=c[i][j];
        }
    }
    
    
    
    //desireability of plant(j)=
    //difference of second smallest and smallest value of f(i,j) for all i
    vector<int>desireability(n,0);
    
    for(int j=0;j<n;j++){
        
        int first_mini=INT_MAX;
        int second_mini=INT_MAX;
        for(int i=0;i<s;i++){
            first_mini=min(first_mini,weight_function[i][j]);
        }
        for(int i=0;i<s;i++){
            if(weight_function[i][j]!=first_mini){
                second_mini=min(second_mini,weight_function[i][j]);
            }
        }
        
        desireability[j]=second_mini-first_mini;

    }
    
    // we have serve suppliers by decreasing order of desireability
    
    int cnt=0;
    while(cnt<p){  // toatl p plants
    
        //finding plant index with max desireability 
        cnt++;
        int max_desireability=0;
        int max_index=0;
        
        for(int j=0;j<n;j++){
            if(y[j]==1){
                max_desireability=max(max_desireability,desireability[j]);
            }
            
        }
        
        for(int j=0;j<n;j++){
            if(y[j]==1&&max_desireability==desireability[j]){
                max_index=j;
                desireability[j]=-1;    // not interfare in next iteration
                break;
            }
        }

        // we have to fullfill capacity reuirement cr
        // first subtract supply volume of intially assigned suppliers
        int temp_cr=cr;
        for(int i=0;i<s;i++){
            if(x[i][max_index]==1){
                temp_cr=temp_cr-sv[i];
            }
        }
        
        while(temp_cr>0){
            // we have select supplier with min weight function
            
            int mini_weight=INT_MAX;
            int mini_index=0;
            
            for(int i=0;i<s;i++){
                mini_weight=min(mini_weight,weight_function[i][max_index]);
            }
            
            for(int i=0;i<s;i++){
                if(mini_weight==weight_function[i][max_index]){
                    mini_index=i;
                    mini_weight=INT_MAX;
                }
            }
            
            temp_cr=temp_cr-sv[mini_index];
            
            x[mini_index][max_index]=1;  // supplier assigned
            
            // max_index contain open plant number
            // mini_index contain supplier index
            
        }
        
    }
}

int main()
{
    // 1. DATA

    //r = total min capacity requirement of all plants / total supply volume of all suppliers
    //n = no of potential sites
    //p = no of plants to be located
    //s = number of suppliers
    double r=0.4;
    int n=200;
    int p=100;
    int s=4000;

    
    // location of supplier and potential plants
    vector<double>x_location_supplier(s,0);
    vector<double>y_location_supplier(s,0);
    vector<double>x_location_plant(n,0);
    vector<double>y_location_plant(n,0);
    
    
    //c[i][j] = geographical positions of the potential sites of plants and the customers are randomly and uniformly generatedin a unit square
    vector<vector<int>>c(s,vector<int>(n,0));
    
    
    //sv[j] = supply volume of each supplier 
    vector<int>sv(s,0);
    
    
    //cr = capacity requirement CR of each plant
    int cr=0;


    //fp[j] = cost of opening plant j
    vector<int>fp(n,0);
    

    // x[i][j] denotes whether supplier i supplies to plant j
    vector<vector<int>>x(s,vector<int>(n,0));
    

    vector<int>y(n,0);  // y[j] denotes whether there is a open plant or not at j
    

    // langrange multipliers
    vector<int>lamda(s,0);


    // upper_bound for z
    int z_ub=INT_MAX;
    
    


    time_t start, end;
    time(&start);

    // 2. DATASET GENERATION
    dataset(r,n,p,s,x_location_supplier,y_location_supplier,x_location_plant,y_location_plant,c,sv,cr,fp); 

    
    time(&end);
    double time_taken = double(end - start);
    cout << "Time taken by program is : " << fixed << time_taken << setprecision(5);
    cout << " sec " << endl;


    time(&start);

    // 3. STANDARD SUBGRADIENT METHOD
    standard_subgradient_method(z_ub,n,p,s,c,x,y,fp,lamda,sv,cr);
    
    
    // 4. SUBGRADIENT DEFLECTION METHOD
    subgradient_deflection_method(z_ub,n,p,s,c,x,y,fp,lamda,sv,cr);
    
    
    // data we got till now:-
    // if supplier i is assigned to exactly one open plant j then x[i][j]=1;
    // we got position of p open plants that is y[j]
    // but we have to select enough suppliers from unassigned ones to satisfy residual capacity of all open plants
    
    // 5. GREEDY ALGORITHM for construction of feasible solution
    greedy_method(n,p,s,c,x,y,fp,lamda,sv,cr);

    time(&end);
    time_taken = double(end - start);
    cout << "Time taken by program is : " << fixed << time_taken << setprecision(5);
    cout << " sec " << endl;
    

    // 6. FINAL SOLUTION
    // cout<<"Plant opening index from Potential Sites"<<endl;
    // for(int i=0;i<n;i++){
    //     cout<<y[i]<<" ";
    // }
    // cout<<endl;


    // cout<<"Supplier Locations"<<endl;
    // for(int i=0;i<s;i++){
    //     for(int j=0;j<n;j++){
    //         cout<<x[i][j]<<" ";
    //     }
    //     cout<<endl;
    // }
    
    // finally feasible solution obtained, however it can be improved also by using tabu search
    // but we are not implementing it here


    return 0;
} 
