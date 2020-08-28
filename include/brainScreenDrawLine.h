void brainScreenBackground();      //繪製背景，運行一次就可以，不用開線程
void dataCollect();                //內部有無線循環，所以需要以線程方式在主程序運行 thread dataCollect1(dataCollect);
