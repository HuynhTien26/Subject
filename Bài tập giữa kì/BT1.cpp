#include <iostream>
#include <fstream>
#include <climits> //su dung INT_MIN

using namespace std;
#define F1 "test.txt"


void findmaxtwo(int **A, int n, int m, int &max1, int &max2){ //cau b tim so lon nhat, lon nhi
	max1=INT_MIN; //khoi tao max1 va max2 co gia tri  nho nhat co the co cua int
	max2=INT_MIN;
	for(int i=0;i<n;i++){ //duyet tung phan tu trong ma tran
		for(int j=0;j<m;j++){
			int val =A[i][j]; //luu gia tri phan tu hien tai vaa val 
			
			if(val> max1){ //gia tri hien tai lon hon max1
				max2=max1; //luu gia tri cu cua max1 vao max2
				max1=val; //cho max1 thanh gia tri hien tai
			}
			else if (val>max2 &&val<max1){ //neu gia tri hien tai nho hon max1 & lon hon max2
				max2=val; //gan gia tri vao max2
			}
			}
			}
}


void tongsotrencol (int **A, int n, int m, int *kq){ //cau c:tinh tong cac phan tu tren moi cot
    for(int j=0; j<m;j++){ //duyet cot
    	kq[j]=0; //cho tong so tren cot hien tai la 0
    	for(int i=0; i<n;i++){ //duyet hang
    		kq[j]+=A[i][j]; //cong don cac phan tu cot j
		}
	}
	}
	
  //tim so nho nhat tren hang 	
void findminnumber(int **A, int n, int m, int *minnumber){
	for(int i=0; i<n;i++){
	    minnumber[i]= A[i][0]; //gan gia tri dau tien cua hang lam moc so sanh
	for (int j=1;j<m;j++){ //do phan tu dau tien cua hang lam moc so sanh -> duyet tu cot thu 2 tro di
		if(A[i][j] <minnumber[i]){
			minnumber[i] =A[i][j];
		}
		}
	}
}
 //cau d: tao mang c tu a va minnumber
void mangC(int **A, int n, int m, int *minnumber, int **&C){
	C = new int*[n]; //tap mang 2 chieu dong cho C
	for(int i=0;i<n;i++){
		C[i] =new int[m]; //cap phat dong 1 mang co m phan tu kieu int
		for (int j=0;j<m;j++){
			C[i][j] =A[i][j] * minnumber[i]; //gan gia tri vao mang C
	}
	}			
	}
    

int main(){
	int max1, max2; //bien luu gia tri lon nhat, lon nhi
	
	FILE* fptr =fopen(F1, "r"); //mo file de doc du lieu
	if(fptr ==NULL){ //neu file rong thi khong mo duoc file
		cout<<"Khong the mo file";
	return 1; 
}	
	int n,m;
	fscanf(fptr, "%d %d", &n, &m);	//doc gia tri n, m tu file
	//tao mang 2 chieu
	int **A = new int*[n]; //tao mang con tro cap 1(chua con tro den hang)
	for (int i=0; i<n; i++){ 
		A[i]= new int[m]; //tao tung hang la mang dong co m phan tu
	}
	
	//doc du lieu trong file
	for(int i=0;i<n;i++){
		for (int j=0; j<m; j++){
		 fscanf(fptr,"%d", &A[i][j]);}
	}
	fclose(fptr); //dong file
	
//	//in ma tran ra man hinh
//	for (int i=0; i<n;i++){
//		for(int j=0;j<m;j++){
//			cout<<A[i][j]<<" ";}
//		cout<<endl;}

	
	//cau b: in so lon nhat va lon nhi ra man hinh
	//goi ham
	cout<<"Cau b: tim so lon nhat va lon nhi"<<endl;
	int *minnumber =new int[n];
	findmaxtwo(A, n, m, max1, max2);
		cout<<"So lon nhat la: "<<max1<<endl;
		cout<<"So lon nhi la: "<<max2<<endl;
	cout<<endl;
    
    //cau c: goi ham tinh tong so tren moi cot
    cout<<"cau c: tinh tong so tren moi cot"<<endl;
    int* tongcot=new int[n]; //cap phat bo nho dong de luu tong tung cot
    tongsotrencol(A,n,m,tongcot); //goi ham tinh tong tung cot
    cout<<"tong so tren cot: "<<endl;
    for(int j=0;j<m;j++){
    	cout<<tongcot[j]<<endl;  //in ra tong tren moi cot
	}
	cout<<endl;
    
//    int *minnumber = new int[n];
//    findminnumber(A, n, m, minnumber);
//    cout<<"so nho nhat tren hang la: "<<endl;
//    for (int i=0;i<n;i++){
//    	cout<<minnumber[i]<<endl;
//}


    //cau d: tao mang c tu mang a *k (k la so nho nhat)
    cout<<"Cau d: tao mang c tu mang a *k"<<endl;
    findminnumber(A,n,m,minnumber); //goi ham tim so nho nhat trong 1 hang
    int **C; 
	mangC(A, n, m, minnumber, C);
    
    cout<<"Mang C: "<<endl;
    for(int i=0;i<n;i++){
    	for(int j=0;j<m;j++){
    		cout<<C[i][j]<<" "; //in mang C ra man hinh
		}
		cout<<endl;
	}
    
	//giai phong bo nho	
	for(int i=0;i<n;i++){ //giai phong tung hang cua mang 2 chieu dong
	delete[] A[i];
	delete[] C[i];
}

    //giai phong con tro cap 1 cua mang 2 chieu dong
	delete[] A;
	delete[] C;
	//giai phong cac mang dong 1 chieu
	delete[] tongcot;
	delete[] minnumber;
	 return 0;
}
