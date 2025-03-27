#include <iostream>
using namespace std;

//De bai
//x0 = 1, x1 = 1
//xn = nx0+nx1+ ... + nxn-1, voi moi n > 1
//bai toan con nho nhat cua day Xn tren la n* (tong cua x chay tu 0 toi n-1)


//ham su dung de quy
int dequy(int n){
	if(n==1 ||n==0) {return 1;} //bai toan con nho nhat, khi n =1 hoac n=0 thi tra ve ket qua =1 va dung ham lai
    else{
    	int sum=0; //goi bien tong la sum, cho bang 0
    	for(int i=0;i<n;i++){ //dung vong lap de duyet qua tat ca ca chi so tu 0 den n-1 nhu de bai
    		sum+= (n* dequy(i)); //nhan voi n nhu cong thuc
		}
		return sum; //tra ve tong sau khi vong lap hoan thanh
	}
}


//ham khong su dung de quy
int khongdequy(int n){
	if(n==1 ||n==0) {return 1;}
	int x[100] ={0};//dung mang tinh de luu gia tri
	x[0]=x[1]=1; //gia tri khoi dau
for (int i=2;i<=n;i++){ // i=2 vi hai gia tri i=0 va i=1 da biet truoc nen khong can tinh lai
	//lap quy tung gia tri cua i tu 2 den n
	x[i] =0; //khoi tao gia tri x[i] ve 0 truoc khi tinh toan
	for(int j=0;j<i;j++){ //tinh tong x[i] dua vao cong thuc truy hoi
	x[i] +=i*x[j]; //cong thuc tinh Xn
}} //vong lap i (ngoai) duyet qua tung gia tri, vong lap j (trong) tinh tong Xi cho tat ca j tu 0 den i-1
return x[n];} //tra ve gia tri cua Xn sau khi tinh toan


int main(){
	int n;
	cout<<"Nhap n: ";
	cin>>n;
	cout<<"gia tri x (de quy) la: "<<dequy(n)<<endl;
	cout<<"gia tri x (khong de quy) la: "<<khongdequy(n)<<endl;
	return 0;
}
