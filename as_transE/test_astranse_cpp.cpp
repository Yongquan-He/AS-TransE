#include<iostream>
#include<cstring>
#include<cstdio>
#include<map>
#include<vector>
#include<string>
#include<ctime>
#include<algorithm>
#include<cmath>
#include<cstdlib>
using namespace std;

bool debug=false;
bool L1_flag=1;
int i1,i2;

//char dataset[20]="WN18";
//string data="WN18";

char dataset[20]="FB15k";
string data="FB15k";
string conf;
int sample;


string version;
int flag;
string trainortest = "test";

map<string,int> relation2id,entity2id;
map<int,string> id2entity,id2relation;
map<string,string> mid2name,mid2type;
map<int,map<int,int> > entity2num;
map<int,int> e2num;
map<pair<string,string>,map<string,double> > rel_left,rel_right;

map<int,int> t_relation_num;

int relation_num,entity_num;
int n= 50;

double sigmod(double x)
{
    return 1.0/(1+exp(-x));
}

double vec_len(vector<double> a)
{
    double res=0;
    for (int i=0; i<a.size(); i++)
        res+=a[i]*a[i];
    return sqrt(res);
}

void vec_output(vector<double> a)
{
    for (int i=0; i<a.size(); i++)
    {
        cout<<a[i]<<"\t";
        if (i%10==9)
            cout<<endl;
    }
    cout<<"-------------------------"<<endl;
}

double sqr(double x)
{
    return x*x;
}

char buf[100000],buf1[100000];

int my_cmp(pair<double,int> a,pair<double,int> b)
{
    return a.first>b.first;
}

double cmp(pair<int,double> a, pair<int,double> b)
{
    return a.second<b.second;
}

double cmp_chain(pair <pair<int,int>, double> a, pair <pair<int,int>, double> b)
{
    return a.second<b.second;
}


class Test{
    vector<vector<double> > relation_vec,entity_vec;


    vector<int> h,l,r;
    vector<int> fb_h,fb_l,fb_r;
    map<pair<int,int>, map<int,int> > ok;
    double res ;

    map<pair<int,int>,int> ok_syn, ok_inv;
    map<int,int> ok_sym;
    map<pair<int,int>, map<int,int> > ok_chain;

    vector<int> syn_r1,syn_r2;
    vector<int> inv_r1,inv_r2;
    vector<int> sym_r1,chain_r1,chain_r2,chain_r3;




public:
    void add(int x,int y,int z, bool flag)
    {
        if (flag)
        {
            fb_h.push_back(x);
            fb_r.push_back(z);
            fb_l.push_back(y);
        }
        ok[make_pair(x,z)][y]=1;
    }

    void add_syn(int x, int y){
        
        syn_r1.push_back(x);
        syn_r2.push_back(y);
        ok_syn[make_pair(x,y)]=1;
    }

    void add_inv(int x, int y){
        
        inv_r1.push_back(x);
        inv_r2.push_back(y);
        ok_inv[make_pair(x,y)]=1;
    }


    void add_sym(int x){
        sym_r1.push_back(x);
        ok_sym[x]=1;
        //cout<<"hello"<<endl;
    }

    void add_chain(int x,int y,int z)
    {

        chain_r1.push_back(x);
        chain_r2.push_back(y);
        chain_r3.push_back(z);

        ok_chain[make_pair(x,y)][z]=1;
    }






    int rand_max(int x)
    {
        int res = (rand()*rand())%x;
        if (res<0)
            res+=x;
        return res;
    }
    double len;
    double calc_sum(int e1,int e2,int rel)
    {
        double sum=0;
        if (L1_flag)
            for (int ii=0; ii<n; ii++)
            sum+=-fabs(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);
        else
        for (int ii=0; ii<n; ii++)
            sum+=-sqr(entity_vec[e2][ii]-entity_vec[e1][ii]-relation_vec[rel][ii]);
        return sum;
    }


    double calc_sum_syn(int r1,int r2)
    {
        double sum=0;
        if (L1_flag)
            for (int ii=0; ii<n; ii++)
            sum+=-fabs(relation_vec[r1][ii]-relation_vec[r2][ii]);
        else
        for (int ii=0; ii<n; ii++)
            sum+=-sqr(relation_vec[r1][ii]-relation_vec[r2][ii]);
        return sum;
    }


    double calc_sum_inv(int r1,int r2)
    {
        double sum=0;
        if (L1_flag)
            for (int ii=0; ii<n; ii++)
            sum+=-fabs(relation_vec[r1][ii]+relation_vec[r2][ii]);
        else
        for (int ii=0; ii<n; ii++)
            sum+=-sqr(relation_vec[r1][ii]+relation_vec[r2][ii]);
        return sum;
    }


    double calc_sum_sym(int r1)
    {
        double sum=0;
        if (L1_flag)
            for (int ii=0; ii<n; ii++)
            sum+=-fabs(relation_vec[r1][ii]);
        else
        for (int ii=0; ii<n; ii++)
            sum+=-sqr(relation_vec[r1][ii]);
        return sum;
    }



    double calc_sum_chain(int r1,int r2,int r3)
    {
        double sum=0;
        if (L1_flag)
            for (int ii=0; ii<n; ii++)
            sum+=-fabs(relation_vec[r3][ii]-relation_vec[r2][ii]-relation_vec[r1][ii]);
        else
        for (int ii=0; ii<n; ii++)
            sum+=-sqr(relation_vec[r3][ii]-relation_vec[r2][ii]-relation_vec[r1][ii]);
        return sum;
    }





    void run()
    {

      string path1,path3;

        if(flag==1)
            {
            path1="./"+data+"_1/"+"relation2vec"+conf+"."+version;
            path3="./"+data+"_1/"+"entity2vec"+conf+"."+version;
            }

        else if(flag==2)
            {
            path1="./"+data+"_2/"+"relation2vec"+conf+"."+version;
            path3="./"+data+"_2/"+"entity2vec"+conf+"."+version;
            }
       else if(flag==5)
            {
            path1="./"+data+"_5/"+"relation2vec"+conf+"."+version;
            path3="./"+data+"_5/"+"entity2vec"+conf+"."+version;
            }
        else if(flag==8)
            {
            path1="./"+data+"_8/"+"relation2vec"+conf+"."+version;
            path3="./"+data+"_8/"+"entity2vec"+conf+"."+version;
            }
        else
            cout<<"flag error!!!"<<endl;    

      cout<<"read data from "<<path1<<endl;

      // path1="./"+data+"/"+"relation2vec."+version;
      // path3="./"+data+"/"+"entity2vec."+version;

                // cout<<path2;

      FILE* f1 = fopen(path1.c_str(),"r");
      FILE* f3 = fopen(path3.c_str(),"r");


        // FILE* f1 = fopen(("relation2vec."+version).c_str(),"r");
        // FILE* f3 = fopen(("entity2vec."+version).c_str(),"r");



        cout<<"number of relation is "<<relation_num<<endl;
        cout<<"number of entities is "<<entity_num<<endl;
        cout<<"number of fb_l is "<<fb_l.size()<<endl;


        int relation_num_fb=relation_num;
        relation_vec.resize(relation_num_fb);

        for (int i=0; i<relation_num_fb;i++)
        {
            relation_vec[i].resize(n);
            for (int ii=0; ii<n; ii++)
                fscanf(f1,"%lf",&relation_vec[i][ii]);
        }
        entity_vec.resize(entity_num);
        for (int i=0; i<entity_num;i++)
        {
            entity_vec[i].resize(n);
            for (int ii=0; ii<n; ii++)
                fscanf(f3,"%lf",&entity_vec[i][ii]);
            if (vec_len(entity_vec[i])-1>1e-3)
                cout<<"wrong_entity"<<i<<' '<<vec_len(entity_vec[i])<<endl;
        }
        fclose(f1);
        fclose(f3);
        double lsum=0 ,lsum_filter= 0;
        double rsum = 0,rsum_filter=0;
        double lp_n=0,lp_n_filter;
        double rp_n=0,rp_n_filter;
        double mid_p_n=0,mid_p_n_filter = 0;
        
        
        
        map<int,double> lsum_r,lsum_filter_r;
        map<int,double> rsum_r,rsum_filter_r;
        

        map<int,double> mid_sum_r,mid_sum_filter_r;
        map<int,double> lp_n_r,lp_n_filter_r;


        double syn_p_n=0,syn_p_n_filter=0;
        double syn_sum = 0,syn_sum_filter=0;
        vector<pair<int,double> >  syn_n_r,syn_n_filter_r;
        vector<pair<int,double> >  syn_r,syn_filter_r;


        double inv_p_n=0,inv_p_n_filter=0;
        double inv_sum = 0,inv_sum_filter=0;
        vector<pair<int,double> >  inv_n_r,inv_n_filter_r;
        vector<pair<int,double> >  inv_r,inv_filter_r;


        double sym_p_n=0,sym_p_n_filter=0;
        double sym_sum = 0,sym_sum_filter=0;
        vector<pair<int,double> >  sym_n_r,sym_n_filter_r;
        vector<pair<int,double> >  sym_r,sym_filter_r;


        double chain_p_n=0,chain_p_n_filter=0;
        double chain_sum = 0,chain_sum_filter=0;
        vector<pair<int,double> >  chain_n_r,chain_n_filter_r;
        
        vector<pair <pair<int,int>, double> >  chain_r,chain_filter_r;



        map<int,double> rp_n_r,rp_n_filter_r;
        map<int,double> mid_p_n_r,mid_p_n_filter_r;
        map<int,int> rel_num;
        int hit_n = 1;
        cout<<"read embeddings done!"<<endl;

        cout<<"length of embeddings is "<<n<<endl;

        vector<pair<int,double> > a;

        vector<pair<int,double> > chain_tmp;


        int syn[syn_r1.size()];

        int inv[inv_r1.size()];


        int r1,r2,r3;
        double tmp;
        int filter;

        cout<<"关系个数为"<<relation_num<<endl;

//---------------------------------synonym ranking--------------------------------------////

        // for (int testid = 0; testid<syn_r1.size(); testid+=1)
        // {


        //     r1=syn_r1[testid];
        //     r2=syn_r2[testid];

        //     tmp = calc_sum_syn(r1,r2);
        //     //用知识库里面的每个关系替换r2，计算这些关系的距离，以便后面进行比较，a（关系号，距离值）
        //     a.clear();
        //     for (int i=0; i<relation_num; i++)
        //     {
        //         double sum = 0;
        //         if(i==r1)
        //             continue;
        //         sum+=calc_sum_syn(r1,i);
        //         a.push_back(make_pair(i,sum));
        //     }
        //     //cout<<a[0].second<<endl;
        //     sort(a.begin(),a.end(),cmp);
        //     //cout<<a[0].second<<endl;
        //     filter=0;

        //     cout<<testid<<":"<<endl;


        //     for (int i=a.size()-1; i>=0; i--)
        //     {

        //         if (ok_syn[make_pair(r1,a[i].first)]!=1)
        //             filter+=1;
        //         // for example
        //         if(filter<10)
        //             cout<<id2relation[a[i].first]<<endl;
                
        //         if (a[i].first==r2)
        //         {
        //             syn_sum+=a.size()-i;
        //             syn_sum_filter+=filter+1;

        //             syn_r.push_back(make_pair(r1,a.size()-i));

        //             syn_filter_r.push_back(make_pair(r1,filter+1));

        //             //syn_tmp.push_back(make_pair(a.size()-i,filter+1));
        //             syn[testid]=filter+1;

        //             if (a.size()-i<=hit_n)
        //             {
        //                 syn_p_n+=1;
        //             }
        //             if (filter<hit_n)
        //             {
        //                 syn_p_n_filter+=1;
        //             }
        //             break;
        //         }
        //     }

        // }

        // //f2.close();

        // cout<<"synonym test samples are:\t"<<syn_r1.size()<<endl;
        // cout<<"synonym:"<<syn_sum/syn_r1.size()<<'\t'<<syn_p_n/syn_r1.size()
        // <<"\t"<<syn_sum_filter/syn_r1.size()<<'\t'<<syn_p_n_filter/syn_r1.size()<<endl;

            // for (int i=0;i<syn_r1.size(); i++)
            // {
            //     cout<<"relation \t"<<id2relation[syn_r[i].first]<<"\trank: "<<syn_r[i].second<<endl;
            // }





        // string path2="./syn_"+to_string(flag)+".txt";
        // FILE* f2 = fopen(path2.c_str(),"w");

        //     for (int i=0;i<syn_r1.size(); i++)
        //     {
        //         //fprintf(f2,"%d\t%d\n",i+1,syn[i]);
                
        //         if(flag==1&&syn[i]!=1)
        //             fprintf(f2,"%d\t%d\n",i+1,syn[i]);
        //         //cout<<"relation \t"<<id2relation[syn_r[i].first]<<"\trank: "<<syn_r[i].second<<endl;
        //         if(flag!=1&&syn[i]==1) 
        //             fprintf(f2,"%d\t%d\n",i+1,syn[i]); 
                
        //     }




////---------------------------------inverse ranking--------------------------------------////


        // for (int testid = 0; testid<inv_r1.size(); testid+=1)
        // {


        //     r1=inv_r1[testid];
        //     r2=inv_r2[testid];

        //     tmp = calc_sum_inv(r1,r2);
        //     //用知识库里面的每个关系替换r2，计算这些关系的距离，以便后面进行比较，a（关系号，距离值）
        //     a.clear();

        //     for (int i=0; i<relation_num; i++)
        //     {
        //         double sum = 0;
        //         if(i==r1)
        //             continue;
        //         sum+=calc_sum_inv(r1,i);
        //         a.push_back(make_pair(i,sum));
        //     }
        //     sort(a.begin(),a.end(),cmp);
        //     filter=0;
        //     for (int i=a.size()-1; i>=0; i--)
        //     {

        //         if (ok_inv[make_pair(r1,a[i].first)]!=1)
        //             filter+=1;
        //         //cout<<id2relation[a[i].first]<<endl;
        //         if (a[i].first==r2)
        //         {
        //             inv_sum+=a.size()-i;
        //             inv_sum_filter+=filter+1;

        //             inv_r.push_back(make_pair(r1,a.size()-i));
        //             inv_filter_r.push_back(make_pair(r1,filter+1));

        //             inv[testid]=filter+1;

        //             if (a.size()-i<=hit_n)
        //             {
        //                 inv_p_n+=1;
        //             }
        //             if (filter<hit_n)
        //             {
        //                 inv_p_n_filter+=1;
        //             }
        //             break;
        //         }
        //     }
        // }


        // cout<<"inverse test samples are:\t"<<inv_r1.size()<<endl;
        // cout<<"inverse:\t"<<inv_sum/inv_r1.size()<<'\t'<<inv_p_n/inv_r1.size()
        // <<"\t"<<inv_sum_filter/inv_r1.size()<<'\t'<<inv_p_n_filter/inv_r1.size()<<endl;


        // string path2="./inv_"+to_string(flag)+".txt";
        // cout<<path2<<endl;
        // FILE* f2 = fopen(path2.c_str(),"w");

        //     for (int i=0;i<inv_r1.size(); i++)
        //     {
        //         //fprintf(f2,"%d\t%d\n",i+1,syn[i]);
                
        //         if(flag==1&&inv[i]==1)
        //             fprintf(f2,"%d\t%d\n",i+1,inv[i]);
        //         //cout<<"relation \t"<<id2relation[syn_r[i].first]<<"\trank: "<<syn_r[i].second<<endl;
        //         if(flag!=1&&inv[i]!=1) 
        //             fprintf(f2,"%d\t%d\n",i+1,inv[i]); 
                
        //     }




          // for (int i=0;i<=inv_r.size()-1; i++)
          //   {
          //       cout<<"relation \t"<<id2relation[inv_r[i].first]<<"\trank: "<<inv_r[i].second<<"\tfilter rank: "<<inv_filter_r[i].second<<endl;  
          //   }      



////---------------------------------symmetric ranking--------------------------------------////


        // for (int testid = 0; testid<sym_r1.size(); testid+=1)
        // {


        //     r1=sym_r1[testid];

        //     tmp = calc_sum_sym(r1);
        //     //用知识库里面的每个关系替换r2，计算这些关系的距离，以便后面进行比较，a（关系号，距离值）
        //     a.clear();

        //     for (int i=0; i<relation_num; i++)
        //     {
        //         double sum = 0;
        //         sum+=calc_sum_sym(i);
        //         a.push_back(make_pair(i,sum));
        //     }
        //     sort(a.begin(),a.end(),cmp);
        //     filter=0;
        //     for (int i=a.size()-1; i>=0; i--)
        //     {

        //         if (ok_sym[a[i].first]!=1)
        //             filter+=1;

        //         //cout<<id2relation[a[i].first]<<endl;
                
        //         if (a[i].first==r1)
        //         {
        //             sym_sum+=a.size()-i;
        //             sym_sum_filter+=filter+1;

        //             sym_r.push_back(make_pair(r1,a.size()-i));
        //             sym_filter_r.push_back(make_pair(r1,filter+1));

        //             if (a.size()-i<=hit_n)
        //             {
        //                 sym_p_n+=1;
        //             }
        //             if (filter<hit_n)
        //             {
        //                 sym_p_n_filter+=1;
        //             }
        //             break;
        //         }
        //     }
        // }


        // cout<<"symmetric test samples are:\t"<<sym_r1.size()<<endl;
        // cout<<"symmetric:\t"<<sym_sum/sym_r1.size()<<'\t'<<sym_p_n/sym_r1.size()
        // <<"\t"<<sym_sum_filter/sym_r1.size()<<'\t'<<sym_p_n_filter/sym_r1.size()<<endl;
            //     for (int i=0;i<=sym_r.size()-1; i++)
            // {
            //     cout<<"relation \t"<<id2relation[sym_r[i].first]<<"\trank: "<<sym_r[i].second<<"\tfilter rank: "<<sym_filter_r[i].second<<endl;  
            // }






//---------------------------------chain ranking--------------------------------------////
        // string path2;
        // path2="../data/FB15k/chain_filter.txt";
        // FILE* f2 = fopen(path2.c_str(),"a");
        // int count=0;

        // for (int testid = 0; testid<chain_r1.size(); testid+=1)
        // {

        //     r1=chain_r1[testid];
        //     r2=chain_r2[testid];
        //     r3=chain_r3[testid]; 

        //     //cout<<id2relation[r1]<<id2relation[r2]<<id2relation[r3]<<endl;

        //     //用知识库里面的每个关系替换r2，计算这些关系的距离，以便后面进行比较，a（关系号，距离值）
        //     a.clear();

        //     for (int i=0; i<relation_num; i++)
        //     {
        //         double sum = 0;
        //         sum+=calc_sum_chain(r1,r2,i);
        //         a.push_back(make_pair(i,sum));
        //     }

        //     sort(a.begin(),a.end(),cmp);
        //     //cout<<id2relation[a[0].first]<<"\t"<<a[0].second<<endl;
        //     //cout<<id2relation[a[relation_num-1].first]<<"\t"<<a[relation_num-1].second<<endl;
        //     filter=0;
        //     //return;

        //     int debug=0;

            
        //     for (int i=a.size()-1; i>=0; i--)
        //     {   
        //         debug++;

        //         if (ok_chain[make_pair(r1,r2)].count(a[i].first)!=1)
        //             filter+=1;

        //         //cout<<id2relation[a[i].first]<<endl;

        //         // if(debug==2)
        //         //     return;

        //         if (a[i].first==r3)
        //         {
        //             chain_sum+=a.size()-i;
        //             chain_sum_filter+=filter+1;

        //             chain_tmp.push_back(make_pair(testid+1,a.size()-i));

        //             chain_r.push_back(make_pair(make_pair(r1,r2),a.size()-i));
        //             chain_filter_r.push_back(make_pair(make_pair(r1,r2),filter+1));

        //             if (a.size()-i<=hit_n)
        //             {
        //                 chain_p_n+=1;
        //             }
        //             if (filter<hit_n)
        //             {
        //                 chain_p_n_filter+=1;
        //             }



        //     if(flag==1&&((a.size()-i)<10)&&count<=100)
        //         {
        //         count++;
        //         fprintf(f2,"%s\t%s\t%s\n",id2relation[r1].c_str(),id2relation[r2].c_str(),id2relation[r3].c_str());
        //         }
       

        //     if(flag!=1&&(a.size()-i)>=100)
        //         {

        //         fprintf(f2,"%s\t%s\t%s\n",id2relation[r1].c_str(),id2relation[r2].c_str(),id2relation[r3].c_str());
         
        //         }

        //             //cout<<"testid: "<<testid<<"\trank: "<<a.size()-i<<"\tfilter: "<<filter<<endl;
                    
        //             break;


        //         }
        //     }
        
        // }
        
        // cout<<"chain test samples are:\t"<<chain_r1.size()<<endl;
        // cout<<"chain:\t"<<chain_sum/chain_r1.size()<<'\t'<<chain_p_n/chain_r1.size()
        // <<"\t"<<chain_sum_filter/chain_r1.size()<<'\t'<<chain_p_n_filter/chain_r1.size()<<endl;






        //sort(chain_r.begin(),chain_r.end(),cmp_chain);

            //     for (int i=0;i<=20; i++)
            // {
            //     cout<<"relation path \t"<<id2relation[chain_r[i].first.first]<<"\t"<<id2relation[chain_r[i].first.second]<<"\trank: "
            //     <<chain_r[i].second<<"\tfilter rank: "<<chain_filter_r[i].second<<endl;  
            // }

            //      for (int i=chain_tmp.size()-1;i>=chain_tmp.size()-21; i++)
            // {
            //     cout<<"relation path \t"<<id2relation[chain_r[i].first.first]<<"\t"<<id2relation[chain_r[i].first.second]<<"\trank: "
            //     <<chain_r[i].second<<"\tfilter rank: "<<chain_filter_r[i].second<<endl;  
            // }

        // sort(chain_tmp.begin(),chain_tmp.end(),cmp);

        // if(flag!=1)      
        //     {
        //     for (int i=0;i<20; i++)
        //         cout<<"testid: "<<chain_tmp[i].first<<"\t rank: "<<chain_tmp[i].second<<endl; 
        //     }
        // else
        //     {
        //     for (int i=chain_tmp.size()-1;i>chain_tmp.size()-20; i--)
        //         cout<<"testid: "<<chain_tmp[i].first<<"\t rank: "<<chain_tmp[i].second<<endl; 
        //     }


            //     cout<<"relation path \t"<<id2relation[chain_r[i].first.first]<<"\t"<<id2relation[chain_r[i].first.second]<<"\trank: "
            //     <<chain_r[i].second<<"\tfilter rank: "<<chain_filter_r[i].second<<endl; 
            // }

 
        // sort(chain_tmp.begin(),chain_tmp.end(),cmp);

        // if(flag==1){
        //     for (int i=chain_tmp.size()-1;i>=chain_tmp.size()-20; i--)
        //     {
        //         printf("%d\n",chain_tmp[i].first);

        //         cout<<"relation path \t"<<id2relation[chain_r[i].first.first]<<"\t"<<id2relation[chain_r[i].first.second]<<"\trank: "
        //         <<chain_r[i].second<<"\tfilter rank: "<<chain_filter_r[i].second<<endl; 
        //     }
        //             }   
        // else
        // {
        //     for (int i=0;i<20;i++)
        //     {
        //         printf("%d\t",chain_tmp[i].first);
        //         cout<<"relation path \t"<<id2relation[chain_r[i].first.first]<<"\t"<<id2relation[chain_r[i].first.second]<<"\trank: "
        //         <<chain_r[i].second<<"\tfilter rank: "<<chain_filter_r[i].second<<endl; 
        //     }

        // }             

//chain_tmp.size()-1

    //     if(sample==1)
    //     {
    //     string path2;
    //     path2="../data/FB15k/chain_test_new.txt";

    //     FILE* f2 = fopen(path2.c_str(),"a");

    //     if(flag==1){
    //         for (int i=chain_tmp.size()-1;i>=chain_tmp.size()-i1; i--)
    //         {
    //             fprintf(f2,"%s\t%s\t%s\n",id2relation[chain_r1[chain_tmp[i].first]].c_str(),
    //                 id2relation[chain_r2[chain_tmp[i].first]].c_str(),
    //                 id2relation[chain_r3[chain_tmp[i].first]].c_str());
    //         }
    //                 }   
    //     else
    //     {
    //         for (int i=0;i<i2;i++)
    //         {
    //             fprintf(f2,"%s\t%s\t%s\n",id2relation[chain_r1[chain_tmp[i].first]].c_str(),
    //             id2relation[chain_r2[chain_tmp[i].first]].c_str(),
    //             id2relation[chain_r3[chain_tmp[i].first]].c_str());
    //         }

    //     }             
    //     fclose(f2);
    // }



        // if(flag!=1)      
        //     {
        //     for (int i=0;i<=100; i++)
        //         cout<<"testid: "<<chain_tmp[i].first<<"\t rank: "<<chain_tmp[i].second<<endl; 
        //     }
        // else
        //     {
        //     for (int i=chain_tmp.size()-1;i>=chain_tmp.size()-101; i--)
        //         cout<<"testid: "<<chain_tmp[i].first<<"\t rank: "<<chain_tmp[i].second<<endl; 
        //     }




            // a.clear();
            // for (int i=0; i<entity_num; i++)
            // {
            //     double sum = calc_sum(i,l,rel);
            //     a.push_back(make_pair(i,sum));
            // }
            // //按照距离值由小到大排序
            // sort(a.begin(),a.end(),cmp);


            // ttt=0;
            // filter = 0;
            // //遍历整个实体集，从后往前,也就是从距离大的元组向距离近的元组方向
            // for (int i=a.size()-1; i>=0; i--)
            // {

            //     //如果被替换的三元组存在于知识库中，那么用ttt记录一下个数
            //     // if (ok[make_pair(a[i].first,rel)].count(l)>0)
            //     //  ttt++;

            //     //如果被替换的三元组不存在于知识库中，那么用filter记录一下个数
            //     if (ok[make_pair(a[i].first,rel)].count(l)==0)
            //         filter+=1;

            //     //如果a[i].first刚好为头h
            //     if (a[i].first ==h)
            //     {
            //         //正确三元组(h,r,l)的排名rank
            //         lsum+=a.size()-i;

            //         lsum_filter+=filter+1;

            //         //对应这个关系rel的排名rank    
            //         lsum_r[rel]+=a.size()-i;
            //         lsum_filter_r[rel]+=filter+1;

            //         //如果排名在前十位
            //         if (a.size()-i<=10)
            //         {
            //             lp_n+=1;
            //             lp_n_r[rel]+=1;
            //         }

            //         if (filter<10)
            //         {
            //             lp_n_filter+=1;
            //             lp_n_filter_r[rel]+=1;
            //         }
            //         break;
            //     }
            // }


            // a.clear();
            // for (int i=0; i<entity_num; i++)
            // {
            //     double sum = calc_sum(h,i,rel);
            //     a.push_back(make_pair(i,sum));
            // }
            // sort(a.begin(),a.end(),cmp);
            // ttt=0;
            // filter=0;
            // for (int i=a.size()-1; i>=0; i--)
            // {
            //     if (ok[make_pair(h,rel)].count(a[i].first)>0)
            //         ttt++;
            //     if (ok[make_pair(h,rel)].count(a[i].first)==0)
            //         filter+=1;
            //     if (a[i].first==l)
            //     {
            //         rsum+=a.size()-i;
            //         rsum_filter+=filter+1;
            //         rsum_r[rel]+=a.size()-i;
            //         rsum_filter_r[rel]+=filter+1;
            //         if (a.size()-i<=10)
            //         {
            //             rp_n+=1;
            //             rp_n_r[rel]+=1;
            //         }
            //         if (filter<10)
            //         {
            //             rp_n_filter+=1;
            //             rp_n_filter_r[rel]+=1;
            //         }
            //         break;
            //     }
            // }
       


        // cout<<fb_l.size()<<endl;
        // cout<<"left:"<<lsum/fb_l.size()<<'\t'<<lp_n/fb_l.size()<<"\t"<<lsum_filter/fb_l.size()<<'\t'<<lp_n_filter/fb_l.size()<<endl;
        // cout<<"right:"<<rsum/fb_r.size()<<'\t'<<rp_n/fb_r.size()<<'\t'<<rsum_filter/fb_r.size()<<'\t'<<rp_n_filter/fb_r.size()<<endl;

        // //cout<<testid<<":"<<"\t"<<lsum/(testid+1)<<' '<<lp_n/(testid+1)<<' '<<rsum/(testid+1)<<' '<<rp_n/(testid+1)<<"\t"<<lsum_filter/(testid+1)<<' '<<lp_n_filter/(testid+1)<<' '<<rsum_filter/(testid+1)<<' '<<rp_n_filter/(testid+1)<<endl;
        // cout<<"mid:"<<mid_sum/fb_l.size()<<'\t'<<mid_p_n/fb_l.size()
        // <<"\t"<<mid_sum_filter/fb_l.size()<<'\t'<<mid_p_n_filter/fb_l.size()<<endl;



}
};
Test test;

        
void prepare()
{

    char path[20],p1[30],p2[30],p3[30],p4[30],p_valid[50],p_syn[50],p_inv[50],p_sym[100],p_chain[100];

    strcpy(path,"../data/");
    strcat(path,data.data());

    strcpy(p1,path);
    strcat(p1,"/entity2id.txt");

    strcpy(p2,path);
    strcat(p2,"/relation2id.txt");


    strcpy(p3,path);
    if(flag==1)
        strcat(p3,"/train.txt");
    else if(flag==2)
        strcat(p3,"/train_new_2.txt");
    else if(flag==5)
        strcat(p3,"/train_new_5.txt");
    else if(flag==8)
        strcat(p3,"/train_new_8.txt");
    else
        cout<<"input data error!!!"<<endl;


    strcpy(p_valid,path);
    cout<<p_valid<<endl;
    strcat(p_valid,"/valid.txt");
    cout<<p_valid<<endl;


    strcpy(p4,path);
    strcat(p4,"/test.txt");

    // strcpy(p_syn,path);
    // strcat(p_syn,"/synonym.txt");

    // 测试example所用的
    strcpy(p_syn,path);
    strcat(p_syn,"/synonym_example.txt");


    strcpy(p_inv,path);
    strcat(p_inv,"/inverse.txt");


    strcpy(p_sym,path);
    strcat(p_sym,"/symmetric.txt");


    strcpy(p_chain,path);
   // if(sample==1)   
    strcat(p_chain,"/chain_test_new.txt");
    // else
    //     strcat(p_chain,"/chain_test_final.txt");

    //cout<<p_valid<<endl;
    //cout<<p1<<endl<<p2<<endl<<p3<<endl<<p4<<endl<<p_valid<<endl;
    //cout<<p_syn<<endl<<p_inv<<endl<<p_sym<<endl<<p_chain<<endl;



    FILE* f1 = fopen(p1,"r");
    FILE* f2 = fopen(p2,"r");
    int x;
    while (fscanf(f1,"%s%d",buf,&x)==2)
    {
        string st=buf;
        entity2id[st]=x;
        id2entity[x]=st;
        mid2type[st]="None";
        entity_num++;
    }
    while (fscanf(f2,"%s%d",buf,&x)==2)
    {
        string st=buf;
        relation2id[st]=x;
        id2relation[x]=st;
        relation_num++;
    }


    FILE* f_syn = fopen(p_syn,"r");
    while (fscanf(f_syn,"%s",buf)==1)
    {
        string s1=buf;
        fscanf(f_syn,"%s",buf);
        string s2=buf;
        if (relation2id.count(s1)==0)
        {
            cout<<"miss relation:"<<s1<<endl;
            relation2id[s1] = relation_num;
            relation_num++;
        }

        if (relation2id.count(s2)==0)
        {
            cout<<"miss relation:"<<s2<<endl;
            relation2id[s2] = relation_num;
            relation_num++;
        }

        //cout<<s1<<"\t"<<s2<<endl;
        //cout<<relation2id[s1]<<"\t"<<relation2id[s2]<<endl;

        test.add_syn(relation2id[s1],relation2id[s2]);

    }




    FILE* f_inv = fopen(p_inv,"r");
    while (fscanf(f_inv,"%s",buf)==1)
    {
        string s1=buf;
        fscanf(f_inv,"%s",buf);
        string s2=buf;
        if (relation2id.count(s1)==0)
        {
            cout<<"miss relation:"<<s1<<endl;
            relation2id[s1] = relation_num;
            relation_num++;
        }

        if (relation2id.count(s2)==0)
        {
            cout<<"miss relation:"<<s2<<endl;
            relation2id[s2] = relation_num;
            relation_num++;
        }

        //cout<<s1<<"\t"<<s2<<endl;
        //cout<<relation2id[s1]<<"\t"<<relation2id[s2]<<endl;

        test.add_inv(relation2id[s1],relation2id[s2]);

    }




    FILE* f_sym = fopen(p_sym,"r");
    while (fscanf(f_sym,"%s",buf)==1)
    {
        string s1=buf;
        if (relation2id.count(s1)==0)
        {
            cout<<"miss relation:"<<s1<<endl;
            relation2id[s1] = relation_num;
            relation_num++;
        }

        //cout<<s1<<endl;
        //cout<<relation2id[s1]<<"\t"<<relation2id[s2]<<endl;

        test.add_sym(relation2id[s1]);

    }



    FILE* f_chain = fopen(p_chain,"r");
    while (fscanf(f_chain,"%s",buf)==1)
    {
        string s1=buf;
        fscanf(f_chain,"%s",buf);
        string s2=buf;
        fscanf(f_chain,"%s",buf);
        string s3=buf;

        if (relation2id.count(s1)==0)
        {
            cout<<"miss relation:"<<s1<<endl;
            relation2id[s1] = relation_num;
            relation_num++;
        }

        if (relation2id.count(s2)==0)
        {
            cout<<"miss relation:"<<s2<<endl;
            relation2id[s2] = relation_num;
            relation_num++;
        }

        if (relation2id.count(s3)==0)
        {
            cout<<"miss relation:"<<s3<<endl;
            relation2id[s3] = relation_num;
            relation_num++;
        }

        //cout<<s1<<"\t"<<s2<<"\t"<<s3<<endl;
        //cout<<relation2id[s1]<<"\t"<<relation2id[s2]<<endl;

        test.add_chain(relation2id[s1],relation2id[s2],relation2id[s3]);

    }





    FILE* f_kb = fopen(p4,"r");
    while (fscanf(f_kb,"%s",buf)==1)
    {
        string s1=buf;
        fscanf(f_kb,"%s",buf);
        string s2=buf;
        fscanf(f_kb,"%s",buf);
        string s3=buf;
        if (entity2id.count(s1)==0)
        {
            cout<<"miss entity:"<<s1<<endl;
        }
        if (entity2id.count(s2)==0)
        {
            cout<<"miss entity:"<<s2<<endl;
        }
        if (relation2id.count(s3)==0)
        {
            cout<<"miss relation:"<<s3<<endl;
            relation2id[s3] = relation_num;
            relation_num++;
        }
        test.add(entity2id[s1],entity2id[s2],relation2id[s3],true);

        t_relation_num[relation2id[s3]]++;

    }

    fclose(f_kb);
    FILE* f_kb1 = fopen(p3,"r");
    while (fscanf(f_kb1,"%s",buf)==1)
    {
        string s1=buf;
        fscanf(f_kb1,"%s",buf);
        string s2=buf;
        fscanf(f_kb1,"%s",buf);
        string s3=buf;
        if (entity2id.count(s1)==0)
        {
            cout<<"miss entity:"<<s1<<endl;
        }
        if (entity2id.count(s2)==0)
        {
            cout<<"miss entity:"<<s2<<endl;
        }
        if (relation2id.count(s3)==0)
        {
            relation2id[s3] = relation_num;
            relation_num++;
        }

        entity2num[relation2id[s3]][entity2id[s1]]+=1;
        entity2num[relation2id[s3]][entity2id[s2]]+=1;
        e2num[entity2id[s1]]+=1;
        e2num[entity2id[s2]]+=1;
        test.add(entity2id[s1],entity2id[s2],relation2id[s3],false);
    }
    fclose(f_kb1);


    FILE* f_kb2 = fopen(p_valid,"r");
    while (fscanf(f_kb2,"%s",buf)==1)
    {
        string s1=buf;
        fscanf(f_kb2,"%s",buf);
        string s2=buf;
        fscanf(f_kb2,"%s",buf);
        string s3=buf;
        if (entity2id.count(s1)==0)
        {
            cout<<"miss entity:"<<s1<<endl;
        }
        if (entity2id.count(s2)==0)
        {
            cout<<"miss entity:"<<s2<<endl;
        }
        if (relation2id.count(s3)==0)
        {
            relation2id[s3] = relation_num;
            relation_num++;
        }
        test.add(entity2id[s1],entity2id[s2],relation2id[s3],false);
    }
    fclose(f_kb2);
}


int main(int argc,char**argv)
{
    //char data[20]="FB15k";
    //cout<<argc<<endl;
    if (argc<6)
        return 0;
    else
    {
        version = argv[1];
        flag = atoi(argv[2]);
        n = atoi(argv[3]);
        data = argv[4];
        conf = argv[5];
        //sample = atoi(argv[6]);
       // i1 = atoi(argv[7]);
        //i2 = atoi(argv[8]);
        //cout<<i1<<"\t"<<i2<<endl;
        //cout<<"sample"<<sample<<endl;
        cout<<conf<<endl;
        prepare();
        test.run();
    }
}

