#include "vex.h"
#include <math.h>
//brain screen的用戶可用分辨率為480 x 272,左上角為(0,0)
//真正給用戶編輯的豎直像素只有239，因為有默認title不讓編輯

/*--------------------------用戶輸入數值-------------------------------*/
int sampleWholeTime=5;           //單位s(秒)，用戶定義的總採樣時間，比如是10s，這個採樣時間關係到畫圖時屏幕的顯示範圍。
float maxDisplayG=1.5;          //單位 g (重力加速度) 用戶定義最大的加速度顯示的數量，如果是2，就是可以測量正負2g。正負4g是測量的極限。
                            //取值是1,2,3,4
float g=9.8;  
/*-------------------------------------------------------------------*/


float timerForDataColletct[99999];    //timerForDataColletct[count]  預設記錄99999個點
float accelerateY[99999];             //accelerateY[count] 預設記錄1000個點
float accelerateRobotY[99999];        //定義機器人真實的加速度
float differenceAccelerateY[99999];   //differenceAccelerateY定義accelerate的變化值預設記錄1000個點
float differenceAccelerateYFromInitial[99999];   //differenceAccelerateYFromInitial定義accelerate和最初的值的變化
float velocityY[99999];                //velocityY定義在Y方向的變化值預設記錄1000個點
float differenceDistanceY[99999];
float distanceY[99999];               //distanceY定義在Y方向的變化值預設記錄1000個點
uint8_t accelerateR[99999];




int colByEachSce=80/sampleWholeTime;      //計算每秒鐘的數據佔用的列數，目的是繪製X軸的坐標標籤。screen是20行，80列float 


/*--------------------screen背景坐標系的繪製-----------------------------*/
void brainScreenBackground()
{
  Brain.Screen.clearScreen(); 

  Brain.Screen.setPenColor(yellow);
  Brain.Screen.setFont(monoXS);           //用最小的字體寫字，最小的字體佔用5*8個像素。上下8個像素，左右5個像素


/*---------------------橫軸時間間隔---------------------------*/
//例如採用週期 sampleT=150msce，由以下時間間隔決定每隔2個像素畫一個點，比如時間間隔為150msce時有如下數據：
// 像素點     0   2    4    6    8     10   
// 單位msce   0  150  300  450  600   750    
//以此類推，每隔1秒相當於是1000msce/150msce=6.67，就是相關6.67*2=13個像素相隔1s，每隔65個像素相間隔5s
//根據以上推論，用戶可以自定義總採樣時間sampleWholeTime，如果sampleWholeTime=5s的話，相當於480個像素分為5s內採樣的總次數


  /*-----------------X軸的標籤--------------------------------*/
  //前面已經將80列按照採樣總時間分割，分割的段數為sampleWholeTime,每段所佔用的行數為colByEachSce
  //特別注意，setCursor(row,Col) 相當於坐標(y,x)，是相反的。 總共只有20行，80列
  for(int i=1;i<=sampleWholeTime;i++)
  {
   Brain.Screen.setCursor(20,i*colByEachSce-1);  //相當於在20行的地方，每隔一段距離寫一個數字。減去1的原因是因為有兩個字符，希望最後一列的字符現實完整。
   Brain.Screen.print(i);                        //i從1一直加到需要採樣的總時間
   Brain.Screen.print("s");
  }  


  /*-----------------y軸的標籤--------------------------------*/
  //寫Y軸的標籤軸，坐標單位要根據數據的比例做調整。
  //加速度有正負之分，最大只能測量正負4g，相當於可以測量8g的間隔，按照240個像素來分，每個g可以放大240/8=30倍
  //但是一般的機器人最大可以達到2g就非常厲害了，所以考慮設置一個選擇"maxDisplayG"，用戶定義最大顯示的範圍。
  int rowPixelByMaxG=240/(maxDisplayG*2);  //通過用戶定義的最大顯示G的範圍可以算出每個g顯示所佔用的像素。

  //繪製水平輔助線，橫向從第10格像素開始繪製，因為10格像素前有標籤
  //239個像素的編輯區域，取中間點是120，所以在120上定義為0點線

  Brain.Screen.setPenColor(green);  //繪製背景線條的顏色
  Brain.Screen.setPenWidth(1);      //線條寬度是1個像素
  Brain.Screen.drawLine(0, 120, 99999, 120); 
  
  Brain.Screen.setPenColor(green);
  
    for(int i=0;i<=maxDisplayG*2;i++)   //maxDisplayG就是用戶希望測量的g的範圍，如果值是1，那就需要循環畫線2次
  {
    Brain.Screen.drawLine(10, rowPixelByMaxG*i, 99999, rowPixelByMaxG*i);  //畫橫線，x=10開始畫，y方向以像素120為中點，每次增加一個區域
    Brain.Screen.setCursor(20/(maxDisplayG*2)*i+1,1);                     //因為總共是20行， 加1的原因是因為不能從0行開始顯示
    Brain.Screen.print(maxDisplayG-i);                           //用Brain.Screen.printAt的命令只能顯示固定字符，不能顯示變量，所以最後使用print
    Brain.Screen.print("g");
  }  

  
//用手畫方式如下： 
 /*  //由於高度總共只有239個像素，所以垂直方向按照30個像素，分割成8個部分。
  //繪製0點以下分界線和標籤
  Brain.Screen.drawLine(10, 150, 99999, 150);
  Brain.Screen.drawLine(10, 180, 99999, 180);
  Brain.Screen.drawLine(10, 210, 99999, 210);
  Brain.Screen.drawLine(10, 239, 99999, 239);
  Brain.Screen.printAt(0, 120, "0g");   
  Brain.Screen.printAt(0, 150, "-1g");      
  Brain.Screen.printAt(0, 180, "-2g");
  Brain.Screen.printAt(0, 210, "-3g");
  Brain.Screen.printAt(0, 240, "-4g");

  //繪製0點以上分界線和標籤
  Brain.Screen.drawLine(10, 90, 99999, 90);
  Brain.Screen.drawLine(10, 60, 99999, 60);
  Brain.Screen.drawLine(10, 30, 99999, 30);
  Brain.Screen.drawLine(10, 0, 99999, 0);  
  Brain.Screen.printAt(0, 90, "1g");      
  Brain.Screen.printAt(0, 60, "2g");
  Brain.Screen.printAt(0, 30, "3g");
  Brain.Screen.printAt(0, 8, "4g"); */
}

void dataCollect()
{
    int count=1;                   //數組計數的序號，比如timerForDataColletct[count] 不能在函數內部定義，否則每次都要重置為1

    timerForDataColletct[0]=0;     //時間由0開始
    accelerateY[0]=iner.acceleration(yaxis); //初始的accelerateY的值
    differenceAccelerateY[0]=0; //設定加速度計差的初始值
    velocityY[0]=0; //設定距離的初始值為0
    differenceDistanceY[0]=0; //設定距離的初始值為0
    int i = 0;
  while(1)
  {
  
  timerForDataColletct[count]=Brain.timer(msec);  //獲取系統時間，注意在獲取前，在主程序中需要Brain.Timer.reset();
                                                  //記錄的timerForDataColletct[count]由timerForDataColletct[1]開始，


  /*---------------------------------定義採樣週期-------------------------------------*/
  //當程序最後wait 20毫秒的時候，手柄上顯示數據採集的時間差是150ms 
  float sampleT=timerForDataColletct[count]-timerForDataColletct[count-1];   //兩次系統timer的差值，兩次採樣之間的時間差

  int sampleF=1000.0/sampleT;          //通過採樣週期計算的採樣頻率，相當於每秒採樣多少次 1000.0msec一定要加.0 否則會直接取整數
  int sampleNum=sampleWholeTime*sampleF+1;    //根據用戶輸入的採樣總時間，計算這段時間內總共要採樣多少次，直接將其變成一個整數，比計算的值多採樣1次
  float hPixelByEachData=480.0/sampleNum;   //計算每次採樣的數據點在屏幕橫軸上佔有的像素寬度，以填滿480像素計算

  float kTime=hPixelByEachData/sampleT;    //在畫圖中以週期timerForDataColletct[count]作為橫坐標，如果兩次timer的差為20ms，所以每次的值在橫坐標上相差20
                                       //但這個20要乘上一個比例，對應我們的480個像素。每個採樣點與每個採樣時間的比例。

  float kAccelerate=120/maxDisplayG;            //因為最大顯示的值為maxDisplayG可以顯示的最大數值為半個屏幕，所以可以放大。

  
  accelerateY[count]=iner.acceleration(yaxis);   //根據inertial上繪製的箭頭方向，向住箭頭方向的加速度為負值。
  //differenceAccelerateY[count]=accelerateY[count]-accelerateY[count-1];    //加速度增加的值
  //differenceAccelerateYFromInitial[count]=accelerateY[count]-accelerateY[0];  //加速度和最初值之間的差

  /* if(differenceAccelerateYFromInitial[count]<0.02 && differenceAccelerateYFromInitial[count]>-0.02)   //在這個範圍內認為加速度沒有變化，是通過觀察而來的。
  {
  differenceAccelerateY[count]=0;
  }
  else
  {Brain.Screen.setCursor(7,1);
  Brain.Screen.print(differenceAccelerateY[count]);}  */
  
  //accelerateY[0]=0.0;
  //accelerateY[count]=accelerateY[count-1]+differenceAccelerateY[count]; 
  accelerateRobotY[count]=accelerateY[count]-accelerateY[0]; 
  //accelerateRobotY[count]=0;

  if(accelerateRobotY[count]>-0.01 && accelerateRobotY[count]<0.01)   //在這個範圍內認為真實的機器人加速度沒有變化，是通過觀察而來的。
  {
    accelerateRobotY[count] = 0;  
    i++;    
    Brain.Screen.print("%d",i); 
    Brain.Screen.setCursor(13,1);
    velocityY[count]=velocityY[count-1];

    if(i>10)                            //希望如果檢測到10次都沒有變化，我們就認為機器人是靜止的，不是勻速運動。這樣就直接將速度設為0
    {
      i=0;
      velocityY[count]=0;

    }
  } 
  else                              //如果檢測到加速度又有變化，則i先設為0，開始重新就算
  {
    i = 0;                               
    Brain.Screen.setCursor(13,1);
    Brain.Screen.print("%d",i); 

    velocityY[count]=velocityY[count-1]+accelerateRobotY[count]*g*sampleT/1000.0;  //經過一個sampleT週期結束時的速度

  }
 
 

  //velocityY[count]=velocityY[count-1]+accelerateRobotY[count]*g*sampleT/1000.0;  //經過一個sampleT週期結束時的速度
  differenceDistanceY[count]=velocityY[count]*sampleT/1000.0; //計算每個sampleT的週期內對距離的貢獻值
  distanceY[count]=distanceY[count-1]+differenceDistanceY[count];   
  

  Brain.Screen.setPenWidth(1);   
  Brain.Screen.setPenColor(red);  //用紅色線畫圖形
  //根據測試“Controller1.Screen.print(timerForDataColletct[count]-timerForDataColletct[count-1]);” 
  //當程序最後wait 20毫秒的時候，手柄上顯示數據採集的時間差是前面用戶定義的sampleT參數，比如：150ms 
  //由於採樣的總時間是確定的，所以每次採樣相距的像素點為hPixelByEachData
  //如果希望間隔3個像素畫一個點，timerForDataColletct[count]要乘以係數3/150=0.02
  //如果希望間隔2個像素畫一個點，timerForDataColletct[count]要乘以係數2/150=0.013
  //如果希望間隔1個像素畫一個點，timerForDataColletct[count]要乘以係數1/150=0.0067
  //最後選取間隔2個像素畫一個點
  //因為每次畫圖是用採樣週期sampleT作為橫坐標，必須縮小比例才能用480個像素覆蓋整個採樣的時間內，


  Brain.Screen.drawLine(timerForDataColletct[count-1]*kTime, differenceAccelerateY[count-1]*kAccelerate+120, timerForDataColletct[count]*kTime, differenceAccelerateY[count]*kAccelerate+120);





  /*-------------------------在手柄上顯示Y軸加速傳感器數值，以及採樣的時間差-----------------------------------------*/
  /* Controller1.Screen.clearScreen();
  Controller1.Screen.setCursor(1,1);
  Controller1.Screen.print(accelerateY[count]);          //在controller上顯示獲取的加速度值,但是數據的傳輸會嚴重減慢採樣頻率。
  Controller1.Screen.setCursor(2,1);
  Controller1.Screen.print(sampleT);   //可以測試出timer中間間隔的時間 150msec ，150msce，該數值需要運行一次程序，看手柄上的顯示時間。
  Controller1.Screen.setCursor(2,6); */

  
  Brain.Screen.setCursor(1,1);             
  Brain.Screen.print(sampleT);          //在brain上顯示獲取的加速度值,但是數據的傳輸會嚴重減慢採樣頻率。
  Brain.Screen.setCursor(3,1);
  Brain.Screen.print("%f",accelerateY[0]); 
  //Brain.Screen.print(accelerateY[0]); 


  Brain.Screen.setCursor(5,1);
  Brain.Screen.print("%f",accelerateY[count]); 
  //Brain.Screen.print(accelerateY[count]); 
 
  //accelerateRobotY[count]=0;
  Brain.Screen.setCursor(7,1);
  Brain.Screen.print("%.10f", accelerateRobotY[count]);
  //Brain.Screen.print("%f --",accelerateRobotY[count]);
  //Brain.Screen.print(accelerateY[count]); 

  Brain.Screen.setCursor(9,1);
  Brain.Screen.print("%f",velocityY[count]); 
  //Brain.Screen.print(velocityY[count]); 



  Brain.Screen.setCursor(11,1);
  //Brain.Screen.print("%f",distanceY[count]); 
  Brain.Screen.print(distanceY[count]);

  //Brain.SDcard.savefile("0abc",(uint8_t*)timerForDataColletct,sizeof(timerForDataColletct));     
  //Brain.SDcard.savefile("1abc",(uint8_t*)accelerateY,sizeof(accelerateY)); 

  count++;
  wait(20,msec);

  //Brain.Screen.clearScreen();

  }

}


