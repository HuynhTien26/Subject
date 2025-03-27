#include <iostream>
#include <cstring> //thu vien de su dung string (chuoi ki tu)
#include <sstream> //thu vien de su dung stringstream

using namespace std;

bool chuyenkitu(char c){
	c=tolower(c);  //chuyen ki tu ve dang chu thuong de so sanh
	return(c=='u' || c=='e' ||c=='o' ||c=='a'||c=='i'); //neu c thuoc cac chu cai u,e,o,a,i thi` tra ve true
}

	
int main(){
string str;
cout<<"Nhap vao chuoi ki tu: ";
getline(cin, str); //nhap ca dong, ke ca dau cach

//cau a dem trong chuoi co bao nhieu nguyen am, bao nhieu phu am
int nguyenam=0, phuam=0;
for(char c : str){ //duyet qua tung phan tu c trong chuoi ki tu str
	if(isalpha(c)){//kiem tra neu la chu cai, neu phai thi tiep tuc kiem tra phu am, nguyen am
      if(chuyenkitu(c)){ // chuyen c ve ki tu thuong va kiem tra
      	nguyenam++; //neu la nguyen am thi tang bien nguyen am
	  }
	  else 
	  phuam++; //la phu am thi tang bien phu am
	}
} //chay hetvong for la da duyet het cac ki tu trong chuoi str

cout<<"Cau a: dem cac ki tu nguyen am va phu am trong chuoi"<<endl;
cout<<"So ki tu nguyen am trong chuoi: "<<nguyenam<<endl;
cout<<"So ki tu phu am trong chuoi: "<<phuam<<endl;
cout<<endl;


//cau b tach chuoi thanh cac tu va dem chuoi co bao nhieu tu, moi tu trong chuoi co bao nhieu ki tu
stringstream ss(str); //tao mot stringstream tu chuoi nhap vao
int sotu=0; //bien dem so tu
string tu[100]; //mang luu cac tu trong chuoi
cout<<"Cau b: dem so tu va moi tu co bao nhieu ki tu"<<endl;
cout<<"So ki tu trong moi tu la"<<endl;

while (ss>> tu[sotu]){ //duyet tung tu trong chuoi
	sotu++;
	cout<<"Tu thu "<<sotu<<"("<<tu[sotu-1]<<") co "<<tu[sotu -1].length()<<" ki tu"<<endl;
	}
cout<<"So tu trong chuoi la: "<<sotu<<endl;
cout<<endl;


//cau c:dem xem moi tu trong chuoi xuat hien bao nhieu lan
cout<<"cau c: so lan xuat hien cua moi tu"<<endl;
bool dadem[100]={false}; //mang danh dau xem tu da duoc dem hay chua

for(int i=0;i<sotu;i++){
	if(!dadem[i]){ //neu tu chua duoc dem
		int dem=1; //dang xet tai tu[i] nen chac chan no xuat hien 1 lan
		for(int j=i+1;j<sotu;j++){ //j=i+1 do neu j=0 se dem lai cac tu da kiem tra truoc do gay ket qua sai
			if(tu[i]==tu[j]){ //tu[i] la tu dang xet, tu[j] la tu tiep theo trong danh sach, neu ca 2 giong nhau thi dem tang 1
				dem++;
				dadem[j]=true; //danh dau tu da duoc dem
			}
		}
		cout<<"Tu ("<<tu[i]<<") xuat hien "<<dem<<" lan "<<endl;
			}
}
cout<<endl;


//cau d: nhap vao mot ki tu, xoa tat ca vi tri ma ki tu do xuat hien trong chuoi
cout<<"cau d: nhap vao mot ki tu, xoa tat ca vi tri ma ki tu do xuat hien trong chuoi "<<endl;
cout<<"Nhap vao ki tu muon xoa "<<endl;

char kitucanxoa;
cin>>kitucanxoa; //nhap ki tu can xoa
string result =""; //luu tru chuoi sau kkhi xoa ki tu, "" la gan gia tri ban dau la chuoi rong
for(char c: str){
	if(c!=kitucanxoa){ //neu ki tu khong phai tu can xoa thi giu lai
		result +=c;
	}
}
//in ra chuoi sau khi xoa di ki tu
cout<<"chuoi sau khi xoa di ki tu "<<kitucanxoa<<" : "<<result<<endl;


return 0;
}
